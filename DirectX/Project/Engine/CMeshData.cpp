#include "pch.h"
#include "CMeshData.h"

#include "CGameObject.h"
#include "CMeshRender.h"
#include "CTransform.h"
#include "CMaterial.h"
#include "CPathMgr.h"
#include "CResMgr.h"

CMeshData::CMeshData(bool _bEngine)
	: CRes(RES_TYPE::MESHDATA, _bEngine)
{
}

CMeshData::~CMeshData()
{
}

CMeshData* CMeshData::LoadFromFBX(const wstring& _strFilePath)
{
	wstring strFullPath = CPathMgr::GetInst()->GetContentPath();
	strFullPath += _strFilePath;

	FBXLoader loader;
	loader.init();
	loader.LoadFbx(strFullPath);

	// 메쉬 가져오기
	CMesh* pMesh = nullptr;
	pMesh = CMesh::CreateFromContainer(loader);

	// ResMgr 에 메쉬 등록
	if (nullptr != pMesh)
	{
		wstring strMeshKey = L"mesh\\";
		strMeshKey += path(strFullPath).stem();
		strMeshKey += L".mesh";
		CResMgr::GetInst()->AddRes<CMesh>(strMeshKey, pMesh);

		// 메시를 실제 파일로 저장
		pMesh->Save(strMeshKey);
	}


	vector<Ptr<CMaterial>> vecMtrl;

	// 메테리얼 가져오기
	for (UINT i = 0; i < loader.GetContainer(0).vecMtrl.size(); ++i)
	{
		Ptr<CMaterial> pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(loader.GetContainer(0).vecMtrl[i].strMtrlName);
		// 예외처리 (material 이름이 입력 안되어있을 수도 있다.)
		assert(pMtrl.Get());

		vecMtrl.push_back(pMtrl);
	}

	CMeshData* pMeshData = new CMeshData(true);
	pMeshData->m_pMesh = pMesh;
	pMeshData->m_vecMtrl = vecMtrl;

	return pMeshData;
}

int CMeshData::Save(const wstring&)
{
	return S_OK;
}

int CMeshData::Load(const wstring& _strFilePath)
{
	return S_OK;
}

CGameObject* CMeshData::Instantiate()
{
	CGameObject* pNewObj = new CGameObject;
	pNewObj->AddComponent(new CTransform);
	pNewObj->AddComponent(new CMeshRender);

	pNewObj->MeshRender()->SetMesh(m_pMesh);

	for (UINT i = 0; i < m_vecMtrl.size(); ++i)
	{
		pNewObj->MeshRender()->SetMaterial(m_vecMtrl[i], i);
	}

	return pNewObj;
}
