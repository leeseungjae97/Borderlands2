#pragma once
#include "UI.h"
class PlacementUI :
    public UI
{
private:
    bool m_bFollow;
public:
    virtual void init() override;
    virtual void tick() override;
    virtual int render_update() override;

public:
    PlacementUI();
    ~PlacementUI();
};

