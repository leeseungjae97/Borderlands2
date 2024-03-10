#pragma once
#include <Engine\CScript.h>
class CMeleeScript :
    public CScript
{
public:
    void finaltick() override;
    void tick() override;

    void LoadFromLevelFile(FILE* _FILE) override;
    void SaveToLevelFile(FILE* _File) override;

public:
    CLONE(CMeleeScript);
public:
    CMeleeScript();
    virtual ~CMeleeScript();
};

