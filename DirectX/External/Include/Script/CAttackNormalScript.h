#pragma once
#include <Engine\CScript.h>
class CAttackNormalScript :
    public CScript
{
private:
    bool bAttack;
    bool bManualAttack;
    bool bManual;
public:
    void tick() override;
    void BeginOverlap(CCollider3D* _Other) override;
    void OnOverlap(CCollider3D* _Other) override;
    void EndOverlap(CCollider3D* _Other) override;

    void ManualAttack(bool _Attack) { bManualAttack = _Attack; }
    void SetManual(bool _Manual) { bManual = _Manual; }
public:
    CLONE(CAttackNormalScript);
public:
    CAttackNormalScript();
    virtual ~CAttackNormalScript();
};

