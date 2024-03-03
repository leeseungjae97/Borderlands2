#include "pch.h"
#include "ObjectPickerUI.h"

#include <Engine/ObjPickerMgr.h>

#include <Engine/CGameObject.h>
#include <Engine/CRigidBody.h>
#include <Engine/CTransform.h>

#include "OutlinerUI.h"

ObjectPickerUI::ObjectPickerUI()
	: UI("##ObjectPicker")
	, m_pObj(nullptr)
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
	OutlinerUI* outliner = (OutlinerUI*)ImGuiMgr::GetInst()->FindUI("##Outliner");

	if (nullptr == outliner)
		return FALSE;

	if (outliner->IsOutlinearReset())
		return FALSE;

	if (nullptr == outliner) return FALSE;

	CGameObject* pSelectedObject = outliner->GetSelectedObject();

	if (ImGui::IsKeyPressed(ImGuiKey_PageDown))
	{
		m_pObj = pSelectedObject;
	}
	if (nullptr == m_pObj) return FALSE;

	string str(m_pObj->GetName().begin(), m_pObj->GetName().end());
	ImGui::Text(str.c_str());
	Vec3 vPos = m_pObj->Transform()->GetRelativePos();
	Vec3 vRot = m_pObj->Transform()->GetRelativeRot();

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
	
	CRigidBody* _db = m_pObj->RigidBody();
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

			//PxShape* pp = nullptr;
			//_dy->getShapes(&pp, sizeof(PxShape));


			PxVec3 _angular_velocity = _dy->getAngularVelocity();
			str = "Angular Velocity\nx :"
				+ std::to_string(_angular_velocity.x)
				+ "\ny : "
				+ std::to_string(_angular_velocity.y)
				+ "\nz : "
				+ std::to_string(_angular_velocity.z);

			ImGui::Text(str.c_str());


			PxTransform _global_trans = _dy->getGlobalPose();
			str = "_global_trans pose\nx :"
				+ std::to_string(_global_trans.p.x)
				+ "\ny : "
				+ std::to_string(_global_trans.p.y)
				+ "\nz : "
				+ std::to_string(_global_trans.p.z);

			ImGui::Text(str.c_str());

			PxTransform _mass_local_trans = _dy->getCMassLocalPose();
			str = "_mass_local_trans pose\nx :"
				+ std::to_string(_mass_local_trans.p.x)
				+ "\ny : "
				+ std::to_string(_mass_local_trans.p.y)
				+ "\nz : "
				+ std::to_string(_mass_local_trans.p.z);

			ImGui::Text(str.c_str());
		}


	}

	

	return TRUE;
}
