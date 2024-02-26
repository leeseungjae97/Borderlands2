#include "pch.h"
#include "CRenderMgr.h"

#include "CDevice.h"
#include "CConstBuffer.h"
#include "CStructuredBuffer.h"

#include "CCamera.h"
#include "CEngine.h"
#include "CLevel.h"
#include "CLevelMgr.h"
#include "CLight3D.h"

#include "CResMgr.h"

#include "mMRT.h"

CRenderMgr::CRenderMgr()
    : m_Light2DBuffer(nullptr)
    , RENDER_FUNC(nullptr)
    , m_pEditorCam(nullptr)
    , m_MRT{}
{
    Vec2 vResolution = CDevice::GetInst()->GetRenderResolution();
    m_RTCopyTex = CResMgr::GetInst()->CreateTexture(L"RTCopyTex"
                                                    , (UINT)vResolution.x, (UINT)vResolution.y
                                                    , DXGI_FORMAT_R8G8B8A8_UNORM, D3D11_BIND_SHADER_RESOURCE
                                                    , D3D11_USAGE_DEFAULT);

    m_DownScaleTex = CResMgr::GetInst()->CreateTexture(L"DownScaleTex", vResolution.x / 4.f, vResolution.y / 4.f
        , DXGI_FORMAT_R8G8B8A8_UNORM
        , D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE);

    CResMgr::GetInst()->FindRes<CMaterial>(L"GrayMtrl")->SetTexParam(TEX_0, m_RTCopyTex);
    CResMgr::GetInst()->FindRes<CMaterial>(L"DistortionMtrl")->SetTexParam(TEX_0, m_RTCopyTex);
}

CRenderMgr::~CRenderMgr()
{
    if (nullptr != m_Light2DBuffer)
        delete m_Light2DBuffer;

    if (nullptr != m_Light3DBuffer)
        delete m_Light3DBuffer;

    DeleteArray(m_MRT);

    //Safe_Del_Vec(m_vecCam);
}


void CRenderMgr::init()
{
    // Light2DBuffer ����ȭ ���� ����
    m_Light2DBuffer = new CStructuredBuffer;
    m_Light2DBuffer->Create(sizeof(tLightInfo), 10, SB_TYPE::READ_ONLY, true);

    m_Light3DBuffer = new CStructuredBuffer;
    m_Light3DBuffer->Create(sizeof(tLightInfo), 10, SB_TYPE::READ_ONLY, true);

    CreateMRT();
}

void CRenderMgr::render()
{
    // ���� �� ���� ������ ������Ʈ �� ���ε�
    UpdateData();

    // MRT Clear    
    ClearMRT();

    render_shadowmap();

    // ���� �Լ� ȣ��
    (this->*RENDER_FUNC)();
    
    // ���� ����
    Clear();
}


void CRenderMgr::render_play()
{
    // ī�޶� ���� ������
    for (size_t i = 0; i < m_vecCam.size(); ++i)
    {
        if (nullptr == m_vecCam[i])
            continue;

        m_vecCam[i]->SortObject();

        m_MRT[(UINT)MRT_TYPE::SWAPCHAIN]->OMSet();

        m_vecCam[i]->render();
    }

    // Debug GameObject �׷��� �� �� Matrix ����
    m_vecCam[0]->FixedTransform();
}

void CRenderMgr::render_editor()
{
    if (nullptr == m_pEditorCam)
        return;

    // ��ü �з�
    m_pEditorCam->SortObject();

    // ��� Ÿ�� ����    
    m_MRT[(UINT)MRT_TYPE::SWAPCHAIN]->OMSet();

    m_pEditorCam->render();    
}


int CRenderMgr::RegisterCamera(CCamera* _Cam, int _idx)
{
    if (m_vecCam.size() <= _idx)
    {
        m_vecCam.resize(_idx + 1);
    }

    m_vecCam[_idx] = _Cam;
    return _idx;
}

void CRenderMgr::SetRenderFunc(bool _IsPlay)
{
    if(_IsPlay)
        RENDER_FUNC = &CRenderMgr::render_play;
    else
        RENDER_FUNC = &CRenderMgr::render_editor;
}

CCamera* CRenderMgr::GetMainCam()
{
    if (CLevelMgr::GetInst()->GetCurLevel()->GetState() == LEVEL_STATE::PLAY)
    {
        if (m_vecCam.empty())
            return nullptr;

        return m_vecCam[0];
    }
    else
    {
        return m_pEditorCam;
    }
}

CCamera* CRenderMgr::GetUICam()
{
    if (CLevelMgr::GetInst()->GetCurLevel()->GetState() == LEVEL_STATE::PLAY)
    {
        if (m_vecCam.empty())
            return nullptr;

        return m_vecCam[1];
    }
    return nullptr;
}

