#pragma once
#include <Engine\CScript.h>
class CWeaponScript :
    public CScript
{
private:
    bool    m_bEquiWeapon;
    bool    m_bStopFollow;
    Vec4    m_vRot;
    Vec4    m_vSign;

    Vec3    m_vPrevRot;

public:
    void tick() override;
	void finaltick() override;
public:
    void SetWeaponPos();
    void SetEquiWeapon(bool _Equi) { m_bEquiWeapon = _Equi; }
public:
    CLONE(CWeaponScript);

public:
    CWeaponScript();
    virtual ~CWeaponScript();
};

