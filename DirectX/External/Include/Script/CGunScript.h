#pragma once
#include <Engine\CScript.h>
class CGunScript :
    public CScript
{
private:
    bool    m_bEquiWeapon;

public:
    void tick() override;
	void finaltick() override;
public:
    void SetWeaponPos();
    void SetEquiWeapon(bool _Equi) { m_bEquiWeapon = _Equi; }
public:
    CLONE(CGunScript);

public:
    CGunScript();
    virtual ~CGunScript();
};

