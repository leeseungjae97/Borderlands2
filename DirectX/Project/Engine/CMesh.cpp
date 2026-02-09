#include "pch.h"
#include "CMesh.h"

#include "CAnimator3D.h"
#include "CDevice.h"
#include "CInstancingBuffer.h"
#include "CPathMgr.h"
#include "CResMgr.h"
#include "mFBXLoader.h"

#include "CStructuredBuffer.h"
#include "IndividualBoneSkinningShader.h"
#include "physx_util.h"

CMesh::CMesh(bool _bEngine)
	: CRes(RES_TYPE::MESH, _bEngine)
	, m_tVBDesc{}
	, m_VtxCount(0)
	, m_vecIdxInfo{}
	, m_pVtxSys(nullptr)
{
}

CMesh::~CMesh()
{
	if (nullptr != m_pVtxSys)
		delete m_pVtxSys;

	for (size_t i = 0; i < m_vecIdxInfo.size(); ++i)
	{
		if (nullptr != m_vecIdxInfo[i].pIdxSysMem)
			delete m_vecIdxInfo[i].pIdxSysMem;
	}

	if (nullptr != m_pBoneFrameData)
		delete m_pBoneFrameData;

	if (nullptr != m_pBoneOffset)
		delete m_pBoneOffset;

	if (nullptr != m_pBlendFrameData)
		delete m_pBlendFrameData;
}


