#include "pch.h"
#include "CollisionUI.h"

#include <Engine\CollisionMgr.h>
#include <Engine\CLevelMgr.h>
#include <Engine\CLevel.h>
#include <Engine\CLayer.h>

CollisionUI::CollisionUI()
    : UI("##Collision")
{
    SetName("Collision");
}

CollisionUI::~CollisionUI()
{
}


void CollisionUI::init()
{
}

void CollisionUI::tick()
{
}

int CollisionUI::render_update()
{
    CLevel* curLevel = CLevelMgr::GetInst()->GetCurLevel();

    if (nullptr == curLevel) return FALSE;

	string str = "";
	static ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;
	ImVec2 outer_size = ImVec2(450.f, 300.f);

	wstring layerName = L"";
	wstring ohterLayerName = L"";

	string otherLayerNameStr = "";
	string layerNameStr = "";

	CLayer* layer = nullptr;
	CLayer* otherLayer = nullptr;

	int sum = 0;
	for (int col = 0; col < MAX_LAYER; ++col)
	{
		layer = curLevel->GetLayer(col);
		layerName = layer->GetName();

		if (layerName == L"") continue;

		++sum;
	}

	int freeze_cols = 1;
	int freeze_rows = 1;

	if (ImGui::BeginTable("LayerTable", sum + 1, flags))
	{
		ImGui::TableSetupScrollFreeze(freeze_cols, freeze_rows);
		ImGui::TableSetupColumn("Layer", ImGuiTableColumnFlags_NoHide);
		for (int row = 0; row < MAX_LAYER; ++row)
		{
			layer = curLevel->GetLayer(row);
			layerName = layer->GetName();

			if (layerName == L"") continue;

			layerNameStr = string(layerName.begin(), layerName.end());
			layerNameStr += "(" + std::to_string(row) + ")";
			ImGui::TableSetupColumn(layerNameStr.c_str());
		}
		ImGui::TableHeadersRow();

		for (int row = 0; row < MAX_LAYER; ++row)
		{
			layer = curLevel->GetLayer(row);
			layerName = layer->GetName();

			if (layerName != L"")
			{
				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				layerNameStr = string(layerName.begin(), layerName.end());
				layerNameStr += "(" + std::to_string(row) + ")";
				ImGui::Text(layerNameStr.c_str());

				for (int col = 0; col < MAX_LAYER; ++col)
				{
					otherLayer = curLevel->GetLayer(col);
					ohterLayerName = otherLayer->GetName();

					if (ohterLayerName == L"") continue;
					otherLayerNameStr = string(ohterLayerName.begin(), ohterLayerName.end());
					ImGui::TableNextColumn();

					otherLayer = curLevel->GetLayer(col);
					ohterLayerName = otherLayer->GetName();

					string m = "##" + layerNameStr + otherLayerNameStr;
					bool t = false;
					if (CollisionMgr::GetInst()->IsLayerIntersect(row, col))
					{
						t = true;
					}

					if(ImGui::Checkbox(m.c_str(), &t))
					{
						CollisionMgr::GetInst()->SetLayerIntersect(row, col, t);
					}
				}
			}
		}
		ImGui::EndTable();
	}
    return TRUE;
}