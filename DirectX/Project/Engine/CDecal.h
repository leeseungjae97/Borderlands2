#pragma once
#include "CRenderComponent.h"
class CDecal :
    public CRenderComponent
{
private:
    Ptr<CTexture>   m_DecalTex;

    bool            m_bDeferred;
    bool            m_bEmissive;

    int            m_bSwitch;

    float           m_fAcc;

public:
    void render() override;
    void finaltick() override;

    void SetDeferredDecal(bool _bDeferred);
    void SetEmissiveDecal(bool _bActivate) { m_bEmissive = _bActivate; }

    CLONE(CDecal);
public:
    CDecal();
    ~CDecal();
};

