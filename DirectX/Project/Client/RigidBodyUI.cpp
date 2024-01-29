#include "pch.h"
#include "RigidBodyUI.h"

#include <Engine/CRigidBody.h>

#include <Engine/CGameObject.h>

RigidBodyUI::RigidBodyUI()
	: ComponentUI("##RigidBody", COMPONENT_TYPE::RIGIDBODY)
{
	SetName("RigidBody");
}

RigidBodyUI::~RigidBodyUI()
{
}

int RigidBodyUI::render_update()
{
	if (FALSE == ComponentUI::render_update())
		return FALSE;

	RIGID_BODY_TYPE _tby = GetTarget()->RigidBody()->GetRigidType();
	RIGID_BODY_SHAPE_TYPE _tbsy = GetTarget()->RigidBody()->GetRigidShapeType();


	ImGui::Text("%s", _tby == RIGID_BODY_TYPE::STATIC ? "STATIC" : "DYNAMIC");

	ImGui::Text("%s", _tbsy == RIGID_BODY_SHAPE_TYPE::BOX ? "BOX" : _tbsy == RIGID_BODY_SHAPE_TYPE::SPHERE ? "SPHERE" : "MESH");

	return TRUE;
}