CMesh* CMesh::CreateFromContainer(FBXLoader& _loader)
{
	const tContainer* container = &_loader.GetContainer(0);
	
	//container->
	UINT iVtxCount = (UINT)container->vecPos.size();

	D3D11_BUFFER_DESC tVtxDesc = {};

	tVtxDesc.ByteWidth = sizeof(Vtx) * iVtxCount;
	tVtxDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	tVtxDesc.Usage = D3D11_USAGE_DEFAULT;
	if (D3D11_USAGE_DYNAMIC == tVtxDesc.Usage)
		tVtxDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA tSub = {};
	tSub.pSysMem = malloc(tVtxDesc.ByteWidth);
	Vtx* pSys = (Vtx*)tSub.pSysMem;
	for (UINT i = 0; i < iVtxCount; ++i)
	{
		pSys[i].vPos = container->vecPos[i];
		pSys[i].vUV = container->vecUV[i];
		pSys[i].vColor = Vec4(1.f, 0.f, 1.f, 1.f);
		pSys[i].vNormal = container->vecNormal[i];
		pSys[i].vTangent = container->vecTangent[i];
		pSys[i].vBinormal = container->vecBinormal[i];
		pSys[i].vWeights = container->vecWeights[i];
		pSys[i].vIndices = container->vecIndices[i];
	}

	ComPtr<ID3D11Buffer> pVB = NULL;
	if (FAILED(DEVICE->CreateBuffer(&tVtxDesc, &tSub, pVB.GetAddressOf())))
	{
		return NULL;
	}

	CMesh* pMesh = new CMesh;
	pMesh->m_VB = pVB;
	pMesh->m_tVBDesc = tVtxDesc;
	pMesh->m_pVtxSys = pSys;
	pMesh->m_VtxCount = iVtxCount;

	// �ε��� ����
	UINT iIdxBufferCount = (UINT)container->vecIdx.size();
	D3D11_BUFFER_DESC tIdxDesc = {};

	for (UINT i = 0; i < iIdxBufferCount; ++i)
	{
		tIdxDesc.ByteWidth = (UINT)container->vecIdx[i].size() * sizeof(UINT); // Index Format �� R32_UINT �̱� ����
		tIdxDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		tIdxDesc.Usage = D3D11_USAGE_DEFAULT;
		if (D3D11_USAGE_DYNAMIC == tIdxDesc.Usage)
			tIdxDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		void* pSysMem = malloc(tIdxDesc.ByteWidth);
		memcpy(pSysMem, container->vecIdx[i].data(), tIdxDesc.ByteWidth);
		tSub.pSysMem = pSysMem;

		ComPtr<ID3D11Buffer> pIB = nullptr;
		if (FAILED(DEVICE->CreateBuffer(&tIdxDesc, &tSub, pIB.GetAddressOf())))
		{
			return NULL;
		}

		tIndexInfo info = {};
		info.tIBDesc = tIdxDesc;
		info.iIdxCount = (UINT)container->vecIdx[i].size();
		info.pIdxSysMem = pSysMem;
		info.pIB = pIB;

		pMesh->m_vecIdxInfo.push_back(info);
	}

	// Animation3D
	if (!container->bAnimation)
		return pMesh;

	vector<tBone*>& vecBone = _loader.GetBones();
	//vector<vector<vector<tKeyFrame>>>& vecKeyframe= _loader.GetKeyFrame();
	vector<tAnimClip*>& vecAnimClip = _loader.GetAnimClip();

	for (UINT i = 0; i < vecAnimClip.size(); ++i)
	{
		tMTAnimClip tClip = {};

		tClip.strAnimName = vecAnimClip[i]->strName;
		tClip.dStartTime = vecAnimClip[i]->tStartTime.GetSecondDouble();
		tClip.dEndTime = vecAnimClip[i]->tEndTime.GetSecondDouble();
		tClip.dTimeLength = tClip.dEndTime - tClip.dStartTime;

		tClip.iStartFrame = (int)vecAnimClip[i]->tStartTime.GetFrameCount(vecAnimClip[i]->eMode);
		tClip.iEndFrame = (int)vecAnimClip[i]->tEndTime.GetFrameCount(vecAnimClip[i]->eMode);
		tClip.iFrameLength = tClip.iEndFrame - tClip.iStartFrame;
		tClip.eMode = vecAnimClip[i]->eMode;

		//pMesh->m_mapAnimClip.push_back(tClip);
		pMesh->m_mapAnimClip.insert(make_pair(tClip.strAnimName, tClip));
	}

	//pMesh->m_veciFrameCount.resize(vecAnimClip.size());

	UINT iFrameCount = 0;

	for (UINT i = 0; i < vecBone.size(); ++i)
	{
		tMTBone bone = {};
		bone.iDepth = vecBone[i]->iDepth;
		bone.iParentIndx = vecBone[i]->iParentIndx;
		bone.matBone = GetMatrixFromFbxMatrix(vecBone[i]->matBone);
		bone.matOffset = GetMatrixFromFbxMatrix(vecBone[i]->matOffset);
		bone.strBoneName = vecBone[i]->strBoneName;
		bone.vBonePos = Vec4(vecBone[i]->vBonePos.mData[0], vecBone[i]->vBonePos.mData[1]
			, vecBone[i]->vBonePos.mData[2], vecBone[i]->vBonePos.mData[3]);

		for (UINT j = 0; j < vecAnimClip.size(); ++j)
		{
			wstring animName = vecAnimClip[j]->strName;

			if (vecBone[i]->vecKeyFrame.size() == 0) continue;

			vector<tKeyFrame> frame = vecBone[i]->vecKeyFrame.at(animName);
			vector<tMTKeyFrame> vecMTKeyFrames;

			for (UINT k = 0; k < frame.size(); ++k)
			{
				tKeyFrame _tKeyFrame = frame[k];
				tMTKeyFrame _tMTKeyframe = {};
				_tMTKeyframe.dTime = _tKeyFrame.dTime;
				_tMTKeyframe.iFrame = k;
				_tMTKeyframe.vTranslate.x = (float)_tKeyFrame.matTransform.GetT().mData[0];
				_tMTKeyframe.vTranslate.y = (float)_tKeyFrame.matTransform.GetT().mData[1];
				_tMTKeyframe.vTranslate.z = (float)_tKeyFrame.matTransform.GetT().mData[2];

				_tMTKeyframe.vScale.x = (float)_tKeyFrame.matTransform.GetS().mData[0];
				_tMTKeyframe.vScale.y = (float)_tKeyFrame.matTransform.GetS().mData[1];
				_tMTKeyframe.vScale.z = (float)_tKeyFrame.matTransform.GetS().mData[2];
				
				_tMTKeyframe.qRot.x = (float)_tKeyFrame.matTransform.GetQ().mData[0];
				_tMTKeyframe.qRot.y = (float)_tKeyFrame.matTransform.GetQ().mData[1];
				_tMTKeyframe.qRot.z = (float)_tKeyFrame.matTransform.GetQ().mData[2];
				_tMTKeyframe.qRot.w = (float)_tKeyFrame.matTransform.GetQ().mData[3];

				vecMTKeyFrames.push_back(_tMTKeyframe);
			}

			bone.vecKeyFrame.insert(make_pair(animName, vecMTKeyFrames));


			//pMesh->m_mapAnimClip[j].iFrameCount = (max(iFrameCount, (UINT)frame.size()));
			pMesh->m_mapAnimClip.at(animName).iFrameCount = (max(iFrameCount, (UINT)frame.size()));
		}
		pMesh->m_vecBones.push_back(bone);
	}
	// Animation �� �ִ� Mesh ��� structuredbuffer �����α�
	if (pMesh->IsAnimMesh())
	{
		TransKeyFrame(pMesh);

		if (nullptr == pMesh->m_pBoneOffset) pMesh->m_pBoneOffset = new CStructuredBuffer;
		pMesh->m_pBoneOffset->Create(sizeof(Matrix), (UINT)pMesh->m_vecBoneOffset.size(), SB_TYPE::READ_ONLY, false, pMesh->m_vecBoneOffset.data());

		if (nullptr == pMesh->m_pBoneFrameData) pMesh->m_pBoneFrameData = new CStructuredBuffer;
		if (nullptr == pMesh->m_pBlendFrameData) pMesh->m_pBlendFrameData = new CStructuredBuffer;
	}

	return pMesh;
}
CStructuredBuffer* CMesh::GetBoneFrameDataBuffer(const wstring& _AnimName)
{
	//tMTAnimClip clip = m_mapAnimClip[_Idx];
	tMTAnimClip clip = m_mapAnimClip.at(_AnimName);
	UINT iElementCount = (UINT)m_vecBoneOffset.size() * clip.iFrameCount;

	if (nullptr == m_pBoneFrameData) m_pBoneFrameData = new CStructuredBuffer;

	if (m_pBoneFrameData->GetElementCount() < iElementCount)
	{
		m_pBoneFrameData->Create(sizeof(tFrameTrans), iElementCount, SB_TYPE::READ_ONLY, true);
	}
	m_pBoneFrameData->SetData((void*)clip.vecTransKeyFrame.data());
	return m_pBoneFrameData;
}

