#include "pch.h"
#include "DefineAnimationUI.h"

#include <Engine/CAnimator3D.h>
#include <Engine/CGameObject.h>
#include <Engine/CEventMgr.h>
#include <Engine/CLevelMgr.h>
#include <Engine/CLevel.h>

#include "InspectorUI.h"
#include "OutlinerUI.h"

DefineAnimationUI::DefineAnimationUI()
	: UI("##DefineAnimationUI")
{
	SetName("DefienAnimationUI");
}

DefineAnimationUI::~DefineAnimationUI()
{
}

int DefineAnimationUI::render_update()
{
	if (CEventMgr::GetInst()->IsLevelChanged())
		return FALSE;

	if (CLevelMgr::GetInst()->GetCurLevel()->GetState() == LEVEL_STATE::PLAY)
		return FALSE;

	OutlinerUI* outliner = (OutlinerUI*)ImGuiMgr::GetInst()->FindUI("##Outliner");

	if (nullptr == outliner)
		return FALSE;

	if (outliner->IsOutlinearReset())
		return FALSE;

	CGameObject* pSelectedObject = outliner->GetSelectedObject();

	if (nullptr == pSelectedObject || nullptr == pSelectedObject->Animator3D())
		return FALSE;

	const char** STRS = nullptr;
	int STRSSIZE = 0;
	if (pSelectedObject->IsItem())
	{
		STRS = GUN_ANIMATION_TYPE_STR;
		STRSSIZE = (int)GUN_ANIMATION_TYPE::END;
	}
	else if (pSelectedObject->IsWarrior())
	{
		STRS = WARRIOR_ANIMATION_TYPE_STR;
		STRSSIZE = (int)WARRIOR_ANIMATION_TYPE::END;
	}
	else if (pSelectedObject->GetLayerIndex() == (int)LAYER_TYPE::Enemy)
	{
		STRS = ENEMY_ANIMATION_TYPE_STR;
		STRSSIZE = (int)ENEMY_ANIMATION_TYPE::END;
	}
	else
	{
		STRS = PLAYER_ANIMATION_TYPE_STR;
		STRSSIZE = (int)PLAYER_ANIMATION_TYPE::END;
	}
	for(int i = 0 ; i < STRSSIZE; ++i)
	{
		ImGui::Text(STRS[i]);
		ImGui::SameLine();
		wstring animName = pSelectedObject->Animator3D()->GetDefineAnimationName(i);
		string str(animName.begin(), animName.end());
		ImGui::Text(str.c_str());
	}

	return TRUE;
}
