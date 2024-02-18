#include "pch.h"
#include "RenderUI.h"

#include <Engine\CRenderMgr.h>
#include <Engine\CCamera.h>

RenderUI::RenderUI()
	: UI("##RenderUI")
{
	SetName("RenderUI");
}

RenderUI::~RenderUI()
{
}


void RenderUI::init()
{
}
void RenderUI::tick()
{
}

int RenderUI::render_update()
{
	CCamera* mainCame = CRenderMgr::GetInst()->GetMainCam();

	ImGui::Text("HDR");
	if(ImGui::Button("ON##HDR_ON"))
	{
		mainCame->SetHDR(true);
	}
	ImGui::SameLine();
	if (ImGui::Button("OFF##HDR_OFF"))
	{
		mainCame->SetHDR(false);
	}

	ImGui::Text("Bloom");
	if (ImGui::Button("ON##Bloom_ON"))
	{
		mainCame->SetBloom(true);
	}
	ImGui::SameLine();
	if (ImGui::Button("OFF##Bloom_OFF"))
	{
		mainCame->SetBloom(false);
	}

	return TRUE;
}