#pragma once
#include "CRenderComponent.h"
class CSolarSystem :
    public CRenderComponent
{
private:
    bool        m_bRevolution;
    bool        m_bRotating;
    bool        m_bEmissive;

    Ptr<CTexture> m_PlanetTexture;
public:
    void SetEmissive(bool _bEmissive) { m_bEmissive = _bEmissive; }
    void SetPlanetTex(const Ptr<CTexture>& _Tex);
public:
    virtual void finaltick() override;
    void render();

    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;

    CLONE(CSolarSystem)

public:
    CSolarSystem();
    ~CSolarSystem();
};

