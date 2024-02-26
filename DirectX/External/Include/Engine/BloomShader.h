#pragma once
#include "CComputeShader.h"
#include "CTexture.h"
#include "ptr.h"

class BloomShader :
    public CComputeShader
{
    Ptr<CTexture> pOrigin;
    Ptr<CTexture> pBloomTex;

    UINT        iGroupSize;
public:
    void SetOriginTex(Ptr<CTexture> _Origin) { pOrigin = _Origin; }
    void SetBloomTex(Ptr<CTexture> _Bloom) { pBloomTex = _Bloom; }
    void SetLumFactor(float _Factor) { m_Const.arrFloat[0] = _Factor; }
    void SetThreshold(float _Threshold) { m_Const.arrFloat[1] = _Threshold; }
    void SetResol(Vec2 _Resol) { m_Const.arrV2[0] = _Resol; }
    void SetGroupSize(UINT _Size) { iGroupSize = _Size; }

public:
    virtual void Clear() override;
    virtual void UpdateData() override;
public:
    BloomShader(UINT _iGroupPerThreadX, UINT _iGroupPerThreadY, UINT _iGroupPerThreadZ);
    virtual ~BloomShader();
};

