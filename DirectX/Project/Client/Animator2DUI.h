#pragma once
#include "ComponentUI.h"
class Animator2DUI :
    public ComponentUI {
private:
    class CGameObject* m_pAnimOwner;
public:
    virtual int render_update() override;

public:
    Animator2DUI();
    ~Animator2DUI();
};

