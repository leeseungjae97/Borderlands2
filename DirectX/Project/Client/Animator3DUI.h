#pragma once
#include "ComponentUI.h"
class Animator3DUI :
    public ComponentUI
{
private:
    class CGameObject* m_pAnimOwner;
public:
    virtual int render_update() override;

public:
    Animator3DUI();
    ~Animator3DUI();
};

