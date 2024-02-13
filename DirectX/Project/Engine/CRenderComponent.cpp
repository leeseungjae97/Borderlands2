#include "pch.h"
#include "CRenderComponent.h"

#include "CAnimator3D.h"
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
	//if(Animator2D())
	//{
	//	Animator2D()->UpdateData();
	//}

	if (Animator3D())
	{
		Animator3D()->UpdateData();

		pShadowMapMtrl->SetAnim3D(true); // Animation Mesh 알리기
		pShadowMapMtrl->SetBoneCount(Animator3D()->GetBoneCount());
	}else
	{
		pShadowMapMtrl->SetAnim3D(false);
	}

	Transform()->UpdateData();

	for (int i = 0; i < GetMesh()->GetSubsetCount(); ++i)
	{
		// 재질 업데이트
		pShadowMapMtrl->UpdateData();

		// 렌더
		GetMesh()->render(i);
	}
	//if(GetMesh()->GetSubsetCount() == 0)
	//{
	//	GetMesh()->render(0);
	//}

	if (Animator3D())
		Animator3D()->ClearData();
	//if (Animator2D())
	//	Animator2D()->ClearData();
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

		bool cloneDynamic = true;
		if(nullptr != m_vecMtrls[i].pDynamicMtrl)
		{
			fwrite(&cloneDynamic, sizeof(bool), 1, _File);
		}else
		{
			cloneDynamic = false;
			fwrite(&cloneDynamic, sizeof(bool), 1, _File);
		}
		//SaveResRef(m_vecMtrls[i].pDynamicMtrl.Get(), _File);
	}

	//fwrite(&m_bDynamicShadow, 1, 1, _File);
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
		Ptr<CMaterial> pSharedMtrl;
		Ptr<CMaterial> pDynamicMtrl;
		tMtrlSet mtrlSet;
		LoadResRef(pSharedMtrl, _File);
		mtrlSet.pSharedMtrl= pSharedMtrl;

		m_vecMtrls.push_back(mtrlSet);
		SetMaterial(pSharedMtrl, i);

		bool cloneDynamic = false;
		fread(&cloneDynamic, sizeof(bool), 1, _File);
		
		if (cloneDynamic)
		{
			mtrlSet.pDynamicMtrl = mtrlSet.pSharedMtrl->Clone();
			GetDynamicMaterial(i);
		}
	}

	//fread(&m_bDynamicShadow, 1, 1, _File);
	fread(&m_bFrustumCheck, 1, 1, _File);
	fread(&m_fBounding, 1, 1, _File);
}
