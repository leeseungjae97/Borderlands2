#include "pch.h"
#include "PlacementUI.h"

#include <Engine/CGameObject.h>
#include <Engine/CTransform.h>

#include "OutlinerUI.h"

#include <Engine/NavigationMgr.h>

PlacementUI::PlacementUI()
	: UI("##PlacementUI")
	, m_bFollow(false)
{
	SetName("PlacementUI");
}

PlacementUI::~PlacementUI()
{
}

void PlacementUI::init()
{
}
void PlacementUI::tick()
{
}

int PlacementUI::render_update()
{
	OutlinerUI* outliner = (OutlinerUI*)ImGuiMgr::GetInst()->FindUI("##Outliner");

	if (nullptr == outliner)
		return FALSE;

	if (outliner->IsOutlinearReset())
		return FALSE;

	CGameObject* pSelectedObject = outliner->GetSelectedObject();
	ImGui::Text("State : %s", m_bFollow ? "FOLLOW" : "NOT FOLLOW");
	if (ImGui::IsKeyPressed(ImGuiKey_PageUp))
	{
		m_bFollow = !m_bFollow;
	}
	if(m_bFollow)
	{
		Vec3 vPos = NavigationMgr::GetInst()->GetMapRaycastPos();
		if(pSelectedObject)
		{
			if(vPos != Vec3::Zero)
			{
				Vec3 vScale = pSelectedObject->Transform()->GetRelativeScale();
				vPos.y += vScale.y / 2.f;
				pSelectedObject->Transform()->SetRelativePos(vPos);
			}
				
		}
	}
}