void CRenderMgr::CopyRenderTarget()
{
    Ptr<CTexture> pRTTex = CResMgr::GetInst()->FindRes<CTexture>(L"RenderTargetTex");
    CONTEXT->CopyResource(m_RTCopyTex->GetTex2D().Get(), pRTTex->GetTex2D().Get());
}

MRT* CRenderMgr::GetMRT(MRT_TYPE _Type)
{
    //for(int i =0 ; i < (int)MRT_TYPE::END; ++i)
    //{
    //    if (i == (int)_Type)
    //        continue;
    //    else
    //    {
    //        m_MRT[i]->SetRenderTarget(false);
    //    }
    //}
    m_CurMRT = m_MRT[(UINT)_Type];
    return m_MRT[(UINT)_Type];
}

void CRenderMgr::UpdateData()
{
    // GlobalData �� ���� �������� ����
    GlobalData.Light2DCount = m_vecLight2DInfo.size();
    GlobalData.Light3DCount = m_vecLight3DInfo.size();

    // ����ȭ������ ũ�Ⱑ ���ڶ�� �� ũ�� ���� �����.
    if (m_Light2DBuffer->GetElementCount() < m_vecLight2DInfo.size())
    {
        m_Light2DBuffer->Create(sizeof(tLightInfo), m_vecLight2DInfo.size(), SB_TYPE::READ_ONLY, true);
    }
    if (m_Light3DBuffer->GetElementCount() < m_vecLight3DInfo.size())
    {
        m_Light3DBuffer->Create(sizeof(tLightInfo), m_vecLight3DInfo.size(), SB_TYPE::READ_ONLY, true);
    }

    // ����ȭ���۷� ���� �����͸� �ű��.
    m_Light2DBuffer->SetData(m_vecLight2DInfo.data(), sizeof(tLightInfo) * m_vecLight2DInfo.size());
    m_Light3DBuffer->SetData(m_vecLight3DInfo.data(), sizeof(tLightInfo) * m_vecLight3DInfo.size());

    m_Light2DBuffer->UpdateData(12, PIPELINE_STAGE::PS_PIXEL);
    m_Light3DBuffer->UpdateData(13, PIPELINE_STAGE::PS_PIXEL);

    // ���� ��� ������ ���ε�
    CConstBuffer* pGlobalBuffer = CDevice::GetInst()->GetConstBuffer(CB_TYPE::GLOBAL);
    pGlobalBuffer->SetData(&GlobalData, sizeof(tGlobal));
    pGlobalBuffer->UpdateData();
    pGlobalBuffer->UpdateData_CS();
}

void CRenderMgr::render_shadowmap()
{
    // ShadowMap MRT �� ��ü
    GetMRT(MRT_TYPE::SHADOWMAP)->OMSet();
    //m_MRT[(UINT)MRT_TYPE::SHADOWMAP]->OMSet();

    for (size_t i = 0; i < m_vecLight3D.size(); ++i)
    {
        if(m_vecLight3D[i])
			m_vecLight3D[i]->render_shadowmap();
    }
}


void CRenderMgr::Clear()
{
    m_vecLight2DInfo.clear();
    m_vecLight2D.clear();
    m_vecLight3DInfo.clear();
    m_vecLight3D.clear();
}

