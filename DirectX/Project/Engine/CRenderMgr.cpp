#include "pch.h"
#include "CRenderMgr.h"

#include "CDevice.h"
#include "CConstBuffer.h"
#include "CStructuredBuffer.h"

#include "CCamera.h"
#include "CEngine.h"
#include "CEventMgr.h"
#include "CLayer.h"
#include "CLevel.h"
#include "CLevelMgr.h"
#include "CLight3D.h"

#include "CResMgr.h"

#include "mMRT.h"

CRenderMgr::CRenderMgr()
	: m_Light2DBuffer(nullptr)
	, RENDER_FUNC(nullptr)
	//, m_pEditorCam(nullptr)
	, m_MRT{}
{
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
	Vec2 vResolution = CDevice::GetInst()->GetRenderResolution();
	m_RTCopyTex = CResMgr::GetInst()->CreateTexture(L"RTCopyTex"
		, (UINT)vResolution.x, (UINT)vResolution.y
		, DXGI_FORMAT_R8G8B8A8_UNORM, D3D11_BIND_SHADER_RESOURCE
		, D3D11_USAGE_DEFAULT);

	m_RTCopyDownScaleTex = CResMgr::GetInst()->CreateUAVTexture(L"RTCopyDownScaleTex"
		, (UINT)500, (UINT)500
		, DXGI_FORMAT_R8G8B8A8_UNORM, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS
		, D3D11_USAGE_DEFAULT);

	m_RTCopyTex2 = CResMgr::GetInst()->CreateTexture(L"RTCopyTex2"
		, (UINT)vResolution.x, (UINT)vResolution.y
		, DXGI_FORMAT_R8G8B8A8_UNORM, D3D11_BIND_SHADER_RESOURCE
		, D3D11_USAGE_DEFAULT);

	m_RTCopyTex3 = CResMgr::GetInst()->CreateTexture(L"RTCopyTex3"
		, (UINT)vResolution.x, (UINT)vResolution.y
		, DXGI_FORMAT_R8G8B8A8_UNORM, D3D11_BIND_SHADER_RESOURCE
		, D3D11_USAGE_DEFAULT);

	CResMgr::GetInst()->FindRes<CMaterial>(L"GrayMtrl")->SetTexParam(TEX_0, m_RTCopyTex);
	CResMgr::GetInst()->FindRes<CMaterial>(L"DistortionMtrl")->SetTexParam(TEX_0, m_RTCopyTex);

	// Light2DBuffer 구조화 버퍼 생성
	m_Light2DBuffer = new CStructuredBuffer;
	m_Light2DBuffer->Create(sizeof(tLightInfo), 10, SB_TYPE::READ_ONLY, true);

	m_Light3DBuffer = new CStructuredBuffer;
	m_Light3DBuffer->Create(sizeof(tLightInfo), 10, SB_TYPE::READ_ONLY, true);

	CreateMRT();

	CResMgr::GetInst()->RoadResource();
}

void CRenderMgr::render()
{
	if (CEventMgr::GetInst()->IsLevelLoad())
	{
		CLevel* pLevel = CLevelMgr::GetInst()->GetCurLevel();
		vector<CGameObject*> cams = pLevel->GetLayer((UINT)LAYER_TYPE::Camera)->GetObjects();
		m_vecCam.resize(cams.size());

		for (int i = 0; i < cams.size(); ++i)
		{
			m_vecCam[i] = cams[i]->Camera();
		}
	}
	// 광원 및 전역 데이터 업데이트 및 바인딩
	UpdateData();

	// MRT Clear    
	ClearMRT();

	render_shadowmap();

	// 렌더 함수 호출
	(this->*RENDER_FUNC)();

	// 광원 해제
	Clear();
}


void CRenderMgr::render_play()
{
	// 카메라 기준 렌더링

	//if (nullptr == m_vecCam[0])
	//    return;

	//m_vecCam[0]->SortObject();
	//m_vecCam[0]->render();

	//m_vecCam[1]->SortObject();
	//m_vecCam[1]->render();

	for (int i = 0; i < m_vecCam.size(); ++i)
	{
		if (nullptr == m_vecCam[i])
		    continue;

		m_vecCam[i]->SortObject();

		m_vecCam[i]->render();
	}

	m_vecCam[0]->FixedTransform();
}

