#pragma once
#include <Engine\CScript.h>

class CMonsterScript :
    public CScript
{
public:
    void Move();

public:
    virtual void tick() override;

    CLONE(CMonsterScript);
public:
    CMonsterScript();
    ~CMonsterScript();
};


