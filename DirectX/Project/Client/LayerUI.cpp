#include "pch.h"
#include "LayerUI.h"

#include <Engine\CLevelMgr.h>
#include <Engine\CLayer.h>
#include <Engine\CLevel.h>

LayerUI::LayerUI()
	: UI("##Layer")
{
	SetName("Layer");
}

LayerUI::~LayerUI()
{
}

void LayerUI::init() { UI::init(); }
void LayerUI::tick() { UI::tick(); }

int LayerUI::render_update()
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
