#pragma once
#include "UI.h"
class SamplerFilterUI :
    public UI
{
public:

public:
    virtual void init() override;
    virtual void tick() override;
    virtual int render_update() override;

public:
    SamplerFilterUI();
    ~SamplerFilterUI();
};


