#include "pch.h"
#include "LayerInspectorUI.h"

#include <Engine\CLevelMgr.h>
#include <Engine\CLayer.h>
#include <Engine\CLevel.h>

LayerInspectorUI::LayerInspectorUI()
	: UI("##LayerInspector")
{
	SetName("LayerInspector");
}

LayerInspectorUI::~LayerInspectorUI()
{
}

void LayerInspectorUI::init() { UI::init(); }
void LayerInspectorUI::tick() { UI::tick(); }

int LayerInspectorUI::render_update()
{
	CLevel* curLevel  = CLevelMgr::GetInst()->GetCurLevel();

	if (nullptr == curLevel) return FALSE;

	for(int i = 0 ; i < MAX_LAYER; ++i)
	{
		CLayer* layer = curLevel->GetLayer(i);
		const wstring layerName = layer->GetName();

		string layerNameStr = string(layerName.begin(), layerName.end());

		if (layerName == L"") continue;

		ImGui::Text(layerNameStr.c_str());
	}

	return TRUE;
}
