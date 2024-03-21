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

	Transform()->UpdateData();

	if (Animator2D())
	{
		Animator2D()->UpdateData();
	}

	if (Animator3D())
	{
		Animator3D()->UpdateData();

		for (UINT i = 0; i < GetMtrlCount(); ++i)
		{
			if (nullptr == GetMaterial(i))
				continue;

			GetMaterial(i)->SetAnim3D(true);
			GetMaterial(i)->SetBoneCount(Animator3D()->GetBoneCount());
		}
	}

	for (int i = 0; i < mats.size(); ++i)
	{
		mats[i]->UpdateData();

		GetMesh()->render(renderSubsets[i]);
	}

	if (Animator3D())
		Animator3D()->ClearData();
}

void CMeshRender::render(UINT _iSubset, bool _Deferred)
{
	if (nullptr == GetMesh() || nullptr == GetMaterial(_iSubset))
		return;

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

	if (Animator2D())
	{
		Animator2D()->UpdateData();
	}

	if (Animator3D())
	{
		Animator3D()->UpdateData();
		GetMaterial(_iSubset)->SetAnim3D(true);
		GetMaterial(_iSubset)->SetBoneCount(Animator3D()->GetBoneCount());
	}

	GetMaterial(_iSubset)->UpdateData();

	Transform()->UpdateData();

	GetMesh()->render(_iSubset);

	if (Animator3D())
		Animator3D()->ClearData();
}
