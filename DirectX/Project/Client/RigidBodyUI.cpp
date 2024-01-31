#include "pch.h"
#include "RigidBodyUI.h"

#include <Engine/CRigidBody.h>

#include <Engine/CGameObject.h>
#include <Engine/CCollider3D.h>

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

	ImGui::Text("%s : %s", _tby == RIGID_BODY_TYPE::STATIC ? "STATIC" : "DYNAMIC", _tbsy == RIGID_BODY_SHAPE_TYPE::BOX ? "BOX" : _tbsy == RIGID_BODY_SHAPE_TYPE::SPHERE ? "SPHERE" : "MESH");

	CCollider3D* col = nullptr;

	if (_tby == RIGID_BODY_TYPE::STATIC)
	{
		PxRigidStatic* _rs = GetTarget()->RigidBody()->GetStaticBody();
		col = static_cast<CCollider3D*>(_rs->userData);
		
	}else
	{
		PxRigidDynamic* _rd = GetTarget()->RigidBody()->GetDynamicBody();
		col = static_cast<CCollider3D*>(_rd->userData);
	}

	if (col)
	{
		UINT colLayerIdx = col->GetOwner()->GetLayerIndex();
		ImGui::Text("Rigid userData Layer index : %d", colLayerIdx);
	}

	return TRUE;
}
