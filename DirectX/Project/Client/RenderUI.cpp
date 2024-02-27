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

	//ImGui::Text("HDR");
	//if(ImGui::Button("ON##HDR_ON"))
	//{
	//	mainCame->SetHDR(true);
	//}
	//ImGui::SameLine();
	//if (ImGui::Button("OFF##HDR_OFF"))
	//{
	//	mainCame->SetHDR(false);
	//}

	//ImGui::Text("Bloom");
	//if (ImGui::Button("ON##Bloom_ON"))
	//{
	//	mainCame->SetBloom(true);
	//}
	//ImGui::SameLine();
	//if (ImGui::Button("OFF##Bloom_OFF"))
	//{
	//	mainCame->SetBloom(false);
	//}

	//ImGui::Text("Blur");
	//if (ImGui::Button("ON##Blur_ON"))
	//{
	//	mainCame->SetBlur(true);
	//}
	//ImGui::SameLine();
	//if (ImGui::Button("OFF##Blur_OFF"))
	//{
	//	mainCame->SetBlur(false);
	//}

	//float factor= mainCame->GetFactor();
	//ImGui::Text("Blur Factor");
	//ImGui::InputFloat("##factor", &factor);

	//mainCame->SetFactor(factor);

	//factor = mainCame->GetFactor1();
	//ImGui::Text("Blur Factor2");
	//ImGui::InputFloat("##factor2", &factor);

	//mainCame->SetFactor1(factor);

	//factor = mainCame->GetFactor2();
	//ImGui::Text("Blur Factor3");
	//ImGui::InputFloat("##factor3", &factor);

	//mainCame->SetFactor2(factor);


	//Vec4 vColor = mainCame->GetColor();
	//float fColor[4];
	//fColor[0] = vColor.x;
	//fColor[1] = vColor.y;
	//fColor[2] = vColor.z;
	//fColor[3] = vColor.w;
	//ImGui::Text("Color");
	//ImGui::InputFloat4("##colorcolor", fColor);
	//vColor.x = fColor[0];
	//vColor.y = fColor[1];
	//vColor.z = fColor[2];
	//vColor.w = fColor[3];
	//mainCame->SetColor(vColor);

	//Vec4 vImpact = mainCame->GetImpact();
	//fColor[0] = vImpact.x;
	//fColor[1] = vImpact.y;
	//fColor[2] = vImpact.z;
	//fColor[3] = vImpact.w;
	//ImGui::Text("Impact");
	//ImGui::InputFloat4("##impactimpact", fColor);
	//vImpact.x = fColor[0];
	//vImpact.y = fColor[1];
	//vImpact.z = fColor[2];
	//vImpact.w = fColor[3];
	//mainCame->SetImpact(vImpact);

	return TRUE;
}