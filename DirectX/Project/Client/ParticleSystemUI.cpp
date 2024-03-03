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

    tParticleModule data = GetTarget()->ParticleSystem()->GetModuleData();
    float fInput[3];
    int inputInt = 0;
    Vec3 vT; string str;

    bool fire = GetTarget()->ParticleSystem()->GetFire();
    ImGui::Text("Fire");
    ImGui::SameLine();
    str = fire ? "true" : "false";
    str += "##fire ";
    if (ImGui::Button(str.c_str()))
    {
        fire = !fire;
    };
    GetTarget()->ParticleSystem()->SetFire(fire);

    ImGui::Text("Velocity Type");
    inputInt = data.AddVelocityType;
    ImGui::SameLine();
    ImGui::InputInt("##velocityType", &inputInt);
    data.AddVelocityType = inputInt;

    ImGui::Text("Space Type");
    inputInt = data.Space;
    ImGui::SameLine();
    ImGui::InputInt("##spaceType", &inputInt);
    data.Space = inputInt;

    ImGui::Text("Velocity Scale Type");
    inputInt = data.VelocityScale;
    ImGui::SameLine();
    ImGui::InputInt("##velocityScaleType", &inputInt);
    data.VelocityScale = inputInt;

    ImGui::Text("Spawn Shape Type");
    inputInt = data.SpawnShapeType;
    ImGui::SameLine();
    ImGui::InputInt("##SpawnShapeType", &inputInt);
    data.SpawnShapeType = inputInt;


    ImGui::Text("Box Shape Scale");
    
    vT = data.vBoxShapeScale;
    fInput[0] = vT.x;
    fInput[1] = vT.y;
    fInput[2] = vT.z;

    ImGui::SameLine();

    ImGui::InputFloat3("##vBoxShapeScale", fInput);
    vT.x = fInput[0];
    vT.y = fInput[1];
    vT.z = fInput[2];

    data.vBoxShapeScale = vT;

    ImGui::Text("Spawn Scale Min");

    vT = data.vSpawnScaleMin;
    fInput[0] = vT.x;
    fInput[1] = vT.y;
    fInput[2] = vT.z;

    ImGui::SameLine();

    ImGui::InputFloat3("##vSpawnScaleMin", fInput);
    vT.x = fInput[0];
    vT.y = fInput[1];
    vT.z = fInput[2];

    data.vSpawnScaleMin = vT;

    ImGui::Text("Spawn Scale Max");

    vT = data.vSpawnScaleMax;
    fInput[0] = vT.x;
    fInput[1] = vT.y;
    fInput[2] = vT.z;

    ImGui::SameLine();

    ImGui::InputFloat3("##vSpawnScaleMax", fInput);
    vT.x = fInput[0];
    vT.y = fInput[1];
    vT.z = fInput[2];

    data.vSpawnScaleMax = vT;

    ImGui::Text("Max Velocity Scale");

    vT = data.vMaxVelocityScale;
    fInput[0] = vT.x;
    fInput[1] = vT.y;
    fInput[2] = vT.z;

    ImGui::SameLine();

    ImGui::InputFloat3("##vMaxVelocityScale", fInput);
    vT.x = fInput[0];
    vT.y = fInput[1];
    vT.z = fInput[2];

    data.vMaxVelocityScale = vT;

    ImGui::Text("Speed");
    inputInt = data.Speed;
    ImGui::SameLine();
    ImGui::InputInt("##Speed", &inputInt);
    data.Speed = inputInt;

    ImGui::Text("vMaxSpeed");
    inputInt = data.vMaxSpeed;
    ImGui::SameLine();
    ImGui::InputInt("##vMaxSpeed", &inputInt);
    data.vMaxSpeed = inputInt;

    ImGui::Text("StartScale");
    inputInt = data.StartScale;
    ImGui::SameLine();
    ImGui::InputInt("##StartScale", &inputInt);
    data.StartScale = inputInt;

    ImGui::Text("EndScale");
    inputInt = data.EndScale;
    ImGui::SameLine();
    ImGui::InputInt("##EndScale", &inputInt);
    data.EndScale = inputInt;


    ImGui::Text("VelocityScale");
    ImGui::SameLine();
    str = data.VelocityScale ? "true" : "false";
    str += "##VelocityScale";
    if(ImGui::Button(str .c_str()))
    {
        data.VelocityScale = !data.VelocityScale;
    };

    ImGui::Text("VelocityAlignment");
    ImGui::SameLine();
    str = data.VelocityAlignment ? "true" : "false";
    str += "##VelocityAlignment";
    if (ImGui::Button(str.c_str()))
    {
        data.VelocityAlignment = !data.VelocityAlignment;
    };

    ImGui::Text("StartDrag");
    inputInt = data.StartDrag;
    ImGui::SameLine();
    ImGui::InputInt("##StartDrag", &inputInt);
    data.StartDrag = inputInt;

    ImGui::Text("EndDrag");
    inputInt = data.EndDrag;
    ImGui::SameLine();
    ImGui::InputInt("##EndDrag", &inputInt);
    data.EndDrag = inputInt;

    GetTarget()->ParticleSystem()->SetModuleData(data);

    return TRUE;
}