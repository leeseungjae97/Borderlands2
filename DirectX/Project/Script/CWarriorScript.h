#pragma once
#include <Engine\CScript.h>
class CWarriorScript :
    public CScript
{
public:
    enum class WARRIOR_STATE {
        ENTER,
        IDLE,
        DIE,
        FAR_BREATH,
        MID_BREATH,
        NEAR_BREATH,
        TAIL_BEAM_ENTER,
        TAIL_BEAM_IDLE,
        TAIL_BEAM_EXIT,
        TAIL_ATTACK,
        ROCK_THROW,
        HIDE,
        APPEAR,
    };
private:
    WARRIOR_STATE tState;
    CGameObject* pBreath;
    CGameObject* pTailBeam;
    CGameObject* pRock;

    CGameObject* pBreathCollider;
    CGameObject* pTailBeamCollider;
    CGameObject* pTailCollider;
    CGameObject* pAttackBoundCollider;

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

    CGameObject* pUI_WarriorHP;
    CGameObject* pUI_WarriorHPBack;
    class CUI*  pWarriorText;

    float       m_fActAcc;
    int         m_RandBehavior;

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
    void paperBurn();
    void IsDie();

    void createUI();
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