CStructuredBuffer* CMesh::GetBlendFrameDataBuffer(const wstring& _AnimName)
{
	//tMTAnimClip clip = m_mapAnimClip[_Idx];
	tMTAnimClip clip = m_mapAnimClip.at(_AnimName);
	UINT iElementCount = (UINT)m_vecBoneOffset.size() * clip.iFrameCount;

	if (nullptr == m_pBlendFrameData) m_pBlendFrameData = new CStructuredBuffer;

	if (m_pBlendFrameData->GetElementCount() < iElementCount)
	{
		m_pBlendFrameData->Create(sizeof(tFrameTrans), iElementCount, SB_TYPE::READ_ONLY, true);
	}
	m_pBlendFrameData->SetData((void*)clip.vecTransKeyFrame.data());
	return m_pBlendFrameData;
}


void CMesh::Create(void* _VtxSysMem, UINT _iVtxCount, void* _IdxSysMem, UINT _IdxCount)
{
	m_VtxCount = _iVtxCount;

	tIndexInfo indexInfo = {};
	indexInfo.iIdxCount = _IdxCount;

	// ���� �����͸� ������ ���ؽ� ���۸� �����Ѵ�.
	m_tVBDesc.ByteWidth = sizeof(Vtx) * m_VtxCount;

	m_tVBDesc.CPUAccessFlags = 0;
	m_tVBDesc.Usage = D3D11_USAGE_DEFAULT;

	// Vertex ���� ����
	m_tVBDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;
	m_tVBDesc.MiscFlags = 0;
	m_tVBDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA tSub = {};
	tSub.pSysMem = _VtxSysMem;
	if (FAILED(DEVICE->CreateBuffer(&m_tVBDesc, &tSub, m_VB.GetAddressOf())))
	{
		assert(nullptr);
	}

	// Index ���� ����
	indexInfo.tIBDesc.ByteWidth = sizeof(UINT) * indexInfo.iIdxCount;
	indexInfo.tIBDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER;
	indexInfo.tIBDesc.CPUAccessFlags = 0;
	indexInfo.tIBDesc.Usage = D3D11_USAGE_DEFAULT;

	tSub = {};
	tSub.pSysMem = _IdxSysMem;
	if (FAILED(DEVICE->CreateBuffer(&indexInfo.tIBDesc, &tSub, indexInfo.pIB.GetAddressOf())))
	{
		assert(nullptr);
	}


	// SystemMem ������ ����
	m_pVtxSys = new Vtx[m_VtxCount];
	memcpy(m_pVtxSys, _VtxSysMem, sizeof(Vtx) * m_VtxCount);

	indexInfo.pIdxSysMem = new UINT[indexInfo.iIdxCount];
	memcpy(indexInfo.pIdxSysMem, _IdxSysMem, sizeof(UINT) * indexInfo.iIdxCount);

	m_vecIdxInfo.push_back(indexInfo);
}