void CRenderMgr::render_editor()
{
	if (nullptr == m_vecCam[0])
		return;

	m_vecCam[0]->SortObject();

	m_vecCam[0]->render();
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
	RENDER_FUNC = &CRenderMgr::render_play;

	//if(_IsPlay)
	//    
	//else
	//    RENDER_FUNC = &CRenderMgr::render_editor;
}

CCamera* CRenderMgr::GetMainCam()
{
	if (m_vecCam.empty())
		return nullptr;

	return m_vecCam[0];

	//if (CLevelMgr::GetInst()->GetCurLevel()->GetState() == LEVEL_STATE::PLAY)
	//{
		//if (m_vecCam.empty())
		//    return nullptr;

		//return m_vecCam[0];
	//}
	//else
	//{
	//    return m_pEditorCam;
	//}
}

CCamera* CRenderMgr::GetUICam()
{
	if (CLevelMgr::GetInst()->GetCurLevel()->GetState() == LEVEL_STATE::PLAY)
	{
		if (m_vecCam.empty())
			return nullptr;

		for(int i = 0 ; i < m_vecCam.size(); ++i)
		{
			if(m_vecCam[i]->GetCamIdx() == 3)
				return m_vecCam[i];
		}
		
	}
	return nullptr;
}

void CRenderMgr::CopyRenderTarget()
{
	Ptr<CTexture> pRTTex = CResMgr::GetInst()->FindRes<CTexture>(L"ScopeRenderTex");
	CONTEXT->CopyResource(m_RTCopyTex->GetTex2D().Get(), pRTTex->GetTex2D().Get());

	pRTTex = CResMgr::GetInst()->FindRes<CTexture>(L"MapRenderTex");
	CONTEXT->CopyResource(m_RTCopyTex2->GetTex2D().Get(), pRTTex->GetTex2D().Get());

	pRTTex = CResMgr::GetInst()->FindRes<CTexture>(L"RenderTargetTex");
	CONTEXT->CopyResource(m_RTCopyTex3->GetTex2D().Get(), pRTTex->GetTex2D().Get());
}

MRT* CRenderMgr::GetMRT(MRT_TYPE _Type)
{
	return m_MRT[(UINT)_Type];
}

void CRenderMgr::UpdateData()
{
	GlobalData.Light2DCount = m_vecLight2DInfo.size();
	GlobalData.Light3DCount = m_vecLight3DInfo.size();

	if (m_Light2DBuffer->GetElementCount() < m_vecLight2DInfo.size())
	{
		m_Light2DBuffer->Create(sizeof(tLightInfo), m_vecLight2DInfo.size(), SB_TYPE::READ_ONLY, true);
	}
	if (m_Light3DBuffer->GetElementCount() < m_vecLight3DInfo.size())
	{
		m_Light3DBuffer->Create(sizeof(tLightInfo), m_vecLight3DInfo.size(), SB_TYPE::READ_ONLY, true);
	}

	m_Light2DBuffer->SetData(m_vecLight2DInfo.data(), sizeof(tLightInfo) * m_vecLight2DInfo.size());
	m_Light3DBuffer->SetData(m_vecLight3DInfo.data(), sizeof(tLightInfo) * m_vecLight3DInfo.size());

	m_Light2DBuffer->UpdateData(12, PIPELINE_STAGE::PS_PIXEL);
	m_Light3DBuffer->UpdateData(13, PIPELINE_STAGE::PS_PIXEL);

	CConstBuffer* pGlobalBuffer = CDevice::GetInst()->GetConstBuffer(CB_TYPE::GLOBAL);
	pGlobalBuffer->SetData(&GlobalData, sizeof(tGlobal));
	pGlobalBuffer->UpdateData();
	pGlobalBuffer->UpdateData_CS();
}

