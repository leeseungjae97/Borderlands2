#pragma once
#include "UI.h"
class RenderUI :
    public UI {
public:

public:
    virtual void init() override;
    virtual void tick() override;
    virtual int render_update() override;

public:
    RenderUI();
    ~RenderUI();
};

