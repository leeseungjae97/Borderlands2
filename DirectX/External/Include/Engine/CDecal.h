#pragma once
#include "CRenderComponent.h"
class CDecal :
    public CRenderComponent
{
private:
    Ptr<CTexture>   m_DecalTex;

    bool            m_bDeferred;
    bool            m_bEmissive;

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

