#pragma once
#include "CComponent.h"
class CGizmo :
    public CComponent
{
private:
    CGameObject* m_pUp;
    CGameObject* m_pRight;
    CGameObject* m_pFront;

public:
    void finaltick() override;

    void LoadFromLevelFile(FILE* _FILE) override {};
    void SaveToLevelFile(FILE* _File) override {};

    CLONE(CGizmo)
public:
    CGizmo();
    virtual ~CGizmo();
};

