#include "pch.h"
#include "LoadingMgr.h"

#include "CLayer.h"
#include "CLevel.h"
#include "CLevelMgr.h"
#include "CMeshRender.h"
#include "CResMgr.h"

LoadingMgr::LoadingMgr()
	: iStageNum(0)
{
}

LoadingMgr::~LoadingMgr()
{
}

void LoadingMgr::tick()
{

}

void LoadingMgr::begin()
{
	if (CLevelMgr::GetInst()->GetCurLevel()->GetName() != L"loading")
		return;

	CLevel* loadingLevel = CLevelMgr::GetInst()->GetLevel(L"loading");
	if (nullptr == loadingLevel)
		return;

	vector<CGameObject*> objs = loadingLevel->GetLayer((int)LAYER_TYPE::Default)->GetParentObject();
	if (objs.empty())
		return;

	Ptr<CTexture> pTex = CResMgr::GetInst()->FindRes<CTexture>(L"texture\\UI\\loading_opp.png");

	for (int i = 0; i < objs.size(); ++i)
	{
		if (objs[i]->GetName() == L"UI Loading Back")
		{

			switch (iStageNum)
			{
			case 1:
			{
				pTex = CResMgr::GetInst()->FindRes<CTexture>(L"texture\\UI\\loading_vault.png");
			}
			break;
			}

			objs[i]->MeshRender()->GetMaterial(0)->SetTexParam(TEX_0, pTex);
		}
		if (objs[i]->GetName() == L"smg_loading")
		{
			switch (iStageNum)
			{
			case 0:
			{
				objs[i]->SetObjectState(CGameObject::OBJECT_STATE::VISIBLE);
			}
			break;
			case 1:
			{
				objs[i]->SetObjectState(CGameObject::OBJECT_STATE::INVISIBLE);
			}
			break;
			}
		}
		if (objs[i]->GetName() == L"sniper_loading")
		{
			switch (iStageNum)
			{
			case 0:
			{
				objs[i]->SetObjectState(CGameObject::OBJECT_STATE::INVISIBLE);
			}
			break;
			case 1:
			{
				objs[i]->SetObjectState(CGameObject::OBJECT_STATE::VISIBLE);
			}
			break;
			}
		}
	}


}