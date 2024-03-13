#pragma once
#include "ComponentUI.h"
class ParticleSystemUI :
    public ComponentUI
{

public:
    virtual int render_update() override;

    void SelectMesh(DWORD_PTR _Key);
    void SelectMaterial(DWORD_PTR _Key);

    void SelectTexture(DWORD_PTR _Key);
    void SelectShader(DWORD_PTR _Key);

public:
    ParticleSystemUI();
    ~ParticleSystemUI();
};

