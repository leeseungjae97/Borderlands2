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

    PlayerMgr::PLAYER_STATE tState;


    bool        bReloading;
    bool        bFire;
    bool        bSprint;

    float mt;
public:
    virtual void begin() override;
    virtual void tick() override;
    virtual void finaltick() override;

private:
    void Look();
    void ShootBullet();
    void ShootMissile();
    void Reload();
    void Draw(int _Idx);
    void Movement();

    void Attacked(float _Damage);
    virtual void CatchRaycast() override;

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

