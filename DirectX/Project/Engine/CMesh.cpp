#include "pch.h"
#include "CMesh.h"

#include "CDevice.h"
#include "CPathMgr.h"
#include "mFBXLoader.h"

#include "CStructuredBuffer.h"

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
}

CMesh* CMesh::CreateFromContainer(FBXLoader& _loader)
{
	const tContainer* container = &_loader.GetContainer(0);

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

	// 인덱스 정보
	UINT iIdxBufferCount = (UINT)container->vecIdx.size();
	D3D11_BUFFER_DESC tIdxDesc = {};

	for (UINT i = 0; i < iIdxBufferCount; ++i)
	{
		tIdxDesc.ByteWidth = (UINT)container->vecIdx[i].size() * sizeof(UINT); // Index Format 이 R32_UINT 이기 때문
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

		pMesh->m_vecAnimClip.push_back(tClip);
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

		for (UINT j = 0; j < vecAnimClip.size(); ++j)
		{
			wstring animName = vecAnimClip[j]->strName;

			if (vecBone[i]->vecKeyFrame.size() == 0) continue;

			vector<tKeyFrame> frame = vecBone[i]->vecKeyFrame.at(animName);
			vector<tMTKeyFrame> vecMTKeyFrames;

			for(UINT k = 0 ; k < frame.size(); ++k)
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
			

			pMesh->m_vecAnimClip[j].iFrameCount = (max(iFrameCount, (UINT)frame.size()));
		}
		pMesh->m_vecBones.push_back(bone);
	}

	// Animation 이 있는 Mesh 경우 structuredbuffer 만들어두기
	if (pMesh->IsAnimMesh())
	{
		for (size_t i = 0; i < pMesh->m_vecBones.size(); ++i)
		{
			tMTBone _bone = pMesh->m_vecBones[i];

			pMesh->m_vecBoneOffset.push_back(_bone.matOffset);

			for (size_t k = 0; k < pMesh->m_vecAnimClip.size(); ++k)
			{
				tMTAnimClip clip = pMesh->m_vecAnimClip[k];
				wstring animName = clip.strAnimName;

				clip.vecTransKeyFrame
					.resize((UINT)pMesh->m_vecBones.size() * clip.iFrameCount);

				if (_bone.vecKeyFrame.size() == 0) continue;

				vector<tMTKeyFrame> vecMTKeyFrame = _bone.vecKeyFrame.at(animName);
				
				for (size_t j = 0; j < vecMTKeyFrame.size(); ++j)
				{
					clip.vecTransKeyFrame[(UINT)pMesh->m_vecBones.size() * j + i] = {
						Vec4(vecMTKeyFrame[j].vTranslate, 0.f),
						Vec4(vecMTKeyFrame[j].vScale, 0.f),
						vecMTKeyFrame[j].qRot
					};
				}
				pMesh->m_vecAnimClip[k] = clip;
			}
		}
		

		pMesh->m_pBoneOffset = new CStructuredBuffer;
		pMesh->m_pBoneOffset->Create(sizeof(Matrix), (UINT)pMesh->m_vecBoneOffset.size(), SB_TYPE::READ_ONLY, false, pMesh->m_vecBoneOffset.data());

		pMesh->m_pBoneFrameData = new CStructuredBuffer;
		//pMesh->m_pBoneFrameData->Create(sizeof(tFrameTrans), (UINT)pMesh->m_vecBoneOffset.size()* pMesh->m_vecAnimClip[0].iFrameCount
		//	, SB_TYPE::READ_ONLY, false, pMesh->m_vecAnimClip[0].vecTransKeyFrame.data());
	}

	return pMesh;
}
CStructuredBuffer* CMesh::GetBoneFrameDataBuffer(int _Idx)
{
	tMTAnimClip clip = m_vecAnimClip[_Idx];
	m_pBoneFrameData->Create(sizeof(tFrameTrans), (UINT)m_vecBoneOffset.size() * clip.iFrameCount
		, SB_TYPE::READ_ONLY, false, clip.vecTransKeyFrame.data());
	return m_pBoneFrameData;
}
 

