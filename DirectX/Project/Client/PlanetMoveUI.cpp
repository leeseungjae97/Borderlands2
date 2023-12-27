#include "pch.h"
#include "PlanetMoveUI.h"

#include <Engine/CGameObject.h>
#include <Engine/CPlanetMove.h>

PlanetMoveUI::PlanetMoveUI()
	: ComponentUI("##PlanetMove", COMPONENT_TYPE::PLANET_MOVE)	
{
	SetName("PlanetMove");
}

PlanetMoveUI::~PlanetMoveUI()
{
}

int PlanetMoveUI::render_update()
{
	if (FALSE == ComponentUI::render_update())
		return FALSE;

	float fOrbitTheta = GetTarget()->PlanetMove()->GetOrbitTheta();

	ImGui::Text("Orbit Theta");
	ImGui::SameLine();
	ImGui::DragFloat("##Orbit Theta", &fOrbitTheta);

	GetTarget()->PlanetMove()->SetOrbitTheta(fOrbitTheta);

    return TRUE;
}