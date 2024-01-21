#pragma once
#include "CComputeShader.h"
#include "CStructuredBuffer.h"
class CLandScapePathShader :
    public CComputeShader
{
private:
    CStructuredBuffer*  m_pPathBuffer;

    int                 m_iLandWidth;
    int                 m_iLandHeight;

    int                 m_iFaceX;
    int                 m_iFaceY;

    Vec3                m_vLandPos;

public:
    void SetPathBuffer(CStructuredBuffer* _Buffer) { m_pPathBuffer = _Buffer; }
    void SetLandWidth(int _LandWidth) { m_iLandWidth = _LandWidth; }
    void SetLandHeight(int _LandHeight) { m_iLandHeight = _LandHeight; }
    void SetFaceX(int _X) { m_iFaceX = _X; }
    void SetFaceY(int _Y) { m_iFaceY = _Y; }
    void SetLandPos(Vec3 _Pos) { m_vLandPos = _Pos; }

public:
    void UpdateData() override;
    void Clear() override;

public:
    CLandScapePathShader(UINT _iGroupPerThreadX, UINT _iGroupPerThreadY, UINT _iGroupPerThreadZ);
    ~CLandScapePathShader();
};

