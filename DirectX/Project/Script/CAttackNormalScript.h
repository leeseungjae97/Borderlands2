#pragma once
#include <Engine\CScript.h>
class CAttackNormalScript :
    public CScript
{
public:
    void BeginOverlap(CCollider3D* _Other) override;

public:
    CLONE(CAttackNormalScript);
public:
    CAttackNormalScript();
    virtual ~CAttackNormalScript();
};

