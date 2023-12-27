#include "pch.h"
#include "ParticleSystemUI.h"

#include <Engine/CGameObject.h>
#include <Engine/CParticleSystem.h>

ParticleSystemUI::ParticleSystemUI()
    : ComponentUI("##ParticleSystem", COMPONENT_TYPE::PARTICLESYSTEM)
{
    SetName("ParticleSystem");
}

ParticleSystemUI::~ParticleSystemUI()
{
}

int ParticleSystemUI::render_update()
{
    if (FALSE == ComponentUI::render_update())
        return FALSE;

    Vec2 _IntervalDistance = GetTarget()->ParticleSystem()->GetIntervalDistance();

    float vIntervalDistance[2] = { _IntervalDistance.x, _IntervalDistance.y};
    float _MaxScaleFator = GetTarget()->ParticleSystem()->GetMaxScaleFator();
    float _MinScaleFator = GetTarget()->ParticleSystem()->GetMinScaleFator();

    ImGui::Text("Interval Distance");
    ImGui::SameLine();
    ImGui::DragFloat2("##Interval Distance", vIntervalDistance);

    ImGui::Text("Max Scale Factor");
    ImGui::SameLine();
    ImGui::DragFloat("##Max Scale Factor", &_MaxScaleFator);

    ImGui::Text("Min Scale Factor");
    ImGui::SameLine();
    ImGui::DragFloat("##Min Scale Factor", &_MinScaleFator);

    GetTarget()->ParticleSystem()->SetParticleIntervalDistance(Vec2(vIntervalDistance[0], vIntervalDistance[1]));
    GetTarget()->ParticleSystem()->SetMaxScaleFator(_MaxScaleFator);
    GetTarget()->ParticleSystem()->SetMinScaleFator(_MinScaleFator);

    return TRUE;
}
