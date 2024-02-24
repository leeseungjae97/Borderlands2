#pragma once
#include <Engine\CScript.h>
#include <Engine\WarriorMgr.h>
class CWarriorScript :
    public CScript
{
private:
    WarriorMgr::WARRIOR_STATE tState;
    CGameObject* pBreath;
    CGameObject* pTailBeam;
    CGameObject* pRock;

    CGameObject* pBreathCollisionPoint;
    CGameObject* pTailBeamCollisionPoint;
    CGameObject* pTailCollisionPoint;

    CGameObject* pHeadCollider;
    CGameObject* pMouseCollider;
    CGameObject* pStomachCollider;
    CGameObject* pChestCollider;

    Vec4        m_vRot;
    Vec4        m_R;

    bool        m_bRockThrow;
    bool        m_bRockFollow;

    int         iWarriorHp;
    int         iWarriorHpCapa;

public:
    void DoFarBreath();
    void DoMidBreath();
    void DoNearBreath();
    void DoTailBeam();
    void DoThrowRock();
    void DoTailAttack();

    void makeCollider();
private:
    void initAnim();
    void breathMove();
    void beamMove();
    void rockMove();
    void tailMove();

    void colliderMove();
    void makeAttackObject();
    void collision();

public:
    void begin() override;
    void tick() override;
    
    void finaltick() override;
    void Attacked(int _Damage);

    void Raycast(tRayInfo _RaycastInfo) override;
    void LoadFromLevelFile(FILE* _FILE) override;
    void SaveToLevelFile(FILE* _File) override;

    int GetHp() { return iWarriorHp; }
    int GetHpCapacity() { return iWarriorHpCapa; }

    float GetHpRatio() { return (float)iWarriorHp / (float)iWarriorHpCapa; }
public:
    CLONE(CWarriorScript);
public:
    CWarriorScript();
    virtual ~CWarriorScript();
};