void CRenderMgr::render_shadowmap()
{
	GetMRT(MRT_TYPE::SHADOWMAP)->OMSet();
	//m_MRT[(UINT)MRT_TYPE::SHADOWMAP]->OMSet();

	for (size_t i = 0; i < m_vecLight3D.size(); ++i)
	{
		if (m_vecLight3D[i])
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
	//Vec2 vMapResol = Vec2(500.f, 500.f);
	Vec2 vMapResol = vResol;
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

		arrRTTex[3] = CResMgr::GetInst()->CreateTexture(L"EmissiveTargetTex", vResol.x, vResol.y
			, DXGI_FORMAT_R8G8B8A8_UNORM
			, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET);

		m_MRT[(UINT)MRT_TYPE::LIGHT]->Create(arrRTTex, 4, nullptr);
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

		arrRTTex[2] = CResMgr::GetInst()->CreateTexture(L"TangentTargetTex", vResol.x, vResol.y
			, DXGI_FORMAT_R32G32B32A32_FLOAT
			, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET);

		arrRTTex[3] = CResMgr::GetInst()->CreateTexture(L"PositionTargetTex", vResol.x, vResol.y
			, DXGI_FORMAT_R32G32B32A32_FLOAT
			, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET);

		arrRTTex[4] = CResMgr::GetInst()->FindRes<CTexture>(L"EmissiveTargetTex");

		arrRTTex[5] = CResMgr::GetInst()->CreateTexture(L"DataTargetTex", vResol.x, vResol.y
			, DXGI_FORMAT_R32G32B32A32_FLOAT
			, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET);

		//arrRTTex[5] = CResMgr::GetInst()->FindRes<CTexture>(L"SpecularTargetTex");

		//arrRTTex[6] = CResMgr::GetInst()->FindRes<CTexture>(L"DiffuseTargetTex");

		m_MRT[(UINT)MRT_TYPE::DEFERRED]->Create(arrRTTex, 6, nullptr);
	}
	// ====================
	// Luminance MRT
	// ====================
	{
		m_MRT[(UINT)MRT_TYPE::BLUR_V] = new MRT;

		Ptr<CTexture> arrRTTex[8] = {};

		arrRTTex[0] = CResMgr::GetInst()->CreateTexture(L"EmissiveVerticalBlurredTargetTex", vResol.x, vResol.y
			, DXGI_FORMAT_R8G8B8A8_UNORM
			, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET);

		//arrRTTex[1] = CResMgr::GetInst()->CreateTexture(L"DiffuseBlurredTargetTex", vResol.x, vResol.y
		//    , DXGI_FORMAT_R8G8B8A8_UNORM
		//    , D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET);

		m_MRT[(UINT)MRT_TYPE::BLUR_V]->Create(arrRTTex, 1, nullptr);
	}
	{
		m_MRT[(UINT)MRT_TYPE::BLUR_H] = new MRT;

		Ptr<CTexture> arrRTTex[8] = {};

		arrRTTex[0] = CResMgr::GetInst()->CreateTexture(L"EmissiveHorizontalBlurredTargetTex", vResol.x, vResol.y
			, DXGI_FORMAT_R8G8B8A8_UNORM
			, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET);

		//arrRTTex[1] = CResMgr::GetInst()->CreateTexture(L"DiffuseBlurredTargetTex", vResol.x, vResol.y
		//    , DXGI_FORMAT_R8G8B8A8_UNORM
		//    , D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET);

		m_MRT[(UINT)MRT_TYPE::BLUR_H]->Create(arrRTTex, 1, nullptr);
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
	{
		m_MRT[(UINT)MRT_TYPE::SCOPE_RENDER] = new MRT;

		Vec2 vScopeResol = Vec2(vResol.x, vResol.y);

		Ptr<CTexture> arrRTTex[8] = {};

		arrRTTex[0] = CResMgr::GetInst()->CreateTexture(L"ScopeRenderTex", vScopeResol.x, vScopeResol.y
			, DXGI_FORMAT_R8G8B8A8_UNORM
			, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE);

		Ptr<CTexture> pDSTex = CResMgr::GetInst()->CreateTexture(L"ScopeDepthStencilTex", vScopeResol.x, vScopeResol.y
			, DXGI_FORMAT_D24_UNORM_S8_UINT, D3D11_BIND_DEPTH_STENCIL, D3D11_USAGE_DEFAULT);

		m_MRT[(UINT)MRT_TYPE::SCOPE_RENDER]->Create(arrRTTex, 1, pDSTex);
		m_MRT[(UINT)MRT_TYPE::SCOPE_RENDER]->SetClearColor(Vec4(1.f, 0.f, 0.f, 1.f), 0);
	}
	{
		m_MRT[(UINT)MRT_TYPE::SCOPE_DEFERRED_RENDER] = new MRT;
		Vec2 vScopeResol = Vec2(vResol.x, vResol.y);
		Ptr<CTexture> arrRTTex[8] = {};
		arrRTTex[0] = CResMgr::GetInst()->CreateTexture(L"ScopeColorTargetTex", vScopeResol.x, vScopeResol.y
			, DXGI_FORMAT_R8G8B8A8_UNORM
			, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET);

		arrRTTex[1] = CResMgr::GetInst()->CreateTexture(L"ScopeNormalTargetTex", vScopeResol.x, vScopeResol.y
			, DXGI_FORMAT_R32G32B32A32_FLOAT
			, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET);

		arrRTTex[2] = CResMgr::GetInst()->CreateTexture(L"ScopeTangentTargetTex", vScopeResol.x, vScopeResol.y
			, DXGI_FORMAT_R32G32B32A32_FLOAT
			, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET);

		arrRTTex[3] = CResMgr::GetInst()->CreateTexture(L"ScopePositionTargetTex", vScopeResol.x, vScopeResol.y
			, DXGI_FORMAT_R32G32B32A32_FLOAT
			, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET);

		arrRTTex[4] = CResMgr::GetInst()->CreateTexture(L"ScopeEmissiveTargetTex", vScopeResol.x, vScopeResol.y
			, DXGI_FORMAT_R8G8B8A8_UNORM
			, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET);

		arrRTTex[5] = CResMgr::GetInst()->CreateTexture(L"ScopeDataTargetTex", vScopeResol.x, vScopeResol.y
			, DXGI_FORMAT_R32G32B32A32_FLOAT
			, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET);

		m_MRT[(UINT)MRT_TYPE::SCOPE_DEFERRED_RENDER]->Create(arrRTTex, 6, nullptr);
	}
	{
		m_MRT[(UINT)MRT_TYPE::SCOPE_LIGHT_RENDER] = new MRT;
		Vec2 vScopeResol = Vec2(vResol.x, vResol.y);
		Ptr<CTexture> arrRTTex[8] = {};
		arrRTTex[0] = CResMgr::GetInst()->CreateTexture(L"ScopeDiffuseTargetTex", vScopeResol.x, vScopeResol.y
			, DXGI_FORMAT_R8G8B8A8_UNORM
			, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET);

		arrRTTex[1] = CResMgr::GetInst()->CreateTexture(L"ScopeSpecularTargetTex", vScopeResol.x, vScopeResol.y
			, DXGI_FORMAT_R8G8B8A8_UNORM
			, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET);


		arrRTTex[2] = CResMgr::GetInst()->CreateTexture(L"ScopeShadowTargetTex", vScopeResol.x, vScopeResol.y
			, DXGI_FORMAT_R32_FLOAT
			, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET);

		arrRTTex[3] = CResMgr::GetInst()->FindRes<CTexture>(L"ScopeEmissiveTargetTex");

		m_MRT[(UINT)MRT_TYPE::SCOPE_LIGHT_RENDER]->Create(arrRTTex, 4, nullptr);
	}
	{
		m_MRT[(UINT)MRT_TYPE::SCOPE_SHADOWMAP_RENDER] = new MRT;

		//Vec2 vResol = Vec2(8192, 8192);
		Vec2 vTexResol = Vec2(4096, 4096);

		Ptr<CTexture> arrRTTex[8] = {};
		arrRTTex[0] = CResMgr::GetInst()->CreateTexture(L"ScopeDynamicShadowMapTex", vTexResol.x, vTexResol.y
			, DXGI_FORMAT_R32_FLOAT
			, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE);

		Ptr<CTexture> pDSTex = CResMgr::GetInst()->CreateTexture(L"ScopeDynamicShadowMapDepthTex", vTexResol.x, vTexResol.y
			, DXGI_FORMAT_D32_FLOAT
			, D3D11_BIND_DEPTH_STENCIL);

		m_MRT[(UINT)MRT_TYPE::SCOPE_SHADOWMAP_RENDER]->Create(arrRTTex, 1, pDSTex);
		m_MRT[(UINT)MRT_TYPE::SCOPE_SHADOWMAP_RENDER]->SetClearColor(Vec4(1.f, 0.f, 0.f, 1.f), 0);
	}

	{
		m_MRT[(UINT)MRT_TYPE::MAP_DEFERRED_RENDER] = new MRT;
		
		Ptr<CTexture> arrRTTex[8] = {};
		arrRTTex[0] = CResMgr::GetInst()->CreateTexture(L"MapColorTargetTex", vMapResol.x, vMapResol.y
			, DXGI_FORMAT_R8G8B8A8_UNORM
			, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET);

		arrRTTex[1] = CResMgr::GetInst()->CreateTexture(L"MapNormalTargetTex", vMapResol.x, vMapResol.y
			, DXGI_FORMAT_R32G32B32A32_FLOAT
			, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET);

		arrRTTex[2] = CResMgr::GetInst()->CreateTexture(L"MapTangentTargetTex", vMapResol.x, vMapResol.y
			, DXGI_FORMAT_R32G32B32A32_FLOAT
			, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET);

		arrRTTex[3] = CResMgr::GetInst()->CreateTexture(L"MapPositionTargetTex", vMapResol.x, vMapResol.y
			, DXGI_FORMAT_R32G32B32A32_FLOAT
			, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET);

		arrRTTex[4] = CResMgr::GetInst()->CreateTexture(L"MapEmissiveTargetTex", vMapResol.x, vMapResol.y
			, DXGI_FORMAT_R8G8B8A8_UNORM
			, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET);

		arrRTTex[5] = CResMgr::GetInst()->CreateTexture(L"MapDataTargetTex", vMapResol.x, vMapResol.y
			, DXGI_FORMAT_R32G32B32A32_FLOAT
			, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET);

		m_MRT[(UINT)MRT_TYPE::MAP_DEFERRED_RENDER]->Create(arrRTTex, 6, nullptr);
		m_MRT[(UINT)MRT_TYPE::MAP_DEFERRED_RENDER]->SetClearColor(Vec4(43.f / 255.f, 76.f / 255.f, 93.f / 255.f, 1.f), 0);
	}
	{
		m_MRT[(UINT)MRT_TYPE::MAP_LINE] = new MRT;


		Ptr<CTexture> arrRTTex[8] = {};
		arrRTTex[0] = CResMgr::GetInst()->CreateTexture(L"OutlineMapTargetTex", vMapResol.x, vMapResol.y
			, DXGI_FORMAT_R8G8B8A8_UNORM
			, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE);

		m_MRT[(UINT)MRT_TYPE::MAP_LINE]->Create(arrRTTex, 1, nullptr);
		m_MRT[(UINT)MRT_TYPE::MAP_LINE]->SetClearColor(Vec4(1.f, 0.f, 1.f, 1.f), 0);
	}
	{
		m_MRT[(UINT)MRT_TYPE::MAP_RENDER] = new MRT;

		Ptr<CTexture> arrRTTex[8] = {};

		arrRTTex[0] = CResMgr::GetInst()->CreateTexture(L"MapRenderTex", vMapResol.x, vMapResol.y
			, DXGI_FORMAT_R8G8B8A8_UNORM
			, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE);

		Ptr<CTexture> pDSTex = CResMgr::GetInst()->CreateTexture(L"MapDepthStencilTex", vMapResol.x, vMapResol.y
			, DXGI_FORMAT_D24_UNORM_S8_UINT, D3D11_BIND_DEPTH_STENCIL, D3D11_USAGE_DEFAULT);

		m_MRT[(UINT)MRT_TYPE::MAP_RENDER]->Create(arrRTTex, 1, pDSTex);
		m_MRT[(UINT)MRT_TYPE::MAP_RENDER]->SetClearColor(Vec4(1.f, 0.f, 0.f, 1.f), 0);
	}
	{
		m_MRT[(UINT)MRT_TYPE::MAP_COLOR] = new MRT;

		Ptr<CTexture> arrRTTex[8] = {};

		arrRTTex[0] = CResMgr::GetInst()->FindRes<CTexture>(L"MapColorTargetTex");

		m_MRT[(UINT)MRT_TYPE::MAP_COLOR]->Create(arrRTTex, 1, nullptr);
		m_MRT[(UINT)MRT_TYPE::MAP_COLOR]->SetClearColor(Vec4(0.f, 0.f, 0.f, 1.f), 0);
	}
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