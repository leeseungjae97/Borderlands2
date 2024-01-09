#include "pch.h"
#include "CameraUI.h"

#include <Engine/CGameObject.h>
#include <Engine/CCamera.h>
CameraUI::CameraUI()
    : ComponentUI("##Camera", COMPONENT_TYPE::CAMERA)
{
    SetName("Camera");
}

CameraUI::~CameraUI()
{
}

int CameraUI::render_update()
{
    if (FALSE == ComponentUI::render_update())
        return FALSE;

    	for(int i = 0 ; i < 3; ++i)
	{
		float cF = GetTarget()->Camera()->GetFloatConstant(i);

		ImGui::Text("float coeff %d", i + 1);
		ImGui::SameLine();
		char str[50] = {};
		sprintf_s(str, "##float coeff %d", i + 1);
		//ImGui::DragFloat(str, &cF);
		ImGui::InputFloat(str, &cF, 1.f);
		GetTarget()->Camera()->SetFloatConstant(i, cF);
	}

    return TRUE;
}
