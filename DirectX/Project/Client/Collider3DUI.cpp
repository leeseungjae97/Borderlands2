#include "pch.h"
#include "Collider3DUI.h"

#include <Engine\CCollider3D.h>
#include <Engine\CLevelMgr.h>
#include <Engine\CLevel.h>
#include <Engine\CLayer.h>

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

	CLevel* curLevel = CLevelMgr::GetInst()->GetCurLevel();
	UINT layerIdx = GetTarget()->GetLayerIndex();
	CLayer* layer1 = curLevel->GetLayer(layerIdx);
	wstring layerName1 = layer1->GetName();
	string layerName1Str = string(layerName1.begin(), layerName1.end());

	CCollider3D* col = GetTarget()->Collider3D();
	if (nullptr == col)
		return FALSE;

	UINT colLayerIdx = col->GetOwner()->GetLayerIndex();
	CLayer* colLayer1 = curLevel->GetLayer(colLayerIdx);
	wstring colLayerName1 = colLayer1->GetName();
	string colLayerName1Str = string(colLayerName1.begin(), colLayerName1.end());

	ImGui::Text("Target layer : %s(%d)", layerName1Str.c_str(), layerIdx);

	ImGui::Text("Collider Owner layer : %s(%d)", colLayerName1Str.c_str(), colLayerIdx);

	Vec3 vScale = col->GetScale();
	float fScale[3];

	fScale[0] = vScale.x;
	fScale[1] = vScale.y;
	fScale[2] = vScale.z;

	ImGui::Text("Collider Scale ");
	ImGui::InputFloat3("##Collider3dScale",fScale);

	vScale.x = fScale[0];
	vScale.y = fScale[1];
	vScale.z = fScale[2];

	//col->SetScale(vScale);

	Vec3 vOffset = col->GetOffset();
	float fOffset[3];

	fOffset[0] = vOffset.x;
	fOffset[1] = vOffset.y;
	fOffset[2] = vOffset.z;

	ImGui::Text("Collider Offset ");
	ImGui::InputFloat3("##Collider3dOffset", fOffset);

	vOffset.x = fOffset[0];
	vOffset.y = fOffset[1];
	vOffset.z = fOffset[2];

	//col->SetOffset(vOffset);

	if (col->GetAttachToRigidBody())
		return FALSE;

	PxTransform trans = col->GetColliderPos();
	Vec3 vPos = Vec3(trans.p.x, trans.p.y, trans.p.z);
	float fPos[3];

	fPos[0] = vPos.x;
	fPos[1] = vPos.y;
	fPos[2] = vPos.z;

	ImGui::Text("Collider Pos ");
	ImGui::InputFloat3("##Collider3dPos", fPos);

	vPos.x = fPos[0];
	vPos.y = fPos[1];
	vPos.z = fPos[2];

	//col->SetColliderPos(vPos);

	Vec3 vRot = QuaternionToVector3(Quat(trans.q.x, trans.q.y, trans.q.z, trans.q.w));
	float fRot[3];

	fRot[0] = vRot.x;
	fRot[1] = vRot.y;
	fRot[2] = vRot.z;

	ImGui::Text("Collider Rot ");
	ImGui::InputFloat3("##Collider3dRot", fRot);

	vRot.x = fRot[0];
	vRot.y = fRot[1];
	vRot.z = fRot[2];

	//col->SetColliderRot(vRot);

	return TRUE;
}
