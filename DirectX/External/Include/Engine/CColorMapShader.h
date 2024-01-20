#pragma once
#include "CComputeShader.h"
#include "CTexture.h"
#include "ptr.h"

class CStructuredBuffer;

class CColorMapShader :
    public CComputeShader
{
private:
    Ptr<CTexture>       m_pColorMap;
    Ptr<CTexture>       m_pBrushTex;
    Vec2                m_vScale;
    CStructuredBuffer*  m_pInput;

public:
    void SetColorMap(Ptr<CTexture> _Tex) { m_pColorMap = _Tex; }
    void SetBrushTex(Ptr<CTexture> _Tex) { m_pBrushTex = _Tex; }
    void SetBrushScale(Vec2 _Scale) { m_vScale = _Scale; }
    void SetInputBuffer(CStructuredBuffer* _Buffer) { m_pInput = _Buffer; }

    void UpdateData() override;
    void Clear() override;

public:
    CColorMapShader(UINT _iGroupPerThreadX, UINT _iGroupPerThreadY, UINT _iGroupPerThreadZ);
    ~CColorMapShader();
};

