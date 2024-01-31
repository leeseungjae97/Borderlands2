#pragma once
#include "UI.h"
class CollisionUI :
    public UI
{
public:
    virtual void init() override;
    virtual void tick() override;
    virtual int render_update() override;

public:
    CollisionUI();
    ~CollisionUI();
};

