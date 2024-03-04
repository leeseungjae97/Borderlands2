#pragma once
#include "CRes.h"

#include "ptr.h"
#include "CGraphicsShader.h"
#include "CTexture.h"
#include "struct.h"

class CMaterial :
    public CRes
{
private:
    Ptr<CGraphicsShader>    m_pShader;
    tMtrlConst              m_Const;
    Ptr<CTexture>           m_arrTex[TEX_END];
   
    tPaperBurnConst         m_PaperBurnConst;

public:
    void SetScalarParam(SCALAR_PARAM _Param, const void* _Src);
    void SetTexParam(TEX_PARAM _Param, const Ptr<CTexture>& _Tex);

    void GetScalarParam(SCALAR_PARAM _param, void* _pData);
    Ptr<CTexture> GetTexParam(TEX_PARAM _param);

	void SetMaterialCoefficient(Vec4 _vDiff, Vec4 _vSpec, Vec4 _vAmb, Vec4 _vEmis)
    {
        m_Const.mtrl.vDiff = _vDiff;
        m_Const.mtrl.vAmb = _vAmb;
        m_Const.mtrl.vSpec = _vSpec;
        m_Const.mtrl.vEmv = _vEmis;
    }


    void SetShader(Ptr<CGraphicsShader> _Shader) { m_pShader = _Shader; }
    Ptr<CGraphicsShader> GetShader() { return m_pShader; }

    void SetAnim3D(bool _bTrue) { m_Const.arrAnimData[0] = (int)_bTrue; }
    void SetBoneCount(int _iBoneCount) { m_Const.arrAnimData[1] = _iBoneCount; }

    void SetTexFlow(bool _bTrue) { m_Const.arrAnimData[2] = (int)_bTrue; }
    void SetFlowSpeed(float _fSpeed) { m_Const.fTexFlowSpeed = _fSpeed; }

    void SetFlowDir(Vec2 _Dir) { m_Const.vTexDir = _Dir; }

    void SetEmissiveCoeff(float _Coeff) { m_Const.fEmisCoeff = _Coeff; }
    void SetFlow(int _Idx, bool _bFlow) { m_Const.arrTexFlow[_Idx] = _bFlow; }

    void SetPaperBurn(bool _Burn) { m_PaperBurnConst.paperAcc = 0.0f;  m_PaperBurnConst.paperBurn = _Burn; }
    void SetPaperBurnTime(float _Time) { m_PaperBurnConst.paperTime = _Time; }
    bool IsPaperBurnEnd() { return m_PaperBurnConst.paperBurnEnd; }

    tMtrlConst GetConst() { return m_Const; }

    virtual void UpdateData() override;
    void UpdateData_Instancing();

private:
    virtual int Load(const wstring& _strFilePath);
public:
    virtual int Save(const wstring& _strRelativePath);

public:
    CLONE(CMaterial);

public:
    CMaterial(bool _bEngine = false);
    ~CMaterial();
};

