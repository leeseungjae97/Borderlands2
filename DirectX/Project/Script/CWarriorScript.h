#pragma once
#include <Engine\CScript.h>
#include <Engine\WarriorMgr.h>
class CWarriorScript :
    public CScript
{
private:
    WarriorMgr::WARRIOR_STATE tState;
    CGameObject* pBreath;
    Vec4        m_vRot;
    Vec4        m_R;

public:
    void DoBreath();
    void DoTailBeam();
    void DoThrowRock();
    void DoTailAttack();

private:
    void makeBreath();

public:
    void begin() override;
    void tick() override;
    void finaltick() override;
    void Raycast(tRayInfo _RaycastInfo) override;
    void LoadFromLevelFile(FILE* _FILE) override;
    void SaveToLevelFile(FILE* _File) override;

public:
    CLONE(CWarriorScript);
public:
    CWarriorScript();
    virtual ~CWarriorScript();
};

