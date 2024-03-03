#pragma once
#include "UI.h"
class ObjectPickerUI :
    public UI
{
private:
    CGameObject* m_pObj;
public:
    virtual void init() override;
    virtual void tick() override;
    virtual int render_update() override;

public:
    ObjectPickerUI();
    ~ObjectPickerUI();
};

