#pragma once
#include <Engine\CScript.h>
class CAttackNormalScript :
    public CScript
{
private:
    bool bBegin;
public:
    void tick() override;
    void BeginOverlap(CCollider3D* _Other) override;
    void OnOverlap(CCollider3D* _Other) override;
    void EndOverlap(CCollider3D* _Other) override;
public:
    CLONE(CAttackNormalScript);
public:
    CAttackNormalScript();
    virtual ~CAttackNormalScript();
};

