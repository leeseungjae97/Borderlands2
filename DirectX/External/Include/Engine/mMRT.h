#pragma once

#include "ptr.h"
#include "CEntity.h"

class CTexture;

class MRT :
    public CEntity
{
private:
    Ptr<CTexture>   m_arrRT[8];
    UINT            m_RTCount;
    Ptr<CTexture>   m_DSTex;

    Vec4            m_ClearColor[8];

    D3D11_VIEWPORT  m_Viewport;

    bool            m_bRenderTarget;

public:
    void Create(Ptr<CTexture>* _arrRTTex, UINT _RTCount, Ptr<CTexture> _DSTex);
    void SetClearColor(Vec4 _ClearColor, UINT _RTIdx) { m_ClearColor[_RTIdx] = _ClearColor; }
    Ptr<CTexture> GetTexture(int _Idx) { return m_arrRT[_Idx]; }
    void ClearTarget();
    void OMSet(bool _bStay = false);

    void SetRenderTarget(bool _RenderTarget) { m_bRenderTarget = _RenderTarget; }
    const D3D11_VIEWPORT& GetViewPort() { return m_Viewport; }

public:
    CLONE_DISABLE(MRT)

public:
    MRT();
    ~MRT();
};