void CMesh::Create(void* _VtxSysMem, UINT _iVtxCount, void* _IdxSysMem, UINT _IdxCount)
{
	m_VtxCount = _iVtxCount;

	tIndexInfo indexInfo = {};
	indexInfo.iIdxCount = _IdxCount;

	// 정점 데이터를 저장할 버텍스 버퍼를 생성한다.
	m_tVBDesc.ByteWidth = sizeof(Vtx) * m_VtxCount;

	m_tVBDesc.CPUAccessFlags = 0;
	m_tVBDesc.Usage = D3D11_USAGE_DEFAULT;

	// Vertex 버퍼 생성
	m_tVBDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;
	m_tVBDesc.MiscFlags = 0;
	m_tVBDesc.StructureByteStride = 0;
	
	D3D11_SUBRESOURCE_DATA tSub = {};
	tSub.pSysMem = _VtxSysMem;
	if (FAILED(DEVICE->CreateBuffer(&m_tVBDesc, &tSub, m_VB.GetAddressOf())))
	{
		assert(nullptr);
	}

	// Index 버퍼 생성
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

	
	// SystemMem 데이터 복사
	m_pVtxSys = new Vtx[m_VtxCount];
	memcpy(m_pVtxSys, _VtxSysMem, sizeof(Vtx) * m_VtxCount);

	indexInfo.pIdxSysMem = new UINT[indexInfo.iIdxCount];
	memcpy(indexInfo.pIdxSysMem, _IdxSysMem, sizeof(UINT) * indexInfo.iIdxCount);

	m_vecIdxInfo.push_back(indexInfo);
}

