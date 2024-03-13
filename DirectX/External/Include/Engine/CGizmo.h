#pragma once
#include "CComponent.h"
class CGizmo :
    public CComponent
{
private:
    bool         m_bUsePhysxPos;

public:
    void finaltick() override;

    void LoadFromLevelFile(FILE* _FILE) override;
    void SaveToLevelFile(FILE* _File) override;

    CLONE(CGizmo)
public:
    CGizmo(bool _UsePhysxPos = false);
    virtual ~CGizmo();
};