void CMesh::TransKeyFrame(CMesh* mesh)
{
	for (size_t i = 0; i < mesh->m_vecBones.size(); ++i)
	{
		tMTBone _bone = mesh->m_vecBones[i];

		mesh->m_vecBoneOffset.push_back(_bone.matOffset);

		for (auto& pair : mesh->m_mapAnimClip)
		{
			tMTAnimClip clip = pair.second;
			wstring animName = clip.strAnimName;

			clip.vecTransKeyFrame
				.resize((UINT)mesh->m_vecBones.size() * clip.iFrameCount);

			if (_bone.vecKeyFrame.size() == 0) continue;

			vector<tMTKeyFrame> vecMTKeyFrame = _bone.vecKeyFrame.at(animName);

			for (size_t j = 0; j < vecMTKeyFrame.size(); ++j)
			{
				clip.vecTransKeyFrame[(UINT)mesh->m_vecBones.size() * j + i] = {
					Vec4(vecMTKeyFrame[j].vTranslate, 0.f),
					Vec4(vecMTKeyFrame[j].vScale, 0.f),
					vecMTKeyFrame[j].qRot
				};
			}
			pair.second = clip;
		}
	}
}

void CMesh::UpdateData(UINT _iSubset)
{
	UINT iStride = sizeof(Vtx);
	UINT iOffset = 0;

	CONTEXT->IASetVertexBuffers(0, 1, m_VB.GetAddressOf(), &iStride, &iOffset);
	CONTEXT->IASetIndexBuffer(m_vecIdxInfo[_iSubset].pIB.Get(), DXGI_FORMAT_R32_UINT, 0);
}

// Subset ����
void CMesh::UpdateData_Instancing(UINT _iSubSet)
{
	if (_iSubSet >= m_vecIdxInfo.size())
		assert(nullptr);

	ID3D11Buffer* arrBuffer[2] = { m_VB.Get(), CInstancingBuffer::GetInst()->GetBuffer().Get() };
	UINT		  iStride[2] = { sizeof(Vtx), sizeof(tInstancingData) };
	UINT		  iOffset[2] = { 0, 0 };

	CONTEXT->IASetVertexBuffers(0, 2, arrBuffer, iStride, iOffset);
	CONTEXT->IASetIndexBuffer(m_vecIdxInfo[_iSubSet].pIB.Get(), DXGI_FORMAT_R32_UINT, 0);
}

void CMesh::render(UINT _iSubset)
{
	UpdateData(_iSubset);

	CONTEXT->DrawIndexed(m_vecIdxInfo[_iSubset].iIdxCount, 0, 0);
}

void CMesh::render_instancing(UINT _iSubSet)
{
	UpdateData_Instancing(_iSubSet);
	
	CONTEXT->DrawIndexedInstanced(m_vecIdxInfo[_iSubSet].iIdxCount
		, CInstancingBuffer::GetInst()->GetInstanceCount(), 0, 0, 0);
}

void CMesh::render_particle(UINT _iParticleCount)
{
	UpdateData(0);

	CONTEXT->DrawIndexedInstanced(m_vecIdxInfo[0].iIdxCount, _iParticleCount, 0, 0, 0);
}

