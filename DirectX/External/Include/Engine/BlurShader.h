#pragma once
#include "CTexture.h"
#include "ptr.h"
#include "CComputeShader.h"

class BlurShader :
    public CComputeShader
{
private:
    Ptr<CTexture> m_pInput;
    Ptr<CTexture> m_pOutput;

public:
    virtual void Clear() override;
    virtual void UpdateData() override;
    void SetOutput(Ptr<CTexture> ptr) { m_pOutput = ptr; }
    void SetInput(Ptr<CTexture> ptr) { m_pInput = ptr; }

public:
    BlurShader(UINT _iGroupPerThreadX, UINT _iGroupPerThreadY, UINT _iGroupPerThreadZ);
    virtual ~BlurShader();
};

