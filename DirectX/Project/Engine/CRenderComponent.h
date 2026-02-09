#pragma once
#include "CComponent.h"


#include "CMesh.h"
#include "CMaterial.h"
#include "ptr.h"

struct tMtrlSet
{
    Ptr<CMaterial>  pSharedMtrl; 
    Ptr<CMaterial>  pDynamicMtrl;
    Ptr<CMaterial>  pCurrentMtrl;
};

class CRenderComponent :
    public CComponent
{
private:
    Ptr<CMesh>              m_pMesh;
    vector<tMtrlSet>        m_vecMtrls;

    float                   m_fBounding;
    bool                    m_bFrustumCheck;

public:
    virtual void render() = 0;
    virtual void render(UINT _iSubset, bool _Deferred) = 0;
    virtual void render_shadowmap();

public:
    void SetMesh(Ptr<CMesh> _Mesh);
    void SetMaterial(Ptr<CMaterial> _Mtrl, UINT _Idx);

    void SetPaparBurn(bool _Burn, float _BurnTime);

    Ptr<CMesh> GetMesh() { return m_pMesh; }

    Ptr<CMaterial> GetMaterial(UINT _Idx);
    Ptr<CMaterial> GetSharedMaterial(UINT _Idx);
    Ptr<CMaterial> GetDynamicMaterial(UINT _Idx);
    ULONG64 GetInstID(UINT _iMtrlIdx);

    UINT GetMtrlCount() { return (UINT)m_vecMtrls.size(); }

    void SetFrustumCheck(bool _bUse) { m_bFrustumCheck = _bUse; }
    bool IsUseFrustumCheck() { return m_bFrustumCheck; }

    void SetBounding(float _fBounding) { m_fBounding = _fBounding; }
    float GetBounding() { return m_fBounding; }

    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;

    bool IsFrustumCheck() { return m_bFrustumCheck; }
public:
    CRenderComponent(COMPONENT_TYPE _type);
    virtual ~CRenderComponent();
};

