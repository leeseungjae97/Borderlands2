#pragma once
#include "CComponent.h"
class CPlanetMove :
    public CComponent
{
private:
    bool        m_bRotate;
    bool        m_bOrbit;
    bool        m_bReverseRotate;
    bool        m_bXRotate;

    bool        m_bSatellite;

    float       m_fOrbitTheta;
    float       m_fOrbitSpeed;
    float       m_fOrbitRadius;

    float       m_fRotateSpeed;

    float       m_f;
    
public:
    void SetRotate(bool _Rotate) { m_bRotate = _Rotate; }
    void SetReverseRotate(bool _Reverse) { m_bReverseRotate = _Reverse; }
    void SetXRotate(bool _XRotate) { m_bXRotate = _XRotate; }
    void SetOrbit(bool _Orbit) { m_bOrbit = _Orbit; }
    void SetSatellite(bool _bSatellite) { m_bSatellite = _bSatellite; }

    void SetOrbitTheta(float _Theta) { m_fOrbitTheta = _Theta; }
    void SetRotateSpeed(float _Speed) { m_fRotateSpeed = _Speed; }
    void SetOrbitSpeed(float _Speed) { m_fOrbitSpeed = _Speed; }
    void SetOrbitRadius(float _Radius) { m_fOrbitRadius = _Radius; }

    float GetOrbitTheta() { return m_fOrbitTheta; }
public:
    void Rotate();
    void Orbit();
public:
    virtual void finaltick() override;

public:
    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _FILE) override;

public:
    CLONE(CPlanetMove);
public:
    CPlanetMove();
    ~CPlanetMove();
};

