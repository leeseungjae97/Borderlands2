#include "pch.h"
#include "Animator2DUI.h"

#include <Engine/CAnimator2D.h>
#include <Engine/CGameObject.h>

Animator2DUI::Animator2DUI()
	: ComponentUI("##Animator2DUI", COMPONENT_TYPE::ANIMATOR2D)
{
	SetName("Animator2DUI");
}

Animator2DUI::~Animator2DUI()
{
}


int Animator2DUI::render_update() {
	if (FALSE == ComponentUI::render_update())
		return FALSE;

	int index = GetTarget()->Animator2D()->GetSpriteIndex();
	CAnimSprite* animSprite = GetTarget()->Animator2D()->GetActiveAnimation();
	if (nullptr == animSprite)
		return FALSE;

	Ptr<CTexture> tex = animSprite->GetSheet();
	
	ImGui::Text("Texture : ");
	ImVec2 uv_min = ImVec2(0.0f, 0.0f);						// Top-left
	ImVec2 uv_max = ImVec2(1.0f, 1.0f);						// Lower-right
	ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);   // No tint
	ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.5f); // 50% opaque white
	ImGui::Image((ImTextureID)tex->GetSRV().Get(), ImVec2(150, 150), uv_min, uv_max, tint_col, border_col);

	return TRUE;
}