int CMesh::Load(const wstring& _strFilePath)
{
	// �б���� ���Ͽ���
	FILE* pFile = nullptr;
	_wfopen_s(&pFile, _strFilePath.c_str(), L"rb");

	// Ű��, �����
	wstring strName, strKey, strRelativePath;
	LoadWString(strName, pFile);
	LoadWString(strKey, pFile);
	LoadWString(strRelativePath, pFile);

	SetName(strName);
	SetKey(strKey);
	SetRelativePath(strRelativePath);

	// ����������
	UINT iByteSize = 0;
	fread(&m_VtxCount, sizeof(UINT), 1, pFile);
	fread(&iByteSize, sizeof(int), 1, pFile);

	m_pVtxSys = (Vtx*)malloc(iByteSize);
	fread(m_pVtxSys, 1, iByteSize, pFile);


	D3D11_BUFFER_DESC tDesc = {};
	tDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	tDesc.ByteWidth = iByteSize;
	tDesc.Usage = D3D11_USAGE_DEFAULT;

	D3D11_SUBRESOURCE_DATA tSubData = {};
	tSubData.pSysMem = m_pVtxSys;

	if (FAILED(DEVICE->CreateBuffer(&tDesc, &tSubData, m_VB.GetAddressOf())))
	{
		assert(nullptr);
	}

	// �ε��� ����
	UINT iMtrlCount = 0;
	fread(&iMtrlCount, sizeof(int), 1, pFile);

	for (UINT i = 0; i < iMtrlCount; ++i)
	{
		tIndexInfo info = {};
		fread(&info, sizeof(tIndexInfo), 1, pFile);

		UINT iByteWidth = info.iIdxCount * sizeof(UINT);

		void* pSysMem = malloc(iByteWidth);
		info.pIdxSysMem = pSysMem;
		fread(info.pIdxSysMem, iByteWidth, 1, pFile);

		tSubData.pSysMem = info.pIdxSysMem;

		if (FAILED(DEVICE->CreateBuffer(&info.tIBDesc, &tSubData, info.pIB.GetAddressOf())))
		{
			assert(nullptr);
		}

		m_vecIdxInfo.push_back(info);
	}

	// Animation3D ���� �б�
	int iCount = 0;
	fread(&iCount, sizeof(int), 1, pFile);
	for (int i = 0; i < iCount; ++i)
	{
		tMTAnimClip tClip = {};

		LoadWString(tClip.strAnimName, pFile);
		fread(&tClip.dStartTime, sizeof(double), 1, pFile);
		fread(&tClip.dEndTime, sizeof(double), 1, pFile);
		fread(&tClip.dTimeLength, sizeof(double), 1, pFile);
		fread(&tClip.eMode, sizeof(int), 1, pFile);
		fread(&tClip.fUpdateTime, sizeof(float), 1, pFile);
		fread(&tClip.iStartFrame, sizeof(int), 1, pFile);
		fread(&tClip.iEndFrame, sizeof(int), 1, pFile);
		fread(&tClip.iFrameLength, sizeof(int), 1, pFile);
		fread(&tClip.iFrameCount, sizeof(int), 1, pFile);

		//m_mapAnimClip.push_back(tClip);
		m_mapAnimClip.insert(make_pair(tClip.strAnimName, tClip));
	}

	iCount = 0;
	fread(&iCount, sizeof(int), 1, pFile);
	m_vecBones.resize(iCount);

	for (int i = 0; i < iCount; ++i)
	{
		LoadWString(m_vecBones[i].strBoneName, pFile);
		fread(&m_vecBones[i].iDepth, sizeof(int), 1, pFile);
		fread(&m_vecBones[i].iParentIndx, sizeof(int), 1, pFile);
		fread(&m_vecBones[i].matBone, sizeof(Matrix), 1, pFile);
		fread(&m_vecBones[i].matOffset, sizeof(Matrix), 1, pFile);

		size_t count = 0;
		fread(&count, sizeof(size_t), 1, pFile);
		for (size_t j = 0; j < count; ++j)
		{
			wstring animName;
			LoadWString(animName, pFile);
			size_t vecSize;
			fread(&vecSize, sizeof(size_t), 1, pFile);
			vector<tMTKeyFrame> vectMT;
			for (size_t k = 0; k < vecSize; ++k)
			{
				tMTKeyFrame _tMTKeyFrame;
				fread(&_tMTKeyFrame, sizeof(tMTKeyFrame), 1, pFile);
				vectMT.push_back(_tMTKeyFrame);
			}

			m_vecBones[i].vecKeyFrame.insert(make_pair(animName, vectMT));
		}
	}

	if (m_mapAnimClip.size() > 0 && m_vecBones.size() > 0)
	{
		TransKeyFrame(this);

		if (nullptr == m_pBoneOffset) m_pBoneOffset = new CStructuredBuffer;
		m_pBoneOffset->Create(sizeof(Matrix), (UINT)m_vecBoneOffset.size(), SB_TYPE::READ_ONLY, false, m_vecBoneOffset.data());

		if (nullptr == m_pBoneFrameData) m_pBoneFrameData = new CStructuredBuffer;
		if (nullptr == m_pBlendFrameData) m_pBlendFrameData = new CStructuredBuffer;
	}

	fclose(pFile);

	return S_OK;
}

