#pragma once
#include "ComponentUI.h"

class CAnimClip;
struct tMTBone;

class Animator3DUI :
    public ComponentUI
{
private:
    class CGameObject* m_pAnimOwner;
public:
    virtual int render_update() override;
    virtual void SetTarget(CGameObject* _Target) override;

    const std::unordered_map<wstring, CAnimClip*>* clips;
    CAnimClip* curAnimClip;
    vector<tMTBone> bones;
    float ratio;
public:
    Animator3DUI();
    ~Animator3DUI();
};

