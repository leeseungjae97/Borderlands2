#pragma once
#include <Engine\CScript.h>
#include <Engine\EnemyMgr.h>

class CEnemyScript :
    public CScript
{
private:
    float fEnemySpeed;
    float fRotateSpeed;
    float fRateOfFire;
    float fRateOfFireAcc;
    int   iEnemyHp;
    int   iEnemyHpCapa;
    int   iAmmo;
    bool  bRotate;

    CGameObject* pHeadCollider;

    EnemyMgr::ENEMY_STATE tState;

    vector<Vec3> m_vecQueryPath;

    Vec3 vDestPos;
    Vec3 vPrevPos;
    float fDestDist;
private:
    void Move();
    bool Rotate();
    bool RotateDest();
    void Reload();

    void makeCollider();
    void moveCollider();
    void doPathQuery();
public:
    void begin() override;

    virtual void tick() override;
    void finaltick() override;
    void BeginOverlap(CCollider3D* _Other) override;
    void Raycast(tRayInfo _RaycastInfo) override;
    void Attacked(float fDamage);

    virtual void CatchRaycast() override;
    void Shoot();
    void Look();
    bool IsDetect();

    void PaperBurn();
    void IsDie();
public:
    void SetRotate(bool _Rotate) { bRotate = _Rotate; }
    void SetRotateSpeed(float _Speed) { fRotateSpeed = _Speed; }
    void SetSpeed(float _Speed) { fEnemySpeed = _Speed; }
    void SetMonsterHp(int _Hp) { iEnemyHp = _Hp; }
    void SetRateOfFire(float _ROF) { fRateOfFire = _ROF; }

    int GetHp() { return iEnemyHp; }
    int GetHpCapacity() { return iEnemyHpCapa; }

    float GetHpRatio() { return (float)iEnemyHp / (float)iEnemyHpCapa; }

    CLONE(CEnemyScript);
public:
    CEnemyScript();
    ~CEnemyScript();
};


