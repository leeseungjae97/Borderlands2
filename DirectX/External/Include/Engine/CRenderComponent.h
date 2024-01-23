#pragma once
#include "CComponent.h"


#include "CMesh.h"
#include "CMaterial.h"
#include "ptr.h"

struct tMtrlSet
{
    Ptr<CMaterial>  pSharedMtrl;    // 공유 메터리얼
    Ptr<CMaterial>  pDynamicMtrl;   // 공유 메터리얼 복사본
    Ptr<CMaterial>  pCurrentMtrl;       // 현재 사용 할 메터리얼
};

class CRenderComponent :
    public CComponent
{
private:
    Ptr<CMesh>              m_pMesh;
    vector<tMtrlSet>        m_vecMtrls;

    float                   m_fBounding;
    bool                    m_bFrustumCheck;
    bool                    m_bDynamicShadow;

public:
    virtual void render() = 0;
    virtual void render(UINT _iSubset) = 0;
    virtual void render_shadowmap();

    void render_shadowmapexp();

public:
    void SetMesh(Ptr<CMesh> _Mesh);
    void SetMaterial(Ptr<CMaterial> _Mtrl, UINT _Idx);

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

    void SetDynamicShadow(bool _bSet) { m_bDynamicShadow = _bSet; }
    bool IsDynamicShadow() { return m_bDynamicShadow; }

    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;

    bool IsFrustumCheck() { return m_bFrustumCheck; }
public:
    CRenderComponent(COMPONENT_TYPE _type);
    ~CRenderComponent();
};

