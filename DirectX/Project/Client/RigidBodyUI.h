#pragma once
#include "ComponentUI.h"
class RigidBodyUI :
    public ComponentUI
{
public:
    int render_update() override;

public:
    RigidBodyUI();
    virtual ~RigidBodyUI();
};

