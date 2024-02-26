#pragma once
#include "CComputeShader.h"
#include "CStructuredBuffer.h"
#include "ptr.h"
class CTexture;
class DownScaleFirstShader :
    public CComputeShader
{
private:
    Ptr<CTexture> g_Tex;
    ID3D11Buffer* AverageLum;
    Ptr<CTexture> HDRDownScale;

    UINT          iGroupCount;
public:
    void SetScaleTex(Ptr<CTexture> _Tex) { g_Tex = _Tex; }
    void SetHDRDownScale(Ptr<CTexture> _Tex) { HDRDownScale = _Tex; }
    void SetAverageLum(ID3D11Buffer* _buffer) { AverageLum = _buffer; }

    void SetLumFactor(float _LumFactor) { m_Const.arrFloat[0] = _LumFactor; }
    void SetResol(Vec2 _Resol) { m_Const.arrV2[0] = _Resol; }
    void SetSize(Vec2 _Size) { m_Const.arrV2[1] = _Size; }

    void SetGroupSize(UINT _Size) { iGroupCount = _Size; }

public:
    virtual void Clear() override;
    virtual void UpdateData() override;

public:
    DownScaleFirstShader(UINT _iGroupPerThreadX, UINT _iGroupPerThreadY, UINT _iGroupPerThreadZ);
    virtual ~DownScaleFirstShader();
};

