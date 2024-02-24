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
    int         iAmmo;
    int         iAmmoCapa;

    CGameObject* pHeadCollider;

    PlayerMgr::PLAYER_STATE tState;

    bool        bBurn;

    float       fBurnTickAcc;
    float       fBurnAcc;
    float       fBurnTime;

    CGameObject* m_pUI_HP;
    CGameObject* m_pUI_AMMO;
    CGameObject* m_pUI_EnemyHp;
    CGameObject* m_pUI_EnemyHp_Back;

    class CUI*        m_pAmmoText;
    class CUI*        m_pHPText;

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
    virtual void CatchRaycast() override;
    void Burn();

    void makeCollider();

public:
    void Attacked(int _Damage);
    bool IsBurn() { return bBurn; }
    void SetBurn(bool _Burn) { bBurn = _Burn; }
    void SetBurnTime(float _Time) { fBurnTime = _Time; }
public:
    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _FILE) override;

    void BeginOverlap(CCollider3D* _Other) override;
    void Raycast(tRayInfo _RaycastInfo) override;

    CLONE(CPlayerScript);
public:
    CPlayerScript();
    ~CPlayerScript();
};

