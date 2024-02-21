#pragma once
#include "CComputeShader.h"
class CStructuredBuffer;

class IndividualBoneSkinningShader :
    public CComputeShader
{
private:
    CStructuredBuffer* m_pOutputMatBuffer;
    CStructuredBuffer* m_pOutputVertexBuffer;
    CStructuredBuffer* m_pInputMatBuffer;


public:
    void SetIndices(Vec4 _vIndices) { m_Const.arrV4[1] = _vIndices; }
    void SetWeight(Vec4 _vWeight) { m_Const.arrV4[2] = _vWeight; }
    void SetPosition(Vec4 _vPos) { m_Const.arrV4[0] = _vPos; }
    
    void SetBoneIdx(int _Idx) { m_Const.arrInt[0] = _Idx; }
    void SetRot(bool _Rotate) { m_Const.arrInt[1] = _Rotate; }
    void SetPos(bool _Pos) { m_Const.arrInt[2] = _Pos; }

    //void SetVertexSkinning(bool _IsVertexSkinning) { m_Const.arrInt[1] = _IsVertexSkinning; }

    void SetOutputMatBuffer(CStructuredBuffer* _buffer) { m_pOutputMatBuffer = _buffer; }
    void SetOutputVertexBuffer(CStructuredBuffer* _buffer) { m_pOutputVertexBuffer = _buffer; }
    void SetInputBuffer(CStructuredBuffer* _buffer) { m_pInputMatBuffer = _buffer; }
public:
    void UpdateData() override;
    void Clear() override;
    

public:

    IndividualBoneSkinningShader(UINT _iGroupPerThreadX, UINT _iGroupPerThreadY, UINT _iGroupPerThreadZ);
    virtual ~IndividualBoneSkinningShader();
};

