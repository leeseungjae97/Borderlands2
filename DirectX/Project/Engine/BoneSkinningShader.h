#pragma once
#include "CComputeShader.h"
class BoneSkinningShader :
    public CComputeShader
{
public:
    void SetBoneIndices();

public:
    BoneSkinningShader(UINT _iGroupPerThreadX, UINT _iGroupPerThreadY, UINT _iGroupPerThreadZ);
    virtual ~BoneSkinningShader();
};

