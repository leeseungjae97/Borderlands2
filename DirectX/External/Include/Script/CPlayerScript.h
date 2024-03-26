#pragma once
#include <Engine\CScript.h>
#include <Engine\PlayerMgr.h>

class CPlayerScript :
    public CScript
{
private:
    float       fSpeed;
    float       fJump;
    float       fRateOfFire;
    float       fRateOfFireAcc;
    float       fMouseAcces;

    int         iPlayerHp;
    int         iPlayerHpCapa;
    int         iAmmo;
    int         iAmmoCapa;
    int         iAmmoRemain;
    int         iExp;
    int         iExpMax;
    int         iLevel;

    CGameObject* pHeadCollider;

    PlayerMgr::PLAYER_STATE tState;

    bool        bBurn;

    float       fBurnTickAcc;
    float       fBurnAcc;
    float       fBurnTime;

    bool        bHitEnemy;
    bool        bJump;

    CGameObject* m_pUI_HP;
    CGameObject* m_pUI_HP_BACK;
    CGameObject* m_pUI_AMMO;
    CGameObject* m_pUI_AMMO_Back;
    CGameObject* m_pUI_EnemyHp;
    CGameObject* m_pUI_EnemyHp_Back;
    CGameObject* m_pUI_AmmoIcon;
    CGameObject* m_pUI_EXP;
    CGameObject* m_pUI_ExpBar_Back;
    CGameObject* m_pUI_CrossHair;

    CGameObject* m_pUI_HPBackHit;
    CGameObject* m_pUI_HPFrontHit;
    CGameObject* m_pUI_HPLeftHit;
    CGameObject* m_pUI_HPRightHit;
    CGameObject* m_pUI_FireHitEffect;

    CGameObject* m_pUI_Scope_Left;
    CGameObject* m_pUI_Scope_Right;
    CGameObject* m_pUI_Scope_Aim;

    float fHpFrontAcc;
    float fHpBackAcc;
    float fHpLeftAcc;
    float fHpRightAcc;

    CGameObject* m_pUI_ShieldBackHit;
    CGameObject* m_pUI_ShieldFrontHit;
    CGameObject* m_pUI_ShieldLeftHit;
    CGameObject* m_pUI_ShieldRightHit;

    class CUI*        m_pAmmoText;
    class CUI*        m_pHPText;
    class CUI*        m_pLevelText;
    class CUI*        m_pEnemyName;
    class CUI*        m_pEnemyLevel;


    wstring wsPlayerWalkSound[5] =
    {
        L"sound\\walk\\player_walk_1.ogg",
        L"sound\\walk\\player_walk_2.ogg",
        L"sound\\walk\\player_walk_3.ogg",
        L"sound\\walk\\player_walk_4.ogg",
        L"sound\\walk\\player_walk_5.ogg",
    };

    int playerWalkIdx;
    float fWalkSoundAcc;
public:
    virtual void begin() override;
    virtual void tick() override;
    virtual void finaltick() override;

private:
    void CreateUI();
    void Look();
    void ShootBullet();
    void ShootMissile();
    void Reload();
    void Draw(int _Idx);
    void Movement();
    void Recoil();
    virtual void CatchRaycast() override;
    void Burn();

    
    void makeCollider();

public:
    void Attacked(int _Damage);
    bool IsBurn() { return bBurn; }
    void SetBurn(bool _Burn) { bBurn = _Burn; }
    void SetBurnTime(float _Time) { fBurnTime = _Time; }
    void AddExp(int _iExp);

public:
    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _FILE) override;

    void BeginOverlap(CCollider3D* _Other) override;
    void OnOverlap(CCollider3D* _Other) override;
    void EndOverlap(CCollider3D* _Other) override;
    void Raycast(tRayInfo _RaycastInfo) override;

    void CustomReloadSound();

    CLONE(CPlayerScript);
public:
    CPlayerScript();
    ~CPlayerScript();
};