int CMesh::Load(const wstring& _strFilePath)
{
	// 읽기모드로 파일열기
	//FILE* pFile = nullptr;
	//_wfopen_s(&pFile, _strFilePath.c_str(), L"rb");

	//// 키값, 상대경로
	//wstring strName, strKey, strRelativePath;
	//LoadWString(strName, pFile);
	//LoadWString(strKey, pFile);
	//LoadWString(strRelativePath, pFile);

	//SetName(strName);
	//SetKey(strKey);
	//SetRelativePath(strRelativePath);

	//// 정점데이터
	//UINT iByteSize = 0;
	//fread(&iByteSize, sizeof(int), 1, pFile);

	//m_pVtxSys = (Vtx*)malloc(iByteSize);
	//fread(m_pVtxSys, 1, iByteSize, pFile);


	//D3D11_BUFFER_DESC tDesc = {};
	//tDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	//tDesc.ByteWidth = iByteSize;
	//tDesc.Usage = D3D11_USAGE_DEFAULT;

	//D3D11_SUBRESOURCE_DATA tSubData = {};
	//tSubData.pSysMem = m_pVtxSys;

	//if (FAILED(DEVICE->CreateBuffer(&tDesc, &tSubData, m_VB.GetAddressOf())))
	//{
	//	assert(nullptr);
	//}

	//// 인덱스 정보
	//UINT iMtrlCount = 0;
	//fread(&iMtrlCount, sizeof(int), 1, pFile);

	//for (UINT i = 0; i < iMtrlCount; ++i)
	//{
	//	tIndexInfo info = {};
	//	fread(&info, sizeof(tIndexInfo), 1, pFile);

	//	UINT iByteWidth = info.iIdxCount * sizeof(UINT);

	//	void* pSysMem = malloc(iByteWidth);
	//	info.pIdxSysMem = pSysMem;
	//	fread(info.pIdxSysMem, iByteWidth, 1, pFile);

	//	tSubData.pSysMem = info.pIdxSysMem;

	//	if (FAILED(DEVICE->CreateBuffer(&info.tIBDesc, &tSubData, info.pIB.GetAddressOf())))
	//	{
	//		assert(nullptr);
	//	}

	//	m_vecIdxInfo.push_back(info);
	//}

	//// Animation3D 정보 읽기
	//int iCount = 0;
	//fread(&iCount, sizeof(int), 1, pFile);
	//for (int i = 0; i < iCount; ++i)
	//{
	//	tMTAnimClip tClip = {};

	//	LoadWString(tClip.strAnimName, pFile);
	//	fread(&tClip.dStartTime, sizeof(double), 1, pFile);
	//	fread(&tClip.dEndTime, sizeof(double), 1, pFile);
	//	fread(&tClip.dTimeLength, sizeof(double), 1, pFile);
	//	fread(&tClip.eMode, sizeof(int), 1, pFile);
	//	fread(&tClip.fUpdateTime, sizeof(float), 1, pFile);
	//	fread(&tClip.iStartFrame, sizeof(int), 1, pFile);
	//	fread(&tClip.iEndFrame, sizeof(int), 1, pFile);
	//	fread(&tClip.iFrameLength, sizeof(int), 1, pFile);

	//	m_vecAnimClip.push_back(tClip);
	//}

	//iCount = 0;
	//fread(&iCount, sizeof(int), 1, pFile);
	//m_vecBones.resize(iCount);

	//UINT _iFrameCount = 0;
	//for (int i = 0; i < iCount; ++i)
	//{
	//	LoadWString(m_vecBones[i].strBoneName, pFile);
	//	fread(&m_vecBones[i].iDepth, sizeof(int), 1, pFile);
	//	fread(&m_vecBones[i].iParentIndx, sizeof(int), 1, pFile);
	//	fread(&m_vecBones[i].matBone, sizeof(Matrix), 1, pFile);
	//	fread(&m_vecBones[i].matOffset, sizeof(Matrix), 1, pFile);

	//	//UINT iFrameCount = 0;
	//	//fread(&iFrameCount, sizeof(int), 1, pFile);

	//	//m_vecAryKeyFrames;
	//	//m_vecBones[i].vecKeyFrame.resize(iFrameCount);
	//	//_iFrameCount = max(_iFrameCount, iFrameCount);
	//	//for (UINT j = 0; j < iFrameCount; ++j)
	//	//{
	//	//	fread(&m_vecBones[i].vecKeyFrame[j], sizeof(tMTKeyFrame), 1, pFile);
	//	//}
	//}

	//// Animation 이 있는 Mesh 경우 Bone StructuredBuffer 만들기
	//if (m_vecAnimClip.size() > 0 && m_vecBones.size() > 0)
	//{
	//	wstring strBone = GetName();

	//	// BoneOffet 행렬
	//	vector<Matrix> vecOffset;
	//	vector<tFrameTrans> vecFrameTrans;
	//	vecFrameTrans.resize((UINT)m_vecBones.size() * _iFrameCount);

	//	for (size_t i = 0; i < m_vecBones.size(); ++i)
	//	{
	//		vecOffset.push_back(m_vecBones[i].matOffset);

	//		//for (size_t j = 0; j < m_vecBones[i].vecKeyFrame.size(); ++j)
	//		//{
	//		//	vecFrameTrans[(UINT)m_vecBones.size() * j + i]
	//		//		= tFrameTrans{ Vec4(m_vecBones[i].vecKeyFrame[j].vTranslate, 0.f)
	//		//		, Vec4(m_vecBones[i].vecKeyFrame[j].vScale, 0.f)
	//		//		, Vec4(m_vecBones[i].vecKeyFrame[j].qRot) };
	//		//}
	//	}

	//	m_pBoneOffset = new CStructuredBuffer;
	//	m_pBoneOffset->Create(sizeof(Matrix), (UINT)vecOffset.size(), SB_TYPE::READ_ONLY, false, vecOffset.data());

	//	m_pBoneFrameData = new CStructuredBuffer;
	//	m_pBoneFrameData->Create(sizeof(tFrameTrans), (UINT)vecOffset.size() * (UINT)_iFrameCount
	//		, SB_TYPE::READ_ONLY, false, vecFrameTrans.data());
	//}

	//fclose(pFile);

	return S_OK;
}

