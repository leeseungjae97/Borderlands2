#pragma once
#include "CComputeShader.h"
#include "CStructuredBuffer.h"

class DownScaleSecondShader :
    public CComputeShader
{
public:
    //ComPtr<ID3D11Buffer> AverageLum;
    //ComPtr<ID3D11Buffer> PrevAverageLum;
    //ComPtr<ID3D11Buffer> AverageValue;

    //int iCount;

public:
    //void SetAverageLum(ID3D11Buffer* _buffer) { AverageLum = _buffer; }
    //void SetPrevAverageLum(ID3D11Buffer* _buffer) { PrevAverageLum = _buffer; }
    //void SetAverageValue(ID3D11Buffer* _buffer) { AverageValue = _buffer; }

    void SetGroupSize(float _size) { m_Const.arrFloat[0] = _size; }
    //void SetAdaption(float _adaption) { m_Const.arrFloat[1] = _adaption; }

    //void SetCount(int _Count) { iCount = _Count; }
public:
    void UpdateData() override;
    void Clear() override;

public:
    DownScaleSecondShader(UINT _iGroupPerThreadX, UINT _iGroupPerThreadY, UINT _iGroupPerThreadZ);
    virtual ~DownScaleSecondShader();
};

