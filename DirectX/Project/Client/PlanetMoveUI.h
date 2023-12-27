#pragma once
#include "ComponentUI.h"
class PlanetMoveUI :
    public ComponentUI
{
public:
    virtual int render_update() override;

public:
    PlanetMoveUI();
    ~PlanetMoveUI();
};

