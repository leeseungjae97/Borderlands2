#pragma once
#include <Engine\CScript.h>

class CMonsterScript :
    public CScript
{
private:
    float fMonsterSpeed;

public:
    void Move();
    bool Rotate();

public:
    virtual void tick() override;

    CLONE(CMonsterScript);
public:
    CMonsterScript();
    ~CMonsterScript();
};


