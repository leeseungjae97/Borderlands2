#pragma once
#include <Engine\CScript.h>
class CMoveScript :
    public CScript
{
private:
    float fMoveAcc;
    float fMoveTime;
    float fSpeed;

    int iRandSign;
public:
    virtual void tick() override;
    virtual void finaltick() override;
    virtual void begin() override;

public:
    void Rotate();

public:
    CLONE(CMoveScript)
public:
    CMoveScript();
    virtual ~CMoveScript();
};

