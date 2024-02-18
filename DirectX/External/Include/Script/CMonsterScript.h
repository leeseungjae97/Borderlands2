#pragma once
#include <Engine\CScript.h>
#include <Engine\MonsterMgr.h>

class CMonsterScript :
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

    MonsterMgr::MONSTER_STATE tState;
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
public:
    void SetRotate(bool _Rotate) { bRotate = _Rotate; }
    void SetRotateSpeed(float _Speed) { fRotateSpeed = _Speed; }
    void SetSpeed(float _Speed) { fMonsterSpeed = _Speed; }
    void SetMonsterHp(int _Hp) { iMonsterHp = _Hp; }
    void SetRateOfFire(float _ROF) { fRateOfFire = _ROF; }

    CLONE(CMonsterScript);
public:
    CMonsterScript();
    ~CMonsterScript();
};


