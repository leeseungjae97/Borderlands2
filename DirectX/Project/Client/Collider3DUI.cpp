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

	ImGui::Text("Target layer : %s(%d)", layerName1Str.c_str(), layerIdx);

	CCollider3D* col = GetTarget()->Collider3D();
	UINT colLayerIdx = col->GetOwner()->GetLayerIndex();

	CLayer* colLayer1 = curLevel->GetLayer(colLayerIdx);
	wstring colLayerName1 = colLayer1->GetName();
	string colLayerName1Str = string(colLayerName1.begin(), colLayerName1.end());

	ImGui::Text("Collider Owner layer : %s(%d)", colLayerName1Str.c_str(), colLayerIdx);

	//GetTarget()->Collider3D();

	//string str = "Collider3D__";
	//ImGui::Text(str.c_str());

	return TRUE;
}
