#include "pch.h"
#include "TransformUI.h"

#include <Engine\CGameObject.h>
#include <Engine\CTransform.h>

TransformUI::TransformUI()
	: ComponentUI("##Transform", COMPONENT_TYPE::TRANSFORM)	
{
	SetName("Transform");
}

TransformUI::~TransformUI()
{
}

int TransformUI::render_update()
{
	if (FALSE == ComponentUI::render_update())
		return FALSE;

	Vec3 vPos = GetTarget()->Transform()->GetRelativePos();
	Vec3 vScale = GetTarget()->Transform()->GetRelativeScale();
	Vec3 vRotation = GetTarget()->Transform()->GetRelativeRot();
	Vec3 vFollowOffset = GetTarget()->Transform()->GetFollowOffset();
	wstring name = GetTarget()->GetName();

	string str = WStringToString(name);

	vRotation = (vRotation / XM_PI) * 180.f;
	ImGui::Text("Object Name");
	ImGui::SameLine();
	ImGui::InputText("##Object name", &str);
	if (ImGui::IsKeyPressed(ImGuiKey_Enter))
	{
		string t_name = WStringToString(name);
		if (Stricmp(str.c_str(), t_name.c_str()))
		{
			GetTarget()->SetName(StringToWString(str));
			LevelRecognize();
		}
	}

	ImGui::Text("Position");
	ImGui::SameLine();
	ImGui::DragFloat3("##Relative Position", vPos);

	ImGui::Text("Scale   ");
	ImGui::SameLine();
	ImGui::DragFloat3("##Relative Scale", vScale);

	ImGui::Text("Rotation");
	ImGui::SameLine();
	ImGui::DragFloat3("##Relative Rotation", vRotation);

	ImGui::Text("Follow Offset");
	ImGui::SameLine();
	ImGui::DragFloat3("##Follow Offset", vFollowOffset);

	GetTarget()->Transform()->SetRelativePos(vPos);
	GetTarget()->Transform()->SetRelativeScale(vScale);
	
	vRotation = (vRotation / 180.f) * XM_PI;
	GetTarget()->Transform()->SetRelativeRot(vRotation);
	GetTarget()->Transform()->SetFollowOffset(vFollowOffset);

	return TRUE;
}