void CRenderMgr::CreateMRT()
{
    Vec2 vResol = CEngine::GetInst()->GetWindowResolution();
    // ====================
    // SwapChain MRT
    // ====================
    {
        m_MRT[(UINT)MRT_TYPE::SWAPCHAIN] = new MRT;

        Ptr<CTexture> arrRTTex[8] = {};
        arrRTTex[0] = CResMgr::GetInst()->FindRes<CTexture>(L"RenderTargetTex");

        Ptr<CTexture> pDSTex = CResMgr::GetInst()->FindRes<CTexture>(L"DepthStencilTex");
        
        m_MRT[(UINT)MRT_TYPE::SWAPCHAIN]->Create(arrRTTex, 1, pDSTex);
        m_MRT[(UINT)MRT_TYPE::SWAPCHAIN]->SetClearColor(Vec4(1.f, 0.f, 0.f, 1.f), 0);
    }
    // ====================
	// HDR MRT
	// ====================
    {
        m_MRT[(UINT)MRT_TYPE::HDR] = new MRT;
        

        Ptr<CTexture> arrRTTex[8] = {};
        arrRTTex[0] = CResMgr::GetInst()->CreateTexture(L"HDRTargetTex", vResol.x, vResol.y
            , DXGI_FORMAT_R32G32B32A32_FLOAT
            , D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE);

        m_MRT[(UINT)MRT_TYPE::HDR]->Create(arrRTTex, 1, nullptr);
    }
    {
        m_MRT[(UINT)MRT_TYPE::HDR_LINE] = new MRT;


        Ptr<CTexture> arrRTTex[8] = {};
        arrRTTex[0] = CResMgr::GetInst()->CreateTexture(L"OutlineHDRTargetTex", vResol.x, vResol.y
            , DXGI_FORMAT_R32G32B32A32_FLOAT
            , D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE);

        m_MRT[(UINT)MRT_TYPE::HDR_LINE]->Create(arrRTTex, 1, nullptr);
    }

    {
        m_MRT[(UINT)MRT_TYPE::OUT_LINE] = new MRT;

        Ptr<CTexture> arrRTTex[8] = {};
        arrRTTex[0] = CResMgr::GetInst()->CreateTexture(L"OutlineTargetTex", vResol.x, vResol.y
            , DXGI_FORMAT_R32_FLOAT
            , D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET);

        m_MRT[(UINT)MRT_TYPE::OUT_LINE]->Create(arrRTTex, 1, nullptr);
    }
    {
        m_MRT[(UINT)MRT_TYPE::OUT_LINE_PLUS] = new MRT;

        Ptr<CTexture> arrRTTex[8] = {};
        arrRTTex[0] = CResMgr::GetInst()->CreateTexture(L"OutlinePlusSizeTargetTex", vResol.x, vResol.y
            , DXGI_FORMAT_R32_FLOAT
            , D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET);

        m_MRT[(UINT)MRT_TYPE::OUT_LINE_PLUS]->Create(arrRTTex, 1, nullptr);
        
    }
    // ====================
	// Bloomed HDR MRT
	// ====================
    {
        m_MRT[(UINT)MRT_TYPE::BLOOMED_HDR] = new MRT;

        Ptr<CTexture> arrRTTex[8] = {};
        arrRTTex[0] = CResMgr::GetInst()->CreateTexture(L"BloomedHDRTargetTex", vResol.x, vResol.y
            , DXGI_FORMAT_R32G32B32A32_FLOAT
            , D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE);

        m_MRT[(UINT)MRT_TYPE::BLOOMED_HDR]->Create(arrRTTex, 1, nullptr);
    }

    // ====================
    // Light MRT
    // ====================
    {
        m_MRT[(UINT)MRT_TYPE::LIGHT] = new MRT;

        Ptr<CTexture> arrRTTex[8] = {};
        arrRTTex[0] = CResMgr::GetInst()->CreateTexture(L"DiffuseTargetTex", vResol.x, vResol.y
            , DXGI_FORMAT_R8G8B8A8_UNORM
            , D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET);

        arrRTTex[1] = CResMgr::GetInst()->CreateTexture(L"SpecularTargetTex", vResol.x, vResol.y
            , DXGI_FORMAT_R8G8B8A8_UNORM
            , D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET);

        
        arrRTTex[2] = CResMgr::GetInst()->CreateTexture(L"ShadowTargetTex", vResol.x, vResol.y
            , DXGI_FORMAT_R32_FLOAT
            , D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET);

        m_MRT[(UINT)MRT_TYPE::LIGHT]->Create(arrRTTex, 3, nullptr);
        //m_MRT[(UINT)MRT_TYPE::LIGHT]->SetClearColor(Vec4(0.f, 1.f, 0.f, 1.f), 0);
    }
    // ====================
	// Deferred MRT
	// ====================
    {
        m_MRT[(UINT)MRT_TYPE::DEFERRED] = new MRT;

        Ptr<CTexture> arrRTTex[8] = {};
        arrRTTex[0] = CResMgr::GetInst()->CreateTexture(L"ColorTargetTex", vResol.x, vResol.y
            , DXGI_FORMAT_R8G8B8A8_UNORM
            , D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET);

        arrRTTex[1] = CResMgr::GetInst()->CreateTexture(L"NormalTargetTex", vResol.x, vResol.y
            , DXGI_FORMAT_R32G32B32A32_FLOAT
            , D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET);

        arrRTTex[2] = CResMgr::GetInst()->CreateTexture(L"PositionTargetTex", vResol.x, vResol.y
            , DXGI_FORMAT_R32G32B32A32_FLOAT
            , D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET);

        arrRTTex[3] = CResMgr::GetInst()->CreateTexture(L"EmissiveTargetTex", vResol.x, vResol.y
            , DXGI_FORMAT_R8G8B8A8_UNORM
            , D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET);

        arrRTTex[4] = CResMgr::GetInst()->CreateTexture(L"DataTargetTex", vResol.x, vResol.y
            , DXGI_FORMAT_R32G32B32A32_FLOAT
            , D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET);

        arrRTTex[5] = CResMgr::GetInst()->FindRes<CTexture>(L"SpecularTargetTex");

        arrRTTex[6] = CResMgr::GetInst()->FindRes<CTexture>(L"DiffuseTargetTex");

        m_MRT[(UINT)MRT_TYPE::DEFERRED]->Create(arrRTTex, 7, nullptr);
    }
    // ====================
	// Luminance MRT
	// ====================
    {
        m_MRT[(UINT)MRT_TYPE::LUMINANCE] = new MRT;

        Ptr<CTexture> arrRTTex[8] = {};

        arrRTTex[0] = CResMgr::GetInst()->CreateTexture(L"EmissiveBlurredTargetTex", vResol.x, vResol.y
            , DXGI_FORMAT_R8G8B8A8_UNORM
            , D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET);

        arrRTTex[1] = CResMgr::GetInst()->CreateTexture(L"SpecularBlurredTargetTex", vResol.x, vResol.y
            , DXGI_FORMAT_R8G8B8A8_UNORM
            , D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET);

        m_MRT[(UINT)MRT_TYPE::LUMINANCE]->Create(arrRTTex, 2, nullptr);
    }
    // ====================
    // Decal MRT
    // ====================
    {
        m_MRT[(UINT)MRT_TYPE::DEFERRED_DECAL] = new MRT;

        Ptr<CTexture> arrRTTex[8] = {};
        arrRTTex[0] = CResMgr::GetInst()->FindRes<CTexture>(L"ColorTargetTex");
        arrRTTex[1] = CResMgr::GetInst()->FindRes<CTexture>(L"EmissiveTargetTex");

        m_MRT[(UINT)MRT_TYPE::DEFERRED_DECAL]->Create(arrRTTex, 2, nullptr);
    }
    // ====================
    // ShadowMap MRT
    // ====================
    {
        m_MRT[(UINT)MRT_TYPE::SHADOWMAP] = new MRT;
    
        //Vec2 vResol = Vec2(8192, 8192);
        Vec2 vTexResol = Vec2(4096, 4096);

        Ptr<CTexture> arrRTTex[8] = {};
        arrRTTex[0] = CResMgr::GetInst()->CreateTexture(L"DynamicShadowMapTex", vTexResol.x, vTexResol.y
                                                    , DXGI_FORMAT_R32_FLOAT
                                                    , D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE);
            
        Ptr<CTexture> pDSTex = CResMgr::GetInst()->CreateTexture(L"DynamicShadowMapDepthTex", vTexResol.x, vTexResol.y
                                                    , DXGI_FORMAT_D32_FLOAT
                                                    , D3D11_BIND_DEPTH_STENCIL);

        m_MRT[(UINT)MRT_TYPE::SHADOWMAP]->Create(arrRTTex, 1, pDSTex);
        m_MRT[(UINT)MRT_TYPE::SHADOWMAP]->SetClearColor(Vec4(1.f, 0.f, 0.f, 1.f), 0);
    }
    //{
	   // m_MRT[(UINT)MRT_TYPE::STENCIL_CULL] = new MRT;
    //
    //    Vec2 vResol = CDevice::GetInst()->GetRenderResolution();

    //    Ptr<CTexture> arrRTTex[8] = {};
    //    Ptr<CTexture> pDSTex = CResMgr::GetInst()->CreateTexture(L"DepthCullingStencilTex", vResol.x, vResol.y
    //                                                , DXGI_FORMAT_D32_FLOAT
    //                                                , D3D11_BIND_DEPTH_STENCIL);

    //    m_MRT[(UINT)MRT_TYPE::STENCIL_CULL]->Create(nullptr, 0, pDSTex);
    //    m_MRT[(UINT)MRT_TYPE::STENCIL_CULL]->SetClearColor(Vec4(1.f, 0.f, 0.f, 1.f), 0);
    //}
    //{
	   // m_MRT[(UINT)MRT_TYPE::STENCIL_CULL_DEPLOY] = new MRT;
    //
    //    Vec2 vResol = CDevice::GetInst()->GetRenderResolution();

    //    Ptr<CTexture> arrRTTex[8] = {};
    //    Ptr<CTexture> pDSTex = CResMgr::GetInst()->CreateTexture(L"DepthStencilDeployTex", vResol.x, vResol.y
    //                                                , DXGI_FORMAT_D32_FLOAT
    //                                                , D3D11_BIND_DEPTH_STENCIL);

    //    m_MRT[(UINT)MRT_TYPE::STENCIL_CULL_DEPLOY]->Create(nullptr, 0, pDSTex);
    //    m_MRT[(UINT)MRT_TYPE::STENCIL_CULL_DEPLOY]->SetClearColor(Vec4(1.f, 0.f, 0.f, 1.f), 0);
    //}
}

void CRenderMgr::ClearMRT()
{
    for (UINT i = 0; i < (UINT)MRT_TYPE::END; ++i)
    {
        if (nullptr != m_MRT[i])
        {
            m_MRT[i]->ClearTarget();
        }
    }
}