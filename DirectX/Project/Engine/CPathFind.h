#pragma once
#include "CComponent.h"
class CPathFind :
    public CComponent
{
private:


public:
    void finaltick() override;

    void LoadFromLevelFile(FILE* _FILE) override;
    void SaveToLevelFile(FILE* _File) override;

    CLONE(CPathFind);
public:
    CPathFind();
    ~CPathFind();
};

