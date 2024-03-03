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

	ImGui::Text("Outline");
	if(ImGui::Button("ON##Outline_on"))
	{
		mainCame->SetOutline(true);
	}
	ImGui::SameLine();
	if (ImGui::Button("OFF##Outline_off"))
	{
		mainCame->SetOutline(false);
	}

	return TRUE;
}