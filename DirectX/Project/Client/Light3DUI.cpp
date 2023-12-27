#include "pch.h"
#include "Light3DUI.h"

#include <Engine/CGameObject.h>
#include <Engine/CLight3D.h>

Light3DUI::Light3DUI()
	: ComponentUI("##Light3D",COMPONENT_TYPE::LIGHT3D)
{
	SetName("Light3D");
}

Light3DUI::~Light3DUI()
{
}

int Light3DUI::render_update()
{
   if (FALSE == ComponentUI::render_update())
    return FALSE;

   Vec3 vLightColor = GetTarget()->Light3D()->GetLightColor();
   Vec3 vAmbient = GetTarget()->Light3D()->GetLightAmbient();
   int iLightType = (int)GetTarget()->Light3D()->GetLightType();
   float fAngle = GetTarget()->Light3D()->GetAngle();
   float fRadius = GetTarget()->Light3D()->GetRadius();

	
	ImGui::Text("Light Color");
	ImGui::SameLine();
	ImGui::DragFloat3("##Light Color", vLightColor);

	ImGui::Text("Light Ambient");
	ImGui::SameLine();
	ImGui::DragFloat3("##Light Ambient", vAmbient);

	ImGui::Text("Light Type");
	ImGui::SameLine();
	ImGui::DragInt("##Light Type", &iLightType);

	ImGui::Text("Light Angle");
	ImGui::SameLine();
	ImGui::DragFloat("##Light Angle", &fAngle);

	ImGui::Text("Light Radius");
	ImGui::SameLine();
	ImGui::DragFloat("##Light Radius", &fRadius);

    return TRUE;
}
