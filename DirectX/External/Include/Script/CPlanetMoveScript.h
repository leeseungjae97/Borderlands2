#pragma once
#include "E:\directx11_3d\DirectX\External\Include\Engine\CScript.h"
class CPlanetMoveScript :
    public CScript
{
private:
    bool    m_bRotating;
    bool    m_bOrbit;
public:
    virtual void tick() override;

public:
    CLONE(CPlanetMoveScript);

public:
    CPlanetMoveScript();
    ~CPlanetMoveScript();
};

