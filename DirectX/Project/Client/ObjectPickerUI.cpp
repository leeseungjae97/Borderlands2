#include "pch.h"
#include "ObjectPickerUI.h"

#include <Engine/ObjPickerMgr.h>

#include <Engine/CGameObject.h>
#include <Engine/CRigidBody.h>
#include <Engine/CTransform.h>

ObjectPickerUI::ObjectPickerUI()
	: UI("##ObjectPicker")
{
	SetName("ObjectPicker");
}

ObjectPickerUI::~ObjectPickerUI()
{
}

void ObjectPickerUI::init()
{
	UI::init();
}
void ObjectPickerUI::tick()
{
	UI::tick();
}

int ObjectPickerUI::render_update()
{
	CGameObject* _obj = ObjPickerMgr::GetInst()->GetPickObj();
	if (nullptr == _obj) return FALSE;
	string str(_obj->GetName().begin(), _obj->GetName().end());
	ImGui::Text(str.c_str());
	Vec3 vPos = _obj->Transform()->GetRelativePos();
	Vec3 vRot = _obj->Transform()->GetRelativeRot();

	vRot *= 180 / XM_PI;

	ImGui::Text("Transform");
	str = "x : "
		+ std::to_string(vPos.x)
		+ "\ny : "
		+ std::to_string(vPos.y)
		+ "\nz : "
		+ std::to_string(vPos.z);
	ImGui::Text(str.c_str());

	ImGui::Text("Rotation");
	str = "x : "
		+ std::to_string(vRot.x)
		+ "\ny : "
		+ std::to_string(vRot.y)
		+ "\nz : "
		+ std::to_string(vRot.z);
	ImGui::Text(str.c_str());
	
	CRigidBody* _db = _obj->RigidBody();
	if(_db)
	{
		ImGui::Text("State : %s", _db->IsCreature() ? "Creature" : "Object");

		PxRigidDynamic* _dy = _db->GetDynamicBody();
		if(_dy)
		{
			PxVec3 _linear_velocity = _dy->getLinearVelocity();
			str = "Linear Velocity\nx :"
				+ std::to_string(_linear_velocity.x)
				+ "\ny : "
				+ std::to_string(_linear_velocity.y)
				+ "\nz : "
				+ std::to_string(_linear_velocity.z);

			ImGui::Text(str.c_str());

			PxVec3 _angular_velocity = _dy->getAngularVelocity();
			str = "Angular Velocity\nx :"
				+ std::to_string(_angular_velocity.x)
				+ "\ny : "
				+ std::to_string(_angular_velocity.y)
				+ "\nz : "
				+ std::to_string(_angular_velocity.z);
		}

		ImGui::Text(str.c_str());
	}

	

	return TRUE;
}
