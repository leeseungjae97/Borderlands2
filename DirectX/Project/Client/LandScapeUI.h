#pragma once
#include "ComponentUI.h"

class LandScapeUI :
    public ComponentUI
{
public:
    virtual int render_update() override;

public:
    void SelectBrushTexture(DWORD_PTR _Key);
    void SelectHeightMap(DWORD_PTR _Key);
    void SelectColorMap(DWORD_PTR _Key);

public:
    LandScapeUI();
    ~LandScapeUI();
};

