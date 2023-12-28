#pragma once
#include "CRenderComponent.h"
class CSolarSystem :
    public CRenderComponent
{
private:
    bool        m_bEmissive;
    int         m_iAlphaCutCoeff;

    Ptr<CTexture> m_PlanetTexture;
    Ptr<CTexture> m_PlanetTexture2;
    Ptr<CTexture> m_PlanetNormalTexture;
    Ptr<CTexture> m_PlanetCoverTexture;

    CGameObject* pCoverImage;
public:
    void SetEmissive(bool _bEmissive) { m_bEmissive = _bEmissive; }
    void SetPlanetTex(const Ptr<CTexture>& _Tex) { m_PlanetTexture = _Tex; }
    void SetPlanetTex2(const Ptr<CTexture>& _Tex){ m_PlanetTexture2 = _Tex;}
    void SetPlanetNormalTex(const Ptr<CTexture>& _Tex){ m_PlanetNormalTexture = _Tex;}
    void SetPlanetCoverTex(const Ptr<CTexture>& _Tex) { m_PlanetCoverTexture = _Tex;}
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

