#pragma once
#include "CComputeShader.h"
class BlurHorizontalShader :
    public CComputeShader
{
public:
    void Clear() override;
    void UpdateData() override;

public:
    BlurHorizontalShader(UINT _iGroupPerThreadX, UINT _iGroupPerThreadY, UINT _iGroupPerThreadZ);
    virtual ~BlurHorizontalShader();
};

