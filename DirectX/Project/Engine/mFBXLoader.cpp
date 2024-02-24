#include "pch.h"
#include "mFBXLoader.h"
#include "CMesh.h"

#include "CResMgr.h"
#include "CPathMgr.h"

#include "CMaterial.h"
#include "CMesh.h"


#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>
using std::experimental::filesystem::path;

FBXLoader::FBXLoader()
	: m_pManager(NULL)
	, m_pScene(NULL)
	, m_pImporter(NULL)
{
}

FBXLoader::~FBXLoader()
{
	m_pScene->Destroy();
	m_pManager->Destroy();

	for (size_t i = 0; i < m_vecBone.size(); ++i)
	{
		if (nullptr != m_vecBone[i])
			delete m_vecBone[i];
	}

	for (size_t i = 0; i < m_vecAnimClip.size(); ++i)
	{
		if (nullptr != m_vecAnimClip[i])
			delete m_vecAnimClip[i];
	}

	for (int i = 0; i < m_arrAnimName.Size(); ++i)
	{
		if (nullptr != m_arrAnimName[i])
			delete m_arrAnimName[i];
	}
}

void FBXLoader::init()
{
	m_pManager = FbxManager::Create();
	if (NULL == m_pManager)
		assert(NULL);

	FbxIOSettings* pIOSettings = FbxIOSettings::Create(m_pManager, IOSROOT);
	m_pManager->SetIOSettings(pIOSettings);

	m_pScene = FbxScene::Create(m_pManager, "");

	//m_pScene->GetGlobalSettings().SetAxisSystem(FbxAxisSystem(FbxAxisSystem::eDirectX));

	if (NULL == m_pScene)
		assert(NULL);
}

void FBXLoader::LoadFbx(const wstring& _strPath)
{
	m_vecContainer.clear();

	m_pImporter = FbxImporter::Create(m_pManager, "");

	//wstring str = wstring_convert<codecvt_utf8<wchar_t>>().from_bytes(strName.c_str());
	string strPath(_strPath.begin(), _strPath.end());

	if (!m_pImporter->Initialize(strPath.c_str(), -1, m_pManager->GetIOSettings()))
		assert(nullptr);

	m_pImporter->Import(m_pScene);

	/*FbxAxisSystem originAxis = FbxAxisSystem::eMax;
	originAxis = m_pScene->GetGlobalSettings().GetAxisSystem();
	FbxAxisSystem DesireAxis = FbxAxisSystem::DirectX;
	DesireAxis.ConvertScene(m_pScene);
	originAxis = m_pScene->GetGlobalSettings().GetAxisSystem();*/
	//m_pMesh = m_pScene->GetSrcObject<FbxMesh>(0);

	//FbxTime time;
	//m_pMesh->GetNode()->EvaluateGlobalTransform();

	m_pScene->GetGlobalSettings().SetAxisSystem(FbxAxisSystem::Max);

	LoadSkeleton(m_pScene->GetRootNode());

	m_pScene->FillAnimStackNameArray(m_arrAnimName);

	LoadAnimationClip();

	Triangulate(m_pScene->GetRootNode());

	LoadMeshDataFromNode(m_pScene->GetRootNode());

	m_pImporter->Destroy();

	LoadTexture();

	CreateMaterial();
}

void FBXLoader::LoadMeshDataFromNode(FbxNode* _pNode)
{
	FbxNodeAttribute* pAttr = _pNode->GetNodeAttribute();


	if (pAttr && FbxNodeAttribute::eMesh == pAttr->GetAttributeType())
	{
		FbxAMatrix matGlobal = _pNode->EvaluateGlobalTransform();
		matGlobal.GetR();

		FbxMesh* pMesh = _pNode->GetMesh();
		if (NULL != pMesh)
			LoadMesh(pMesh);
	}

	UINT iMtrlCnt = _pNode->GetMaterialCount();
	if (iMtrlCnt > 0)
	{
		for (UINT i = 0; i < iMtrlCnt; ++i)
		{
			FbxSurfaceMaterial* pMtrlSur = _pNode->GetMaterial(i);
			LoadMaterial(pMtrlSur);
		}
	}

	// 자식 노드 정보 읽기
	int iChildCnt = _pNode->GetChildCount();
	for (int i = 0; i < iChildCnt; ++i)
	{
		LoadMeshDataFromNode(_pNode->GetChild(i));
	}
}

