#pragma once
#include "CComputeShader.h"
#include "ptr.h"
class CTexture;
class DownSamplingShader :
    public CComputeShader
{
private:
    Ptr<CTexture> m_pInput;
    Ptr<CTexture> m_pOutput;

public:
    void SetThreshold(float _Threshold) { m_Const.arrFloat[0] = _Threshold; }
    void SetInput(Ptr<CTexture> tex) { m_pInput = tex; }
    void SetOutput(Ptr<CTexture> tex) { m_pOutput = tex; }

public:
    virtual void Clear() override;
    virtual void UpdateData() override;
public:
    DownSamplingShader(UINT _iGroupPerThreadX, UINT _iGroupPerThreadY, UINT _iGroupPerThreadZ);
    virtual ~DownSamplingShader();
};

