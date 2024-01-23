#include "pch.h"
#include "CDecal.h"

#include "CKeyMgr.h"
#include "CResMgr.h"
#include "CTimeMgr.h"
#include "CTransform.h"

CDecal::CDecal()
    : CRenderComponent(COMPONENT_TYPE::DECAL)
    , m_bDeferred(false)
    , m_bEmissive(false)
{
    SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"CubeMesh"));
    m_DecalTex = CResMgr::GetInst()->FindRes<CTexture>(L"texture\\red_circle.png");
    SetDeferredDecal(m_bDeferred);
}

CDecal::~CDecal()
{
}

void CDecal::render()
{
    if (nullptr == GetMaterial(0) || nullptr == GetMesh())
        return;

    Transform()->UpdateData();

    int IsEmissive = m_bEmissive;
    GetMaterial(0)->SetScalarParam(INT_0, &IsEmissive);
    GetMaterial(0)->UpdateData();

    GetMesh()->render(0);
}

void CDecal::finaltick()
{
    //DrawDebugCube(Transform()->GetWorldMat(), Vec4(0.f, 1.f, 0.f, 1.f), 0.f, true);
}

void CDecal::SetDecalTex(Ptr<CTexture> _Tex)
{
    m_DecalTex = _Tex;
    GetMaterial(0)->SetTexParam(TEX_1, m_DecalTex);
}

void CDecal::SetDeferredDecal(bool _bDeferred)
{
    m_bDeferred = _bDeferred;

    if (m_bDeferred)
    {
        SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"DeferredDecalMtrl"), 0);
	    GetMaterial(0)->SetTexParam(TEX_1, m_DecalTex);
    }
    else
        SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"DecalMtrl"), 0);

    GetMaterial(0)->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"PositionTargetTex"));
}

void CDecal::LoadFromLevelFile(FILE* _FILE)
{
    LoadResRef(m_DecalTex, _FILE);

    fread(&m_bDeferred, sizeof(bool), 1, _FILE);
    fread(&m_bEmissive, sizeof(bool), 1, _FILE);
}

void CDecal::SaveToLevelFile(FILE* _File)
{
    SaveResRef(m_DecalTex.Get(), _File);

    fwrite(&m_bDeferred, sizeof(bool), 1, _File);
    fwrite(&m_bEmissive, sizeof(bool), 1, _File);
}

void CDecal::render(UINT _iSubset)
{
    render();
}
