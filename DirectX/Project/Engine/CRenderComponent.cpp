#include "pch.h"
#include "CRenderComponent.h"

#include "CResMgr.h"
#include "CTransform.h"

CRenderComponent::CRenderComponent(COMPONENT_TYPE _type)
	: CComponent(_type)
	, m_bFrustumCheck(true)
{
}

CRenderComponent::~CRenderComponent()
{
}

void CRenderComponent::SetMesh(Ptr<CMesh> _Mesh)
{
	m_pMesh = _Mesh;

	if (!m_vecMtrls.empty())
	{
		m_vecMtrls.clear();
		vector<tMtrlSet> vecMtrls;
		m_vecMtrls.swap(vecMtrls);
	}


	if (nullptr != m_pMesh)
		m_vecMtrls.resize(m_pMesh->GetSubsetCount());
}

Ptr<CMaterial> CRenderComponent::GetMaterial(UINT _Idx)
{
	if (m_vecMtrls.empty()) return nullptr;

	if (nullptr == m_vecMtrls[_Idx].pCurrentMtrl)
	{
		m_vecMtrls[_Idx].pCurrentMtrl = m_vecMtrls[_Idx].pSharedMtrl;
	}

	return m_vecMtrls[_Idx].pCurrentMtrl;
}

Ptr<CMaterial> CRenderComponent::GetSharedMaterial(UINT _Idx)
{
	if (m_vecMtrls.empty()) return nullptr;

	if (m_vecMtrls[_Idx].pCurrentMtrl != m_vecMtrls[_Idx].pSharedMtrl)
	{
		m_vecMtrls[_Idx].pCurrentMtrl = m_vecMtrls[_Idx].pSharedMtrl;
		m_vecMtrls[_Idx].pDynamicMtrl = nullptr;
	}

	return m_vecMtrls[_Idx].pSharedMtrl;
}

Ptr<CMaterial> CRenderComponent::GetDynamicMaterial(UINT _Idx)
{
	if (m_vecMtrls.empty()) return nullptr;

		// 원본 재질이 없다 -> Nullptr 반환
	if (nullptr == m_vecMtrls[_Idx].pSharedMtrl)
	{
		m_vecMtrls[_Idx].pCurrentMtrl = nullptr;
		return m_vecMtrls[_Idx].pCurrentMtrl;
	}

	// 동적 재질 최초 요청시 제작 해서 준다.
	if (nullptr == m_vecMtrls[_Idx].pDynamicMtrl)
	{		
		m_vecMtrls[_Idx].pDynamicMtrl = m_vecMtrls[_Idx].pSharedMtrl->Clone();		
	}

	// 동적 재질을 현재 사용재질로 지정한다.
	m_vecMtrls[_Idx].pCurrentMtrl = m_vecMtrls[_Idx].pDynamicMtrl;


	return m_vecMtrls[_Idx].pCurrentMtrl;
}

ULONG64 CRenderComponent::GetInstID(UINT _iMtrlIdx)
{
	if (m_vecMtrls.empty()) return 0;

	if (m_pMesh == NULL || m_vecMtrls[_iMtrlIdx].pCurrentMtrl == NULL)
		return 0;

	uInstID id{ (UINT)m_pMesh->GetID(), (WORD)m_vecMtrls[_iMtrlIdx].pCurrentMtrl->GetID(), (WORD)_iMtrlIdx };
	return id.llID;
}

void CRenderComponent::render_shadowmap()
{
	Ptr<CMaterial> pShadowMapMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"ShadowMapMtrl");

	Transform()->UpdateData();

	//pShadowMapMtrl->GetShader()->SetStencilRef(0);
	pShadowMapMtrl->UpdateData();

	GetMesh()->render(0);
}

void CRenderComponent::render_shadowmapexp()
{
	Ptr<CMaterial> pShadowMapMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"ShadowMapExpMtrl");

	Transform()->UpdateData();

	pShadowMapMtrl->UpdateData();

	GetMesh()->render(0);
}

void CRenderComponent::SetMaterial(Ptr<CMaterial> _Mtrl, UINT _Idx)
{
	if (m_vecMtrls.empty())	
		assert(nullptr);

	m_vecMtrls[_Idx].pSharedMtrl = _Mtrl;
	m_vecMtrls[_Idx].pCurrentMtrl = _Mtrl;
}

void CRenderComponent::SaveToLevelFile(FILE* _File)
{
	//COMPONENT_TYPE type = GetType();
	//fwrite(&type, sizeof(UINT), 1, _File);


	SaveResRef(m_pMesh.Get(), _File);

	UINT iMtrlCount = GetMtrlCount();
	fwrite(&iMtrlCount, sizeof(UINT), 1, _File);

	for (UINT i = 0; i < iMtrlCount; ++i)
	{
		SaveResRef(m_vecMtrls[i].pSharedMtrl.Get(), _File);
		//SaveResRef(m_vecMtrls[i].pDynamicMtrl.Get(), _File);
	}

	fwrite(&m_bDynamicShadow, 1, 1, _File);
	fwrite(&m_bFrustumCheck, 1, 1, _File);
	fwrite(&m_fBounding, 1, 1, _File);
}

void CRenderComponent::LoadFromLevelFile(FILE* _File)
{
	LoadResRef(m_pMesh, _File);

	UINT iMtrlCount = GetMtrlCount();
	fread(&iMtrlCount, sizeof(UINT), 1, _File);

	for (UINT i = 0; i < iMtrlCount; ++i)
	{
		Ptr<CMaterial> pCurMtrl;
		//Ptr<CMaterial> pDyMtrl;
		tMtrlSet mtrlSet;
		LoadResRef(pCurMtrl, _File);
		//LoadResRef(pDyMtrl, _File);
		//mtrlSet.pCurrentMtrl = pCurMtrl;
		//mtrlSet.pDynamicMtrl = pDyMtrl;
		m_vecMtrls.push_back(mtrlSet);
		SetMaterial(pCurMtrl, i);
	}

	fread(&m_bDynamicShadow, 1, 1, _File);
	fread(&m_bFrustumCheck, 1, 1, _File);
	fread(&m_fBounding, 1, 1, _File);
}
