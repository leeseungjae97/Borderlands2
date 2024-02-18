#pragma once
#include "CComputeShader.h"
class HDRShader :
    public CComputeShader
{

public:
	virtual void UpdateData();
	virtual void Clear();

public:
	HDRShader(UINT _iGroupPerThreadX, UINT _iGroupPerThreadY, UINT _iGroupPerThreadZ);
	virtual ~HDRShader();
};

