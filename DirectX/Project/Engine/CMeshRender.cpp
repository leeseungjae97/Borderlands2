#include "pch.h"
#include "CMeshRender.h"

#include "CAnimator2D.h"
#include "CTransform.h"
#include "CAnimator3D.h"
#include "CRenderMgr.h"
#include "CResMgr.h"
#include "mMRT.h"

CMeshRender::CMeshRender()
	: CRenderComponent(COMPONENT_TYPE::MESHRENDER)
{
}

CMeshRender::~CMeshRender()
{
}

void CMeshRender::finaltick()
{
}

void CMeshRender::render()
{
	if (nullptr == GetMesh() || nullptr == GetMaterial(0))
		return;
	Ptr<CMaterial> pOutline = CResMgr::GetInst()->FindRes<CMaterial>(L"OutlineMtrl");
	MRT* mrt = CRenderMgr::GetInst()->GetCurMRT();

	vector<Ptr<CMaterial>> mats;
	vector<int> renderSubsets;
	for (int i = 0; i < GetMtrlCount(); ++i)
	{
		if (nullptr == GetMaterial(i)) continue;

		if (GetMaterial(i)->GetShader()->GetDomain() == SHADER_DOMAIN::DOMAIN_DEFERRED)
		{
			continue;
		}
		else
		{
			mats.push_back(GetMaterial(i));
			renderSubsets.push_back(i);
		}
	}

	// Transform 에 UpdateData 요청
	Transform()->UpdateData();

	if (Animator2D())
	{
		Animator2D()->UpdateData();
	}

	// Animator3D 업데이트
	if (Animator3D())
	{
		Animator3D()->UpdateData();

		for (UINT i = 0; i < GetMtrlCount(); ++i)
		{
			if (nullptr == GetMaterial(i))
				continue;

			GetMaterial(i)->SetAnim3D(true); // Animation Mesh 알리기
			GetMaterial(i)->SetBoneCount(Animator3D()->GetBoneCount());
		}
	}

	for (int i = 0; i < mats.size(); ++i)
	{
		// 재질 업데이트
		mats[i]->UpdateData();

		// 렌더
		GetMesh()->render(renderSubsets[i]);
	}
	//CRenderMgr::GetInst()->GetMRT(MRT_TYPE::OUT_LINE)->OMSet();

	//for (int i = 0; i < mats.size(); ++i)
	//{
	//	// 재질 업데이트
	//	pOutline->UpdateData();

	//	// 렌더
	//	GetMesh()->render(renderSubsets[i]);
	//}
	//CRenderMgr::GetInst()->GetMRT(MRT_TYPE::OUT_LINE_PLUS)->OMSet();
	//bool SizeUp = true;
	//pOutline->SetScalarParam(INT_0, &SizeUp);

	//for (int i = 0; i < mats.size(); ++i)
	//{
	//	// 재질 업데이트
	//	pOutline->UpdateData();

	//	// 렌더
	//	GetMesh()->render(renderSubsets[i]);
	//}
	//
	//mrt->OMSet();
	if (Animator3D())
		Animator3D()->ClearData();
	if (Animator2D())
		Animator2D()->ClearData();
}

void CMeshRender::render(UINT _iSubset, bool _Deferred)
{
	if (nullptr == GetMesh() || nullptr == GetMaterial(_iSubset))
		return;

	Ptr<CMaterial> pOutline = CResMgr::GetInst()->FindRes<CMaterial>(L"OutlineMtrl");
	MRT* mrt = CRenderMgr::GetInst()->GetCurMRT();
	if (_Deferred)
	{
		if (GetMaterial(_iSubset)->GetShader()->GetDomain() != SHADER_DOMAIN::DOMAIN_DEFERRED)
		{
			return;
		}
	}
	else
	{
		if (GetMaterial(_iSubset)->GetShader()->GetDomain() == SHADER_DOMAIN::DOMAIN_DEFERRED)
		{
			return;
		}
	}

	Transform()->UpdateData();

	if (Animator2D())
	{
		Animator2D()->UpdateData();
	}

	// Animator3D 업데이트
	if (Animator3D())
	{
		Animator3D()->UpdateData();
		GetMaterial(_iSubset)->SetAnim3D(true); // Animation Mesh 알리기
		GetMaterial(_iSubset)->SetBoneCount(Animator3D()->GetBoneCount());
	}

	// 사용할 재질 업데이트
	GetMaterial(_iSubset)->UpdateData();

	// 사용할 메쉬 업데이트 및 렌더링
	GetMesh()->render(_iSubset);

	//CRenderMgr::GetInst()->GetMRT(MRT_TYPE::OUT_LINE)->OMSet();

	//pOutline->UpdateData();
	//GetMesh()->render(_iSubset);

	//CRenderMgr::GetInst()->GetMRT(MRT_TYPE::OUT_LINE_PLUS)->OMSet();

	//bool SizeUp = true;
	//pOutline->SetScalarParam(INT_0, &SizeUp);
	//pOutline->UpdateData();
	//GetMesh()->render(_iSubset);
	//mrt->OMSet();

	if (Animator3D())
		Animator3D()->ClearData();
	if (Animator2D())
		Animator2D()->ClearData();
}
