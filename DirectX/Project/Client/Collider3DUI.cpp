#include "pch.h"
#include "Collider3DUI.h"

#include <Engine\CCollider3D.h>

Collider3DUI::Collider3DUI()
	: ComponentUI("##Collider3D", COMPONENT_TYPE::COLLIDER3D)
{
	SetName("Collider3D");
}

Collider3DUI::~Collider3DUI()
{

}
int Collider3DUI::render_update()
{
	if (FALSE == ComponentUI::render_update())
		return FALSE;
	//GetTarget()->Collider3D();

	//string str = "Collider3D__";
	//ImGui::Text(str.c_str());

	return TRUE;
}
