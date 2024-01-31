#pragma once
#include <Engine\CScript.h>

class CPlayerScript :
    public CScript
{
private:
    float       m_fSpeed;
    float       m_fJump;
    float       m_fRateOfFire;
    float       m_fRateOfFireAcc;
    float       m_MouseAcces;

public:
    virtual void begin() override;
    virtual void tick() override;

private:
    void Shoot();

public:
    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _FILE) override;

    void BeginOverlap(CCollider3D* _Other) override;

    CLONE(CPlayerScript);
public:
    CPlayerScript();
    ~CPlayerScript();
};

