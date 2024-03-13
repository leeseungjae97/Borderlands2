#pragma once
#include "ComponentUI.h"
class SkyBoxUI :
    public ComponentUI
{
public:
    virtual int render_update() override;

public:
    void SelectMesh(DWORD_PTR _Key);
    void SelectMaterial(DWORD_PTR _Key);

    void SelectTexture(DWORD_PTR _Key);
    void SelectShader(DWORD_PTR _Key);

public:
    SkyBoxUI();
    virtual ~SkyBoxUI();
};

