#pragma once
#include "CComputeShader.h"
#include "CStructuredBuffer.h"
#include "CTexture.h"
#include "ptr.h"

class DownScaleShader :
    public CComputeShader
{
private:
    Ptr<CTexture> m_pOriginTexBuffer;
    Ptr<CTexture> m_pDownScaleTexBuffer;

public:
    void SetDownScale(Vec2 _vScale) { m_Const.arrV2[0] = _vScale; }
    void SetOriginTex(Ptr<CTexture> _tex) { m_pOriginTexBuffer = _tex; }
    void SetDownScaleTex(Ptr<CTexture> _tex) { m_pDownScaleTexBuffer = _tex; }

public:
    virtual void UpdateData() override;
    virtual void Clear() override;

public:
    DownScaleShader(UINT _iGroupPerThreadX, UINT _iGroupPerThreadY, UINT _iGroupPerThreadZ);
    virtual ~DownScaleShader();
};

