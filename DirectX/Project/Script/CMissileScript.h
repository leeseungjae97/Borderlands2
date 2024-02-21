#pragma once
#include <Engine\CScript.h>

class CMissileScript :
    public CScript
{
private:
public:
    virtual void begin() override;
    virtual void tick() override;

    CLONE(CMissileScript);
public:
    CMissileScript();
    ~CMissileScript();
};