int CMesh::Save(const wstring& _strRelativePath)
{
	// ����� ����
	SetRelativePath(_strRelativePath);

	// ���� ��� �����
	wstring strFilePath = CPathMgr::GetInst()->GetContentPath() + _strRelativePath;

	// ���� ������� ����
	FILE* pFile = nullptr;
	errno_t err = _wfopen_s(&pFile, strFilePath.c_str(), L"wb");
	assert(pFile);

	// Ű��, ��� ���	
	SaveWString(GetName(), pFile);
	SaveWString(GetKey(), pFile);
	SaveWString(GetRelativePath(), pFile);

	// ���� ������ ����
	fwrite(&m_VtxCount, sizeof(UINT), 1, pFile);
	int iByteSize = m_tVBDesc.ByteWidth;
	fwrite(&iByteSize, sizeof(int), 1, pFile);
	fwrite(m_pVtxSys, iByteSize, 1, pFile);

	// �ε��� ����
	UINT iMtrlCount = (UINT)m_vecIdxInfo.size();
	fwrite(&iMtrlCount, sizeof(int), 1, pFile);

	UINT iIdxBuffSize = 0;
	for (UINT i = 0; i < iMtrlCount; ++i)
	{
		fwrite(&m_vecIdxInfo[i], sizeof(tIndexInfo), 1, pFile);
		fwrite(m_vecIdxInfo[i].pIdxSysMem
			, m_vecIdxInfo[i].iIdxCount * sizeof(UINT)
			, 1, pFile);
	}

	// Animation3D ���� 
	UINT iCount = (UINT)m_mapAnimClip.size();
	fwrite(&iCount, sizeof(int), 1, pFile);
	//for (UINT i = 0; i < iCount; ++i)
	//{
	//	SaveWString(m_mapAnimClip[i].strAnimName, pFile);
	//	fwrite(&m_mapAnimClip[i].dStartTime, sizeof(double), 1, pFile);
	//	fwrite(&m_mapAnimClip[i].dEndTime, sizeof(double), 1, pFile);
	//	fwrite(&m_mapAnimClip[i].dTimeLength, sizeof(double), 1, pFile);
	//	fwrite(&m_mapAnimClip[i].eMode, sizeof(int), 1, pFile);
	//	fwrite(&m_mapAnimClip[i].fUpdateTime, sizeof(float), 1, pFile);
	//	fwrite(&m_mapAnimClip[i].iStartFrame, sizeof(int), 1, pFile);
	//	fwrite(&m_mapAnimClip[i].iEndFrame, sizeof(int), 1, pFile);
	//	fwrite(&m_mapAnimClip[i].iFrameLength, sizeof(int), 1, pFile);
	//	fwrite(&m_mapAnimClip[i].iFrameCount, sizeof(int), 1, pFile);
	//}
	for (const auto& pair : m_mapAnimClip)
	{
		SaveWString(pair.second.strAnimName, pFile);
		fwrite(&pair.second.dStartTime, sizeof(double), 1, pFile);
		fwrite(&pair.second.dEndTime, sizeof(double), 1, pFile);
		fwrite(&pair.second.dTimeLength, sizeof(double), 1, pFile);
		fwrite(&pair.second.eMode, sizeof(int), 1, pFile);
		fwrite(&pair.second.fUpdateTime, sizeof(float), 1, pFile);
		fwrite(&pair.second.iStartFrame, sizeof(int), 1, pFile);
		fwrite(&pair.second.iEndFrame, sizeof(int), 1, pFile);
		fwrite(&pair.second.iFrameLength, sizeof(int), 1, pFile);
		fwrite(&pair.second.iFrameCount, sizeof(int), 1, pFile);
	}

	iCount = (UINT)m_vecBones.size();
	fwrite(&iCount, sizeof(int), 1, pFile);

	for (UINT i = 0; i < iCount; ++i)
	{
		SaveWString(m_vecBones[i].strBoneName, pFile);
		fwrite(&m_vecBones[i].iDepth, sizeof(int), 1, pFile);
		fwrite(&m_vecBones[i].iParentIndx, sizeof(int), 1, pFile);
		fwrite(&m_vecBones[i].matBone, sizeof(Matrix), 1, pFile);
		fwrite(&m_vecBones[i].matOffset, sizeof(Matrix), 1, pFile);

		size_t count = m_vecBones[i].vecKeyFrame.size();
		fwrite(&count, sizeof(size_t), 1, pFile);
		for (const auto& pair : m_vecBones[i].vecKeyFrame)
		{
			SaveWString(pair.first, pFile);
			size_t vecSize = pair.second.size();
			fwrite(&vecSize, sizeof(size_t), 1, pFile);
			for (size_t j = 0; j < vecSize; ++j)
			{
				fwrite(&pair.second[j], sizeof(tMTKeyFrame), 1, pFile);
			}
		}
	}

	fclose(pFile);


	return S_OK;
}
