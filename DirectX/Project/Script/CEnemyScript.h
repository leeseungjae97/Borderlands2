#pragma once
#include <Engine\CScript.h>
#include <Engine\EnemyMgr.h>

class CEnemyScript :
    public CScript
{
private:
    float fMonsterSpeed;
    float fRotateSpeed;
    float fRateOfFire;
    float fRateOfFireAcc;
    int   iMonsterHp;
    int   iAmmo;
    bool  bRotate;

    EnemyMgr::ENEMY_STATE tState;
public:
    void Move();
    bool Rotate();
    void Reload();

public:
    void begin() override;

    virtual void tick() override;
    void BeginOverlap(CCollider3D* _Other) override;
    void Raycast(tRayInfo _RaycastInfo) override;
    void Attacked(float fDamage);

    virtual void CatchRaycast() override;
    void Shoot();
    void Look();
    bool IsDetect();
public:
    void SetRotate(bool _Rotate) { bRotate = _Rotate; }
    void SetRotateSpeed(float _Speed) { fRotateSpeed = _Speed; }
    void SetSpeed(float _Speed) { fMonsterSpeed = _Speed; }
    void SetMonsterHp(int _Hp) { iMonsterHp = _Hp; }
    void SetRateOfFire(float _ROF) { fRateOfFire = _ROF; }

    CLONE(CEnemyScript);
public:
    CEnemyScript();
    ~CEnemyScript();
};