int CMesh::Save(const wstring& _strRelativePath)
{
	// 상대경로 저장
	//SetRelativePath(_strRelativePath);

	//// 파일 경로 만들기
	//wstring strFilePath = CPathMgr::GetInst()->GetContentPath() + _strRelativePath;

	//// 파일 쓰기모드로 열기
	//FILE* pFile = nullptr;
	//errno_t err = _wfopen_s(&pFile, strFilePath.c_str(), L"wb");
	//assert(pFile);

	//// 키값, 상대 경로	
	//SaveWString(GetName(), pFile);
	//SaveWString(GetKey(), pFile);
	//SaveWString(GetRelativePath(), pFile);

	//// 정점 데이터 저장				
	//int iByteSize = m_tVBDesc.ByteWidth;
	//fwrite(&iByteSize, sizeof(int), 1, pFile);
	//fwrite(m_pVtxSys, iByteSize, 1, pFile);

	//// 인덱스 정보
	//UINT iMtrlCount = (UINT)m_vecIdxInfo.size();
	//fwrite(&iMtrlCount, sizeof(int), 1, pFile);

	//UINT iIdxBuffSize = 0;
	//for (UINT i = 0; i < iMtrlCount; ++i)
	//{
	//	fwrite(&m_vecIdxInfo[i], sizeof(tIndexInfo), 1, pFile);
	//	fwrite(m_vecIdxInfo[i].pIdxSysMem
	//		, m_vecIdxInfo[i].iIdxCount * sizeof(UINT)
	//		, 1, pFile);
	//}

	//// Animation3D 정보 
	//UINT iCount = (UINT)m_vecAnimClip.size();
	//fwrite(&iCount, sizeof(int), 1, pFile);
	//for (UINT i = 0; i < iCount; ++i)
	//{
	//	SaveWString(m_vecAnimClip[i].strAnimName, pFile);
	//	fwrite(&m_vecAnimClip[i].dStartTime, sizeof(double), 1, pFile);
	//	fwrite(&m_vecAnimClip[i].dEndTime, sizeof(double), 1, pFile);
	//	fwrite(&m_vecAnimClip[i].dTimeLength, sizeof(double), 1, pFile);
	//	fwrite(&m_vecAnimClip[i].eMode, sizeof(int), 1, pFile);
	//	fwrite(&m_vecAnimClip[i].fUpdateTime, sizeof(float), 1, pFile);
	//	fwrite(&m_vecAnimClip[i].iStartFrame, sizeof(int), 1, pFile);
	//	fwrite(&m_vecAnimClip[i].iEndFrame, sizeof(int), 1, pFile);
	//	fwrite(&m_vecAnimClip[i].iFrameLength, sizeof(int), 1, pFile);
	//}

	//iCount = (UINT)m_vecBones.size();
	//fwrite(&iCount, sizeof(int), 1, pFile);

	//for (UINT i = 0; i < iCount; ++i)
	//{
	//	SaveWString(m_vecBones[i].strBoneName, pFile);
	//	fwrite(&m_vecBones[i].iDepth, sizeof(int), 1, pFile);
	//	fwrite(&m_vecBones[i].iParentIndx, sizeof(int), 1, pFile);
	//	fwrite(&m_vecBones[i].matBone, sizeof(Matrix), 1, pFile);
	//	fwrite(&m_vecBones[i].matOffset, sizeof(Matrix), 1, pFile);

	//	//int iFrameCount = (int)m_vecBones[i].vecKeyFrame.size();
	//	//fwrite(&iFrameCount, sizeof(int), 1, pFile);

	//	//for (int j = 0; j < m_vecBones[i].vecKeyFrame.size(); ++j)
	//	//{
	//	//	fwrite(&m_vecBones[i].vecKeyFrame[j], sizeof(tMTKeyFrame), 1, pFile);
	//	//}
	//}

	//fclose(pFile);


	return S_OK;
}

void CMesh::UpdateData(UINT _iSubset)
{
	UINT iStride = sizeof(Vtx);
	UINT iOffset = 0;

	CONTEXT->IASetVertexBuffers(0, 1, m_VB.GetAddressOf(), &iStride, &iOffset);
	CONTEXT->IASetIndexBuffer(m_vecIdxInfo[_iSubset].pIB.Get(), DXGI_FORMAT_R32_UINT, 0);
}

void CMesh::render(UINT _iSubset)
{
	UpdateData(_iSubset);

	CONTEXT->DrawIndexed(m_vecIdxInfo[_iSubset].iIdxCount, 0, 0);
}

void CMesh::render_particle(UINT _iParticleCount)
{
	UpdateData(0);

	// 인스턴싱
	CONTEXT->DrawIndexedInstanced(m_vecIdxInfo[0].iIdxCount, _iParticleCount, 0, 0, 0);
}
