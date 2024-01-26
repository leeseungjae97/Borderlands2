#include "pch.h"
#include "RigidBodyUI.h"

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

	return TRUE;
}
