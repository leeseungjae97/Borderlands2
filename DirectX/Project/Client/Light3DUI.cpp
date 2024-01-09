#include "pch.h"
#include "Light3DUI.h"

#include <Engine/CGameObject.h>
#include <Engine/CLight3D.h>

Light3DUI::Light3DUI()
	: ComponentUI("##Light3D", COMPONENT_TYPE::LIGHT3D)
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
	float fLightDepthCoeff = GetTarget()->Light3D()->GetLightDepthCoeff();

	bool bGaus = GetTarget()->Light3D()->GetGaus();
	bool bShadow = GetTarget()->Light3D()->GetShadow();

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


	ImGui::Text("Light Shadow");
	ImGui::SameLine();
	if (ImGui::RadioButton("##Light Shadow", bShadow))
	{
		bShadow = bShadow != true;
	}

	ImGui::Text("Light Filter");
	ImGui::SameLine();
	if (ImGui::RadioButton("##Light Filter", bGaus))
	{
		bGaus = bGaus != true;
	}

	ImGui::Text("Light Depth Coeff");
	ImGui::SameLine();
	ImGui::DragFloat("##Light Depth Coeff", &fLightDepthCoeff);

	for(int i = 0 ; i < 3; ++i)
	{
		float cF = GetTarget()->Light3D()->GetFloatConstant(i);

		ImGui::Text("float coeff %d", i + 1);
		ImGui::SameLine();
		char str[50] = {};
		sprintf_s(str, "##float coeff %d", i + 1);
		//ImGui::DragFloat(str, &cF);
		ImGui::InputFloat(str, &cF, 1.f);
		GetTarget()->Light3D()->SetFloatConstant(i, cF);
	}

	GetTarget()->Light3D()->SetLightColor(vLightColor);
	GetTarget()->Light3D()->SetLightAmbient(vAmbient);
	GetTarget()->Light3D()->SetLightType((LIGHT_TYPE)iLightType);
	GetTarget()->Light3D()->SetLightDepthCoeff(fLightDepthCoeff);
	GetTarget()->Light3D()->SetGaus(bGaus);
	GetTarget()->Light3D()->SetShadow(bShadow);
	GetTarget()->Light3D()->SetAngle(fAngle);
	GetTarget()->Light3D()->SetRadius(fRadius);

	return TRUE;
}
