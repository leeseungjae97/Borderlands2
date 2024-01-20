#pragma once
#include "CComputeShader.h"
#include "ptr.h"

class CStructuredBuffer;
class CTexture;

class CRayCastShader :
    public CComputeShader
{
private:
    Ptr<CTexture>       m_pHeightMap;

    UINT                m_iXFace;
    UINT                m_iZFace;

    tRay                m_ray;

    CStructuredBuffer*  m_pOutput;

public:
    void SetFaceCount(UINT _x, UINT _z) { m_iXFace = _x; m_iZFace = _z; }
    void SetCameraRay(const tRay& _ray) { m_ray = _ray; }
    void SetOuputBuffer(CStructuredBuffer* _pOutputBuffer) { m_pOutput = _pOutputBuffer; }
    void SetHeightMap(Ptr<CTexture> _ptex) { m_pHeightMap = _ptex; }

protected:
    virtual void UpdateData() override;
    virtual void Clear() override;

public:
    CRayCastShader(UINT _iGroupPerThreadX, UINT _iGroupPerThreadY, UINT _iGroupPerThreadZ);
    ~CRayCastShader();
};