void FBXLoader::LoadMesh(FbxMesh* _pFbxMesh)
{
	m_pMesh = _pFbxMesh;

	m_vecContainer.push_back(tContainer{});
	tContainer& Container = m_vecContainer[m_vecContainer.size() - 1];

	string strName = _pFbxMesh->GetName();
	Container.strName = wstring(strName.begin(), strName.end());

	int iVtxCnt = _pFbxMesh->GetControlPointsCount();
	Container.Resize(iVtxCnt);

	FbxVector4* pFbxPos = _pFbxMesh->GetControlPoints();

	for (int i = 0; i < iVtxCnt; ++i)
	{
		Container.vecPos[i].x = (float)pFbxPos[i].mData[0];
		Container.vecPos[i].y = (float)pFbxPos[i].mData[2];
		Container.vecPos[i].z = (float)pFbxPos[i].mData[1];
	}

	int iPolyCnt = _pFbxMesh->GetPolygonCount();
	//_pFbxMesh->GetUV
	int iMtrlCnt = _pFbxMesh->GetNode()->GetMaterialCount();
	Container.vecIdx.resize(iMtrlCnt);

	FbxGeometryElementMaterial* pMtrl = _pFbxMesh->GetElementMaterial();

	UINT iVtxOrder = 0;
	UINT arrIdx[3] = {};

	//FbxStringList* list = new FbxStringList;
	//_pFbxMesh->GetUVSetNames(*list);

	//delete list;

	for (int i = 0; i < iPolyCnt; ++i)
	{
		int iPolySize = _pFbxMesh->GetPolygonSize(i);

		if (3 != iPolySize)
			assert(NULL);

		for (int j = 0; j < iPolySize; ++j)
		{
			int iIdx = _pFbxMesh->GetPolygonVertex(i, j);
			
			arrIdx[j] = iIdx;

			GetUV(_pFbxMesh, &Container, iIdx, _pFbxMesh->GetTextureUVIndex(i, j));

			GetNormal(_pFbxMesh, &Container, iIdx, iVtxOrder);
			GetBinormal(_pFbxMesh, &Container, iIdx, iVtxOrder);
			GetTangent(_pFbxMesh, &Container, iIdx, iVtxOrder);

			++iVtxOrder;
		}
		UINT iSubsetIdx = pMtrl->GetIndexArray().GetAt(i);
		Container.vecIdx[iSubsetIdx].push_back(arrIdx[0]);
		Container.vecIdx[iSubsetIdx].push_back(arrIdx[2]);
		Container.vecIdx[iSubsetIdx].push_back(arrIdx[1]);
	}
	
	LoadAnimationData(_pFbxMesh, &Container);
}

void FBXLoader::LoadMaterial(FbxSurfaceMaterial* _pMtrlSur)
{
	tFbxMaterial tMtrlInfo{};

	string str = _pMtrlSur->GetName();
	tMtrlInfo.strMtrlName = wstring(str.begin(), str.end());

	// Diff
	tMtrlInfo.tMtrl.vDiff = GetMtrlData(_pMtrlSur
		, FbxSurfaceMaterial::sDiffuse
		, FbxSurfaceMaterial::sDiffuseFactor);

	// Amb
	tMtrlInfo.tMtrl.vAmb = GetMtrlData(_pMtrlSur
		, FbxSurfaceMaterial::sAmbient
		, FbxSurfaceMaterial::sAmbientFactor);

	// Spec
	tMtrlInfo.tMtrl.vSpec = GetMtrlData(_pMtrlSur
		, FbxSurfaceMaterial::sSpecular
		, FbxSurfaceMaterial::sSpecularFactor);

	// Emisv
	tMtrlInfo.tMtrl.vEmv = GetMtrlData(_pMtrlSur
		, FbxSurfaceMaterial::sEmissive
		, FbxSurfaceMaterial::sEmissiveFactor);

	// Texture Name
	tMtrlInfo.strDiff = GetMtrlTextureName(_pMtrlSur, FbxSurfaceMaterial::sDiffuse);
	tMtrlInfo.strNormal = GetMtrlTextureName(_pMtrlSur, FbxSurfaceMaterial::sNormalMap);
	tMtrlInfo.strSpec = GetMtrlTextureName(_pMtrlSur, FbxSurfaceMaterial::sSpecular);
	tMtrlInfo.strEmis = GetMtrlTextureName(_pMtrlSur, FbxSurfaceMaterial::sEmissive);


	m_vecContainer.back().vecMtrl.push_back(tMtrlInfo);
}

