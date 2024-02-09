#pragma once
#include "CComputeShader.h"
class CStructuredBuffer;

class IndividualBoneSkinningShader :
    public CComputeShader
{
private:
    CStructuredBuffer* m_pOutputBuffer;
    CStructuredBuffer* m_pBoneMatBuffer;

public:
    void SetIndices(Vec4 _vIndices) { m_Const.arrV4[1] = _vIndices; }
    void SetWeight(Vec4 _vWeight) { m_Const.arrV4[2] = _vWeight; }
    void SetPosition(Vec4 _vPos) { m_Const.arrV4[0] = _vPos; }

    void SetBoneIdx(int _Idx) { m_Const.arrInt[0] = _Idx; }
    void SetBoneSkinning(bool _IsBoneSkinning) { m_Const.arrInt[1] = _IsBoneSkinning; }

    void SetOutputBuffer(CStructuredBuffer* _buffer) { m_pOutputBuffer = _buffer; }
    void SetBoneMat(CStructuredBuffer* _buffer) { m_pBoneMatBuffer = _buffer; }
public:
    void UpdateData() override;
    void Clear() override;
    

public:

    IndividualBoneSkinningShader(UINT _iGroupPerThreadX, UINT _iGroupPerThreadY, UINT _iGroupPerThreadZ);
    virtual ~IndividualBoneSkinningShader();
};

