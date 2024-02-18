#pragma once

#include "CSingleton.h"

class HDRBuffer
	: public CSingleton<HDRBuffer>
{
public:
    // buffer 1
    ComPtr<ID3D11Buffer>                m_SB1;
    // buffer 2
    ComPtr<ID3D11Buffer>                m_SB2;
    ComPtr<ID3D11Buffer>                m_UB1;
    ComPtr<ID3D11Buffer>                m_UB2;

    // Input
    ComPtr<ID3D11ShaderResourceView>    m_InputSRV1;
    ComPtr<ID3D11ShaderResourceView>    m_InputSRV2;

    // output
    ComPtr<ID3D11UnorderedAccessView>   m_OuputUAV;

public:

public:
    void init(int width, int height);
    void UpdateData();

public:
	HDRBuffer();
	virtual ~HDRBuffer();
};