void FBXLoader::GetTangent(FbxMesh* _pMesh
	, tContainer* _pContainer
	, int _iIdx
	, int _iVtxOrder)
{
	int iTangentCnt = _pMesh->GetElementTangentCount();
	if (1 < iTangentCnt)
	{
		assert(NULL);
	}


	if (1 == iTangentCnt)
	{
		// 탄젠트 data 의 시작 주소
		FbxGeometryElementTangent* pTangent = _pMesh->GetElementTangent();
		UINT iTangentIdx = 0;

		if (pTangent->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
		{
			if (pTangent->GetReferenceMode() == FbxGeometryElement::eDirect)
				iTangentIdx = _iVtxOrder;
			else
				iTangentIdx = pTangent->GetIndexArray().GetAt(_iVtxOrder);
		}
		else if (pTangent->GetMappingMode() == FbxGeometryElement::eByControlPoint)
		{
			if (pTangent->GetReferenceMode() == FbxGeometryElement::eDirect)
				iTangentIdx = _iIdx;
			else
				iTangentIdx = pTangent->GetIndexArray().GetAt(_iIdx);
		}

		FbxVector4 vTangent = pTangent->GetDirectArray().GetAt(iTangentIdx);

		_pContainer->vecTangent[_iIdx].x = (float)vTangent.mData[0];
		_pContainer->vecTangent[_iIdx].y = (float)vTangent.mData[2];
		_pContainer->vecTangent[_iIdx].z = (float)vTangent.mData[1];
	}
	if (0 == iTangentCnt)
	{
		_pMesh->InitTangents();
		_pMesh->GenerateTangentsData(0, true);

		FbxGeometryElementTangent* pTangent = _pMesh->GetElementTangent();
		UINT iTangentIdx = 0;
		if (pTangent->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
		{
			if (pTangent->GetReferenceMode() == FbxGeometryElement::eDirect)
				iTangentIdx = _iVtxOrder;
			else
				iTangentIdx = pTangent->GetIndexArray().GetAt(_iVtxOrder);
		}
		else if (pTangent->GetMappingMode() == FbxGeometryElement::eByControlPoint)
		{
			if (pTangent->GetReferenceMode() == FbxGeometryElement::eDirect)
				iTangentIdx = _iIdx;
			else
				iTangentIdx = pTangent->GetIndexArray().GetAt(_iIdx);
		}

		FbxVector4 vTangent = pTangent->GetDirectArray().GetAt(iTangentIdx);

		_pContainer->vecTangent[_iIdx].x = (float)vTangent.mData[0];
		_pContainer->vecTangent[_iIdx].y = (float)vTangent.mData[2];
		_pContainer->vecTangent[_iIdx].z = (float)vTangent.mData[1];
	}
	//else
	//{
	//	_pContainer->vecTangent[_iIdx].x = 0.f;
	//	_pContainer->vecTangent[_iIdx].y = 0.f;
	//	_pContainer->vecTangent[_iIdx].z = 0.f;
	//}
}

void FBXLoader::GetBinormal(FbxMesh* _pMesh, tContainer* _pContainer, int _iIdx, int _iVtxOrder)
{
	int iBinormalCnt = _pMesh->GetElementBinormalCount();
	if (1 < iBinormalCnt)
	{
		assert(NULL);
		//_pMesh->InitBinormals();
		//_pMesh->CreateElementBinormal();

		//FbxGeometryElementBinormal* pBinormal = _pMesh->GetElementBinormal();
		//UINT iBinormalIdx = 0;

		//if (pBinormal->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
		//{
		//	if (pBinormal->GetReferenceMode() == FbxGeometryElement::eDirect)
		//		iBinormalIdx = _iVtxOrder;
		//	else
		//		iBinormalIdx = pBinormal->GetIndexArray().GetAt(_iVtxOrder);
		//}
		//else if (pBinormal->GetMappingMode() == FbxGeometryElement::eByControlPoint)
		//{
		//	if (pBinormal->GetReferenceMode() == FbxGeometryElement::eDirect)
		//		iBinormalIdx = _iIdx;
		//	else
		//		iBinormalIdx = pBinormal->GetIndexArray().GetAt(_iIdx);
		//}

		//FbxVector4 vBinormal = pBinormal->GetDirectArray().GetAt(iBinormalIdx);

		//_pContainer->vecBinormal[_iIdx].x = (float)vBinormal.mData[0];
		//_pContainer->vecBinormal[_iIdx].y = (float)vBinormal.mData[2];
		//_pContainer->vecBinormal[_iIdx].z = (float)vBinormal.mData[1];

		//

		//_pMesh->InitNormals();
		//_pMesh->GenerateNormals();
	}

	if (1 == iBinormalCnt)
	{
		// 종법선 data 의 시작 주소
		FbxGeometryElementBinormal* pBinormal = _pMesh->GetElementBinormal();
		UINT iBinormalIdx = 0;

		if (pBinormal->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
		{
			if (pBinormal->GetReferenceMode() == FbxGeometryElement::eDirect)
				iBinormalIdx = _iVtxOrder;
			else
				iBinormalIdx = pBinormal->GetIndexArray().GetAt(_iVtxOrder);
		}
		else if (pBinormal->GetMappingMode() == FbxGeometryElement::eByControlPoint)
		{
			if (pBinormal->GetReferenceMode() == FbxGeometryElement::eDirect)
				iBinormalIdx = _iIdx;
			else
				iBinormalIdx = pBinormal->GetIndexArray().GetAt(_iIdx);
		}

		FbxVector4 vBinormal = pBinormal->GetDirectArray().GetAt(iBinormalIdx);

		_pContainer->vecBinormal[_iIdx].x = (float)vBinormal.mData[0];
		_pContainer->vecBinormal[_iIdx].y = (float)vBinormal.mData[2];
		_pContainer->vecBinormal[_iIdx].z = (float)vBinormal.mData[1];
	}
	//else
	//{
	//	_pContainer->vecBinormal[_iIdx].x = 0.f;
	//	_pContainer->vecBinormal[_iIdx].y = 0.f;
	//	_pContainer->vecBinormal[_iIdx].z = 0.f;
	//}
}

void FBXLoader::GetNormal(FbxMesh* _pMesh, tContainer* _pContainer, int _iIdx, int _iVtxOrder)
{
	int iNormalCnt = _pMesh->GetElementNormalCount();
	if (1 != iNormalCnt)
	{
		assert(NULL);
	}


	// 종법선 data 의 시작 주소
	FbxGeometryElementNormal* pNormal = _pMesh->GetElementNormal();
	UINT iNormalIdx = 0;

	if (pNormal->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
	{
		if (pNormal->GetReferenceMode() == FbxGeometryElement::eDirect)
			iNormalIdx = _iVtxOrder;
		else
			iNormalIdx = pNormal->GetIndexArray().GetAt(_iVtxOrder);
	}
	else if (pNormal->GetMappingMode() == FbxGeometryElement::eByControlPoint)
	{
		if (pNormal->GetReferenceMode() == FbxGeometryElement::eDirect)
			iNormalIdx = _iIdx;
		else
			iNormalIdx = pNormal->GetIndexArray().GetAt(_iIdx);
	}

	FbxVector4 vNormal = pNormal->GetDirectArray().GetAt(iNormalIdx);

	_pContainer->vecNormal[_iIdx].x = (float)vNormal.mData[0];
	_pContainer->vecNormal[_iIdx].y = (float)vNormal.mData[2];
	_pContainer->vecNormal[_iIdx].z = (float)vNormal.mData[1];
}

void FBXLoader::GetUV(FbxMesh* _pMesh, tContainer* _pContainer, int _iIdx, int _iUVIndex)
{
	FbxGeometryElementUV* pUV = _pMesh->GetElementUV();
	//_pMesh->GetControlPointAt(_iIdx).mData;

	UINT iUVIdx = _iUVIndex;
	if (pUV->GetMappingMode() == FbxGeometryElement::eByControlPoint)
	{
		switch (pUV->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
			iUVIdx = _iIdx;
			break;
		case FbxGeometryElement::eIndexToDirect:
			iUVIdx = pUV->GetIndexArray().GetAt(_iIdx);
			break;
		}
	}else if(pUV->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
	{
		iUVIdx = _iUVIndex;
	}

	FbxVector2 vUV = pUV->GetDirectArray().GetAt(iUVIdx);
	
	FbxLayerElement::EMappingMode mapping = pUV->GetMappingMode();
	
	_pContainer->vecUV[_iIdx].x = (float)vUV.mData[0];
	_pContainer->vecUV[_iIdx].y = 1.f - (float)vUV.mData[1];
}

Vec4 FBXLoader::GetMtrlData(FbxSurfaceMaterial* _pSurface
	, const char* _pMtrlName
	, const char* _pMtrlFactorName)
{
	FbxDouble3  vMtrl;
	FbxDouble	dFactor = 0.;

	FbxProperty tMtrlProperty = _pSurface->FindProperty(_pMtrlName);
	FbxProperty tMtrlFactorProperty = _pSurface->FindProperty(_pMtrlFactorName);

	if (tMtrlProperty.IsValid() && tMtrlFactorProperty.IsValid())
	{
		vMtrl = tMtrlProperty.Get<FbxDouble3>();
		dFactor = tMtrlFactorProperty.Get<FbxDouble>();
	}

	Vec4 vRetVal = Vec4((float)vMtrl.mData[0] * (float)dFactor, (float)vMtrl.mData[1] * (float)dFactor, (float)vMtrl.mData[2] * (float)dFactor, (float)dFactor);
	return vRetVal;
}

wstring FBXLoader::GetMtrlTextureName(FbxSurfaceMaterial* _pSurface, const char* _pMtrlProperty)
{
	string strName;

	FbxProperty TextureProperty = _pSurface->FindProperty(_pMtrlProperty);
	if (TextureProperty.IsValid())
	{
		UINT iCnt = TextureProperty.GetSrcObjectCount();

		if (1 <= iCnt)
		{
			FbxFileTexture* pFbxTex = TextureProperty.GetSrcObject<FbxFileTexture>(0);
			if (NULL != pFbxTex)
				strName = pFbxTex->GetFileName();
		}
	}

	return wstring(strName.begin(), strName.end());
}

FbxVector4 FBXLoader::GetBoneWorldTransform(int idx)
{
	FbxNode* pNode = m_pScene->GetRootNode();
	for (int i = 0; i < pNode->GetChildCount(); ++i)
	{
		FbxNode* child = pNode->GetChild(i);
		string str = child->GetName();
		wstring wstr(str.begin(), str.end());
		wstr += L"\n";
		OutputDebugStringW(wstr.c_str());
	}
	pNode->GetChild(idx);
	FbxVector4 vT = pNode->GetGeometricTranslation(FbxNode::eSourcePivot);
	FbxVector4 vR = pNode->GetGeometricRotation(FbxNode::eSourcePivot);
	FbxVector4 vS = pNode->GetGeometricScaling(FbxNode::eSourcePivot);

	return vS;
}

void FBXLoader::LoadTexture()
{
	path path_content = CPathMgr::GetInst()->GetContentPath();

	path path_fbx_texture = path_content.wstring() + L"texture\\FBXTexture\\";

	if (false == exists(path_fbx_texture))
	{
		create_directory(path_fbx_texture);
	}

	path path_origin;
	path path_filename;
	path path_dest;

	for (UINT i = 0; i < m_vecContainer.size(); ++i)
	{
		for (UINT j = 0; j < m_vecContainer[i].vecMtrl.size(); ++j)
		{
			vector<path> vecPath;
			vecPath.push_back(m_vecContainer[i].vecMtrl[j].strDiff.c_str());
			vecPath.push_back(m_vecContainer[i].vecMtrl[j].strNormal.c_str());
			vecPath.push_back(m_vecContainer[i].vecMtrl[j].strSpec.c_str());
			vecPath.push_back(m_vecContainer[i].vecMtrl[j].strEmis.c_str());

			for (size_t k = 0; k < vecPath.size(); ++k)
			{
				if (vecPath[k].filename().empty())
					continue;

				path_origin = vecPath[k];
				path_filename = vecPath[k].filename();
				path_dest = path_fbx_texture.wstring() + path_filename.wstring();

				if (false == exists(path_dest))
				{
					copy(path_origin, path_dest);
				}

				path_dest = GetRelativePath(CPathMgr::GetInst()->GetContentPath(), path_dest);
				CResMgr::GetInst()->Load<CTexture>(path_dest, path_dest);

				switch (k)
				{
				case 0: m_vecContainer[i].vecMtrl[j].strDiff = path_dest; break;
				case 1: m_vecContainer[i].vecMtrl[j].strNormal = path_dest; break;
				case 2: m_vecContainer[i].vecMtrl[j].strSpec = path_dest; break;
				case 3: m_vecContainer[i].vecMtrl[j].strEmis = path_dest; break;
				}
			}
		}
		//path_origin = path_origin.parent_path();
		//remove_all(path_origin);
	}
}

void FBXLoader::CreateMaterial()
{
	wstring strMtrlName;
	wstring strPath;

	for (UINT i = 0; i < m_vecContainer.size(); ++i)
	{
		for (UINT j = 0; j < m_vecContainer[i].vecMtrl.size(); ++j)
		{
			// Material 이름짓기
			strMtrlName = m_vecContainer[i].vecMtrl[j].strMtrlName;
			if (strMtrlName.empty())
				strMtrlName = path(m_vecContainer[i].vecMtrl[j].strDiff).stem();

			strPath = L"material\\";
			strPath += strMtrlName + L".mtrl";

			// 재질 이름
			m_vecContainer[i].vecMtrl[j].strMtrlName = strPath;

			// 이미 로딩된 재질이면 로딩된 것을 사용
			Ptr<CMaterial> pMaterial = CResMgr::GetInst()->FindRes<CMaterial>(strPath);
			if (nullptr != pMaterial)
				continue;

			pMaterial = new CMaterial;

			// 상대경로가 곧 키
			pMaterial->SetKey(strPath);
			pMaterial->SetRelativePath(strPath);

			//if(j == 3) // 4번째 Material 투명으로 사용
			//	pMaterial->SetShader(CResMgr::GetInst()->FindRes<CGraphicsShader>(L"Std3DShader"));
			//else


			pMaterial->SetShader(CResMgr::GetInst()->FindRes<CGraphicsShader>(L"Std3D_DeferredShader"));
			wstring strTexKey = m_vecContainer[i].vecMtrl[j].strDiff;
			Ptr<CTexture> pTex = CResMgr::GetInst()->FindRes<CTexture>(strTexKey);



			if (NULL != pTex)
			{
				//pTex->GenerateMip(8);
				pMaterial->SetTexParam(TEX_PARAM::TEX_0, pTex);
			}

			strTexKey = m_vecContainer[i].vecMtrl[j].strNormal;
			pTex = CResMgr::GetInst()->FindRes<CTexture>(strTexKey);
			if (NULL != pTex)
			{
				//pTex->GenerateMip(8);
				pMaterial->SetTexParam(TEX_PARAM::TEX_1, pTex);
			}

			strTexKey = m_vecContainer[i].vecMtrl[j].strSpec;
			pTex = CResMgr::GetInst()->FindRes<CTexture>(strTexKey);
			if (NULL != pTex)
			{
				//pTex->GenerateMip(8);
				pMaterial->SetTexParam(TEX_PARAM::TEX_2, pTex);
			}

			strTexKey = m_vecContainer[i].vecMtrl[j].strEmis;
			pTex = CResMgr::GetInst()->FindRes<CTexture>(strTexKey);
			if (NULL != pTex)
			{
				//pTex->GenerateMip(8);
				pMaterial->SetTexParam(TEX_PARAM::TEX_3, pTex);
			}


			pMaterial->SetMaterialCoefficient(
				m_vecContainer[i].vecMtrl[j].tMtrl.vDiff
				, m_vecContainer[i].vecMtrl[j].tMtrl.vSpec
				, m_vecContainer[i].vecMtrl[j].tMtrl.vAmb
				, m_vecContainer[i].vecMtrl[j].tMtrl.vEmv);

			CResMgr::GetInst()->AddRes<CMaterial>(pMaterial->GetKey(), pMaterial.Get());
			pMaterial->Save(strPath);
		}
	}
}

void FBXLoader::LoadSkeleton(FbxNode* _pNode)
{
	//int iChildCount = _pNode->GetChildCount();
	//for (int i = 0; i < iChildCount; ++i)
	//{
	//	LoadSkeleton_Re(_pNode->GetChild(i), 0, 0, -1);
	//}
	LoadSkeleton_Re(_pNode, 0, 0, -1);
}

void FBXLoader::LoadSkeleton_Re(FbxNode* _pNode, int _iDepth, int _iIdx, int _iParentIdx)
{
	FbxNodeAttribute* pAttr = _pNode->GetNodeAttribute();
	FbxTime::EMode eTimeMode = m_pScene->GetGlobalSettings().GetTimeMode();

	if (pAttr && pAttr->GetAttributeType() == FbxNodeAttribute::eSkeleton)
	{
		tBone* pBone = new tBone;

		FbxTime   tTime = 0;
		tTime.SetFrame(0, eTimeMode);

		string strBoneName = _pNode->GetName();
		pBone->strBoneName = wstring(strBoneName.begin(), strBoneName.end());
		pBone->iDepth = _iDepth++;
		pBone->iParentIndx = _iParentIdx;
		pBone->vBonePos = _pNode->EvaluateGlobalTransform(tTime).GetT();
		//pBone->vBonePos = _pNode->EvaluateLocalTransform().GetT();
		//pBone->vBonePos = _pNode->LclTranslation.Get();

		m_vecBone.push_back(pBone);
	}

	int iChildCount = _pNode->GetChildCount();
	for (int i = 0; i < iChildCount; ++i)
	{
		LoadSkeleton_Re(_pNode->GetChild(i), _iDepth, (int)m_vecBone.size(), _iIdx);
	}
}

void FBXLoader::LoadAnimationClip()
{
	int iAnimCount = m_arrAnimName.GetCount();

	for (int i = 0; i < iAnimCount; ++i)
	{
		FbxAnimStack* pAnimStack = m_pScene->FindMember<FbxAnimStack>(m_arrAnimName[i]->Buffer());

		if (!pAnimStack)
			continue;

		tAnimClip* pAnimClip = new tAnimClip;

		string strClipName = pAnimStack->GetName();
		pAnimClip->strName = wstring(strClipName.begin(), strClipName.end());

		FbxTakeInfo* pTakeInfo = m_pScene->GetTakeInfo(pAnimStack->GetName());
		pAnimClip->tStartTime = pTakeInfo->mLocalTimeSpan.GetStart();
		pAnimClip->tEndTime = pTakeInfo->mLocalTimeSpan.GetStop();

		pAnimClip->eMode = m_pScene->GetGlobalSettings().GetTimeMode();
		pAnimClip->llTimeLength = pAnimClip->tEndTime.GetFrameCount(pAnimClip->eMode) - pAnimClip->tStartTime.GetFrameCount(pAnimClip->eMode);

		m_vecAnimClip.push_back(pAnimClip);
	}
}

void FBXLoader::Triangulate(FbxNode* _pNode)
{
	FbxNodeAttribute* pAttr = _pNode->GetNodeAttribute();

	if (pAttr &&
		(pAttr->GetAttributeType() == FbxNodeAttribute::eMesh
			|| pAttr->GetAttributeType() == FbxNodeAttribute::eNurbs
			|| pAttr->GetAttributeType() == FbxNodeAttribute::eNurbsSurface))
	{
		FbxGeometryConverter converter(m_pManager);
		converter.Triangulate(pAttr, true);
	}

	//int iChildCount = _pNode->GetChildCount();

	//for (int i = 0; i < iChildCount; ++i)
	//{
	//	Triangulate(_pNode->GetChild(i));
	//}
}

void FBXLoader::LoadAnimationData(FbxMesh* _pMesh, tContainer* _pContainer)
{
	int iSkinCount = _pMesh->GetDeformerCount(FbxDeformer::eSkin);
	if (iSkinCount <= 0 || m_vecAnimClip.empty())
		return;

	//m_vecAryKeyFrames.resize(m_mapAnimClip.size());
	//for(int i = 0 ; i < m_mapAnimClip.size(); ++i)
	//{
	//	m_vecAryKeyFrames[i].resize(m_vecBone.size());
	//}

	_pContainer->bAnimation = true;

	for (int i = 0; i < iSkinCount; ++i)
	{
		FbxSkin* pSkin = (FbxSkin*)_pMesh->GetDeformer(i, FbxDeformer::eSkin);

		if (pSkin)
		{
			FbxSkin::EType eType = pSkin->GetSkinningType();
			if (FbxSkin::eRigid == eType || FbxSkin::eLinear /*|| FbxSkin::eBlend*/)
			{
				int iClusterCount = pSkin->GetClusterCount();

				for (int j = 0; j < iClusterCount; ++j)
				{
					FbxCluster* pCluster = pSkin->GetCluster(j);

					if (!pCluster->GetLink())
						continue;

					int iBoneIdx = FindBoneIndex(pCluster->GetLink()->GetName());
					if (-1 == iBoneIdx)
						assert(NULL);

					FbxAMatrix matNodeTransform = GetTransform(_pMesh->GetNode());

					LoadWeightsAndIndices(pCluster, iBoneIdx, _pContainer);

					LoadOffsetMatrix(pCluster, matNodeTransform, iBoneIdx, _pContainer);

					LoadKeyframeTransform(_pMesh->GetNode(), pCluster, matNodeTransform, iBoneIdx, _pContainer);
				}
			}
		}
	}
	CheckWeightAndIndices(_pMesh, _pContainer);
}


void FBXLoader::CheckWeightAndIndices(FbxMesh* _pMesh, tContainer* _pContainer)
{
	vector<vector<tWeightsAndIndices>>::iterator iter = _pContainer->vecWI.begin();

	int iVtxIdx = 0;
	for (iter; iter != _pContainer->vecWI.end(); ++iter, ++iVtxIdx)
	{
		if ((*iter).size() > 1)
		{
			sort((*iter).begin(), (*iter).end()
				, [](const tWeightsAndIndices& left, const tWeightsAndIndices& right)
				{
					return left.dWeight > right.dWeight;
				}
			);

			double dWeight = 0.f;
			for (UINT i = 0; i < (*iter).size(); ++i)
			{
				dWeight += (*iter)[i].dWeight;
			}

			double revision = 0.f;
			if (dWeight > 1.0)
			{
				revision = 1.0 - dWeight;
				(*iter)[0].dWeight += revision;
			}

			if ((*iter).size() >= 4)
			{
				(*iter).erase((*iter).begin() + 4, (*iter).end());
			}
		}

		float fWeights[4] = {};
		float fIndices[4] = {};

		for (UINT i = 0; i < (*iter).size(); ++i)
		{
			fWeights[i] = (float)(*iter)[i].dWeight;
			fIndices[i] = (float)(*iter)[i].iBoneIdx;
		}

		memcpy(&_pContainer->vecWeights[iVtxIdx], fWeights, sizeof(Vec4));
		memcpy(&_pContainer->vecIndices[iVtxIdx], fIndices, sizeof(Vec4));
	}
}

void FBXLoader::LoadKeyframeTransform(FbxNode* _pNode, FbxCluster* _pCluster
	, const FbxAMatrix& _matNodeTransform, int _iBoneIdx, tContainer* _pContainer)
{
	if (m_vecAnimClip.empty())
		return;

	FbxVector4	v1 = { 1, 0, 0, 0 };
	FbxVector4	v2 = { 0, 0, 1, 0 };
	FbxVector4	v3 = { 0, 1, 0, 0 };
	FbxVector4	v4 = { 0, 0, 0, 1 };
	FbxAMatrix	matReflect;
	matReflect.mData[0] = v1;
	matReflect.mData[1] = v2;
	matReflect.mData[2] = v3;
	matReflect.mData[3] = v4;

	FbxTime::EMode eTimeMode = m_pScene->GetGlobalSettings().GetTimeMode();
	for (int i = 0; i < m_vecAnimClip.size(); ++i)
	{
		FbxAnimStack* pAnimStack = m_pScene->FindMember<FbxAnimStack>(m_arrAnimName[i]->Buffer());
		m_pScene->SetCurrentAnimationStack(pAnimStack);

		m_vecBone[_iBoneIdx]->matBone = _matNodeTransform;
		FbxLongLong llStartFrame = m_vecAnimClip[i]->tStartTime.GetFrameCount(eTimeMode);
		FbxLongLong llEndFrame = m_vecAnimClip[i]->tEndTime.GetFrameCount(eTimeMode);

		vector<tKeyFrame> keyFrames;

		for (FbxLongLong j = llStartFrame; j < llEndFrame; ++j)
		{
			tKeyFrame tFrame = {};
			FbxTime   tTime = 0;

			tTime.SetFrame(j, eTimeMode);

			FbxAMatrix matFromNode = _pNode->EvaluateGlobalTransform(tTime) * _matNodeTransform;
			FbxAMatrix matCurTrans = matFromNode.Inverse() * _pCluster->GetLink()->EvaluateGlobalTransform(tTime);
			matCurTrans = matReflect * matCurTrans * matReflect;

			tFrame.dTime = tTime.GetSecondDouble();
			tFrame.matTransform = matCurTrans;

			keyFrames.push_back(tFrame);
		}
		m_vecBone[_iBoneIdx]->vecKeyFrame.insert(make_pair(m_vecAnimClip[i]->strName, keyFrames));
	}


}

void FBXLoader::LoadOffsetMatrix(FbxCluster* _pCluster
	, const FbxAMatrix& _matNodeTransform
	, int _iBoneIdx, tContainer* _pContainer)
{
	FbxAMatrix matClusterTrans;
	FbxAMatrix matClusterLinkTrans;

	_pCluster->GetTransformMatrix(matClusterTrans);
	_pCluster->GetTransformLinkMatrix(matClusterLinkTrans);

	// Reflect Matrix
	FbxVector4 V0 = { 1, 0, 0, 0 };
	FbxVector4 V1 = { 0, 0, 1, 0 };
	FbxVector4 V2 = { 0, 1, 0, 0 };
	FbxVector4 V3 = { 0, 0, 0, 1 };

	FbxAMatrix matReflect;
	matReflect[0] = V0;
	matReflect[1] = V1;
	matReflect[2] = V2;
	matReflect[3] = V3;

	FbxAMatrix matOffset;
	matOffset = matClusterLinkTrans.Inverse() * matClusterTrans * _matNodeTransform;
	matOffset = matReflect * matOffset * matReflect;

	m_vecBone[_iBoneIdx]->matOffset = matOffset;
}


void FBXLoader::LoadWeightsAndIndices(FbxCluster* _pCluster
	, int _iBoneIdx
	, tContainer* _pContainer)
{
	int iIndicesCount = _pCluster->GetControlPointIndicesCount();

	for (int i = 0; i < iIndicesCount; ++i)
	{
		tWeightsAndIndices tWI = {};

		tWI.iBoneIdx = _iBoneIdx;
		tWI.dWeight = _pCluster->GetControlPointWeights()[i];

		int iVtxIdx = _pCluster->GetControlPointIndices()[i];

		_pContainer->vecWI[iVtxIdx].push_back(tWI);
	}
}



int FBXLoader::FindBoneIndex(string _strBoneName)
{
	wstring strBoneName = wstring(_strBoneName.begin(), _strBoneName.end());

	for (UINT i = 0; i < m_vecBone.size(); ++i)
	{
		if (m_vecBone[i]->strBoneName == strBoneName)
			return i;
	}

	return -1;
}

FbxAMatrix FBXLoader::GetTransform(FbxNode* _pNode)
{
	FbxVector4 vT = _pNode->GetGeometricTranslation(FbxNode::eSourcePivot);
	FbxVector4 vR = _pNode->GetGeometricRotation(FbxNode::eSourcePivot);
	//FbxVector4 vR = _pNode->
	FbxVector4 vS = _pNode->GetGeometricScaling(FbxNode::eSourcePivot);

	//FbxVector4 vT;
	//FbxVector4 vR;
	//FbxVector4 vS;

	//vT.Set(vT.mData[0], vT.mData[1], -vT.mData[2]);
	//vR.Set(-vR.mData[0], -vR.mData[1], -vR.mData[2]);

	// This negate X,Y of Rotation Component of the matrix 
	// These 2 lines finally set "input" to the eventual converted result 
	//input.SetT(translation);
	//input.SetR(rotation);

	return FbxAMatrix(vT, vR, vS);
}
