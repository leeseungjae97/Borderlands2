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

    void SetDecalTex(Ptr<CTexture> _Tex);
    Ptr<CTexture> GetDecalTex() { return m_DecalTex; }
    void SetDeferredDecal(bool _bDeferred);
    void SetEmissiveDecal(bool _bActivate) { m_bEmissive = _bActivate; }

    void LoadFromLevelFile(FILE* _FILE) override;
    void SaveToLevelFile(FILE* _File) override;

    CLONE(CDecal);
public:
    CDecal();
    ~CDecal();
};

