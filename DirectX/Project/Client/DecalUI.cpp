#include "pch.h"
#include "DecalUI.h"

#include <Engine/CGameObject.h>
#include <Engine/CDecal.h>

DecalUI::DecalUI()
	: ComponentUI("##Decal", COMPONENT_TYPE::DECAL)
{
	SetName("Decal");
}

DecalUI::~DecalUI()
{
}

int DecalUI::render_update()
{
	if (FALSE == ComponentUI::render_update())
		return FALSE;

	ImVec2 uv_min = ImVec2(0.0f, 0.0f);                 // Top-left
	ImVec2 uv_max = ImVec2(1.0f, 1.0f);                 // Lower-right
	ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);   // No tint
	ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.5f); // 50% opaque white

	Ptr<CTexture> pDecalTex = GetTarget()->Decal()->GetDecalTex();
	ImGui::Text("Height Map Texture");
	ImGui::Image((ImTextureID)pDecalTex->GetSRV().Get(), ImVec2(150, 150), uv_min, uv_max, tint_col, border_col);

	return TRUE;
}
