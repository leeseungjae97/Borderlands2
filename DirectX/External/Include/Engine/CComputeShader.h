#pragma once
#include "CShader.h"

class CComputeShader :
    public CShader
{
private:
    ComPtr<ID3D11ComputeShader>     m_CS;
    ComPtr<ID3DBlob>                m_CSBlob;   

protected:
    // ���̴��� ������ ��� ������
    tMtrlConst                      m_Const;

    // �׷� ����
    UINT                            m_iGroupX; 
    UINT                            m_iGroupY;
    UINT                            m_iGroupZ;

    // �׷� 1���� ������ ����
    UINT                            m_iGroupPerThreadX;
    UINT                            m_iGroupPerThreadY;
    UINT                            m_iGroupPerThreadZ;

public:
    void CreateComputeShader(const wstring& _strFileName, const string& _strFuncName);
    void Execute();

    void SetScalarParam(SCALAR_PARAM _Param, const void* _pSrc);

private:
    virtual void UpdateData() = 0;
    virtual void Clear() = 0;


    CLONE_DISABLE(CComputeShader);
public:
    CComputeShader();
    virtual ~CComputeShader();
};

