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
    SetDeferredDecal(m_bDeferred);
}

CDecal::~CDecal()
{
}

void CDecal::render()
{
    if (nullptr == GetMaterial() || nullptr == GetMesh())
        return;

    Transform()->UpdateData();

    int IsEmissive = m_bEmissive;
    GetMaterial()->SetScalarParam(INT_0, &IsEmissive);
    GetMaterial()->UpdateData();

    GetMesh()->render();
}

void CDecal::finaltick()
{
    DrawDebugCube(Transform()->GetWorldMat(), Vec4(0.f, 1.f, 0.f, 1.f), 0.f, true);
}

void CDecal::SetDeferredDecal(bool _bDeferred)
{
    m_bDeferred = _bDeferred;

    if (m_bDeferred)
    {
        SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"DeferredDecalMtrl"));
	    //GetMaterial()->SetTexParam(TEX_1, CResMgr::GetInst()->FindRes<CTexture>(L"texture\\red.png"));
    }
    else
        SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"DecalMtrl"));

    GetMaterial()->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"PositionTargetTex"));
}