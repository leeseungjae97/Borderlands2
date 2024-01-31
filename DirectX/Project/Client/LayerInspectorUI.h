#pragma once
#include "UI.h"
class LayerInspectorUI :
    public UI
{
public:

public:
    virtual void init() override;
    virtual void tick() override;
    virtual int render_update() override;

public:
    LayerInspectorUI();
    ~LayerInspectorUI();
};

