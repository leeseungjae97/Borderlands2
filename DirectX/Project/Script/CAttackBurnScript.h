#pragma once
#include <Engine\CScript.h>
class CAttackBurnScript :
    public CScript
{
public:
    void BeginOverlap(CCollider3D* _Other) override;
    
public:
    CLONE(CAttackBurnScript);
public:
    CAttackBurnScript();
    virtual ~CAttackBurnScript();
};

