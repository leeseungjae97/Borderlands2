#include "pch.h"
#include "CCamera.h"


#include "CDevice.h"
#include "CRenderMgr.h"
#include "CTransform.h"

#include "CLevelMgr.h"
#include "CLevel.h"
#include "CLayer.h"
#include "CGameObject.h"
#include "CRenderComponent.h"
#include "CMaterial.h"
#include "CGraphicsShader.h"
#include "CLight3D.h"
#include "CResMgr.h"
#include "mMRT.h"


CCamera::CCamera()
	: CComponent(COMPONENT_TYPE::CAMERA)
	, m_fAspectRatio(1.f)
	, m_fScale(1.f)
	, m_Frustum(this)
	, m_ProjType(PROJ_TYPE::ORTHOGRAPHIC)
	, m_iLayerMask(0)
	, m_FOV(XM_PI / 2.f)
	, m_OrthoWidth(0.f)
	, m_OrthoHeight(0.f)
	, m_iCamIdx(-1)
    , m_NearZ(1.f)
    , m_FarZ(1000000.f)
	, m_bESM(false)
	, m_fT{}
{
	SetName(L"Camera");

	Vec2 vRenderResol = CDevice::GetInst()->GetRenderResolution();
	m_fAspectRatio = vRenderResol.x / vRenderResol.y;

	m_OrthoWidth = vRenderResol.x;
	m_OrthoHeight = vRenderResol.y;
}

CCamera::CCamera(const CCamera& _Other)
	: CComponent(_Other)
	, m_fAspectRatio(_Other.m_fAspectRatio)
	, m_Frustum(this)
	, m_fScale(_Other.m_fScale)
	, m_FOV(_Other.m_FOV)
	, m_OrthoWidth(_Other.m_OrthoWidth)
	, m_OrthoHeight(_Other.m_OrthoHeight)
	, m_ProjType(_Other.m_ProjType)
	, m_iLayerMask(_Other.m_iLayerMask)
	, m_bESM(_Other.m_bESM)
	, m_iCamIdx(-1)
{
}

CCamera::~CCamera()
{	
}

void CCamera::begin()
{
	if (-1 != m_iCamIdx)
	{
		CRenderMgr::GetInst()->RegisterCamera(this, m_iCamIdx);
	}
}

void CCamera::finaltick()
{
	CalcViewMat();

	CalcProjMat();

	m_Frustum.finaltick();
}

void CCamera::CalcViewMat()
{
	// ==============
	// View 행렬 계산
	// ==============
	m_matView = XMMatrixIdentity();

	// 카메라 좌표를 원점으로 이동
	Vec3 vCamPos = Transform()->GetRelativePos();
	Matrix matViewTrans = XMMatrixTranslation(-vCamPos.x, -vCamPos.y, -vCamPos.z);

	// 카메라가 바라보는 방향을 Z 축과 평행하게 만드는 회전 행렬을 적용
	Matrix matViewRot = XMMatrixIdentity();

	Vec3 vR = Transform()->GetWorldDir(DIR_TYPE::RIGHT);
	Vec3 vU = Transform()->GetWorldDir(DIR_TYPE::UP);
	Vec3 vF = Transform()->GetWorldDir(DIR_TYPE::FRONT);

	matViewRot._11 = vR.x;	matViewRot._12 = vU.x;	matViewRot._13 = vF.x;
	matViewRot._21 = vR.y;	matViewRot._22 = vU.y;	matViewRot._23 = vF.y;
	matViewRot._31 = vR.z;	matViewRot._32 = vU.z;	matViewRot._33 = vF.z;

	m_matView = matViewTrans * matViewRot;

	m_matViewInv = XMMatrixInverse(nullptr, m_matView);
}

void CCamera::CalcProjMat()
{
	// =============
	// 투영 행렬 계산
	// =============
	m_matProj = XMMatrixIdentity();
	
	if (PROJ_TYPE::ORTHOGRAPHIC == m_ProjType)
	{
		// 직교 투영
		m_matProj =  XMMatrixOrthographicLH(m_OrthoWidth * (1.f / m_fScale), m_OrthoHeight * (1.f / m_fScale), m_NearZ, m_FarZ);
	}
	else
	{	
		// 원근 투영
		m_matProj = XMMatrixPerspectiveFovLH(m_FOV, m_fAspectRatio, m_NearZ, m_FarZ);
	}
	m_matProjInv = XMMatrixInverse(nullptr, m_matProj);
}



void CCamera::SetLayerMask(int _iLayer, bool _Visible)
{
	if (_Visible)
	{
		m_iLayerMask |= 1 << _iLayer;
	}
	else
	{
		m_iLayerMask &= ~(1 << _iLayer);
	}
}

void CCamera::SetLayerMaskAll(bool _Visible)
{
	if (_Visible)
		m_iLayerMask = 0xffffffff;
	else
		m_iLayerMask = 0;
}

void CCamera::SetCameraIndex(int _idx)
{
	m_iCamIdx = _idx;
	CRenderMgr::GetInst()->RegisterCamera(this, m_iCamIdx);
}

void CCamera::SortObject()
{
	// 이전 프레임 분류정보 제거
	clear();

	// 현재 레벨 가져와서 분류
	CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurLevel();

	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		// 레이어 마스크 확인
		if (m_iLayerMask & (1 << i))
		{
			CLayer* pLayer = pCurLevel->GetLayer(i);
			const vector<CGameObject*>& vecObject = pLayer->GetObjects();

			for (size_t j = 0; j < vecObject.size(); ++j)
			{
				CRenderComponent* pRenderCom = vecObject[j]->GetRenderComponent();

				// 렌더링 기능이 없는 오브젝트는 제외
				if (   nullptr == pRenderCom 
					|| nullptr == pRenderCom->GetMaterial(0)
					|| nullptr == pRenderCom->GetMaterial(0)->GetShader())
					continue;

				//if (pRenderCom->IsFrustumCheck() 
				//	&& false == m_Frustum.FrustumCheck(vecObject[j]->Transform()->GetWorldPos(), vecObject[j]->Transform()->GetRelativeScale().x / 5.f))
				//	continue;

				// 쉐이더 도메인에 따른 분류
				SHADER_DOMAIN eDomain = pRenderCom->GetMaterial(0)->GetShader()->GetDomain();
				switch (eDomain)
				{
				case SHADER_DOMAIN::DOMAIN_DEFERRED:
					m_vecDeferred.push_back(vecObject[j]);
					break;
				case SHADER_DOMAIN::DOMAIN_DEFERRED_DECAL:
					m_vecDeferredDecal.push_back(vecObject[j]);
					break;
				case SHADER_DOMAIN::DOMAIN_OPAQUE:
					m_vecOpaque.push_back(vecObject[j]);
					break;
				case SHADER_DOMAIN::DOMAIN_MASK:
					m_vecMask.push_back(vecObject[j]);
					break;
				case SHADER_DOMAIN::DOMAIN_DECAL:
					m_vecDecal.push_back(vecObject[j]);
					break;
				case SHADER_DOMAIN::DOMAIN_TRANSPARENT:
					m_vecTransparent.push_back(vecObject[j]);
					break;
				case SHADER_DOMAIN::DOMAIN_POSTPROCESS:
					m_vecPost.push_back(vecObject[j]);
					break;
				case SHADER_DOMAIN::DOMAIN_UI:
					m_vecUI.push_back(vecObject[j]);
					break;				
				}
			}
		}
	}
}

void CCamera::SortObject_Shadow()
{
	// 이전 프레임 분류정보 제거
	clear_shadow();

	// 현재 레벨 가져와서 분류
	CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurLevel();

	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		// 레이어 마스크 확인
		if (m_iLayerMask & (1 << i))
		{
			CLayer* pLayer = pCurLevel->GetLayer(i);
			const vector<CGameObject*>& vecObject = pLayer->GetObjects();

			for (size_t j = 0; j < vecObject.size(); ++j)
			{
				CRenderComponent* pRenderCom = vecObject[j]->GetRenderComponent();

				// 렌더링 기능이 없는 오브젝트는 제외
				if (   nullptr == pRenderCom 
					|| nullptr == pRenderCom->GetMaterial(0)
					|| nullptr == pRenderCom->GetMaterial(0)->GetShader())
					continue;

				m_vecShadow.push_back(vecObject[j]);
			}
		}
	}
}

void CCamera::SortObject_Stencil()
{
		// 이전 프레임 분류정보 제거
	clear_stencil();

	// 현재 레벨 가져와서 분류
	CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurLevel();

	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		// 레이어 마스크 확인
		if (m_iLayerMask & (1 << i))
		{
			CLayer* pLayer = pCurLevel->GetLayer(i);
			const vector<CGameObject*>& vecObject = pLayer->GetObjects();

			for (size_t j = 0; j < vecObject.size(); ++j)
			{
				CRenderComponent* pRenderCom = vecObject[j]->GetRenderComponent();

				// 렌더링 기능이 없는 오브젝트는 제외
				if (   nullptr == pRenderCom 
					|| nullptr == pRenderCom->GetMaterial(0)
					|| nullptr == pRenderCom->GetMaterial(0)->GetShader())
					continue;

				m_vecStencil.push_back(vecObject[j]);
			}
		}
	}
}

void CCamera::SortObject_StencilDeploy()
{
	clear_stencildeploy();

	// 현재 레벨 가져와서 분류
	CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurLevel();

	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		// 레이어 마스크 확인
		if (m_iLayerMask & (1 << i))
		{
			CLayer* pLayer = pCurLevel->GetLayer(i);
			const vector<CGameObject*>& vecObject = pLayer->GetObjects();

			for (size_t j = 0; j < vecObject.size(); ++j)
			{
				CRenderComponent* pRenderCom = vecObject[j]->GetRenderComponent();

				// 렌더링 기능이 없는 오브젝트는 제외
				if (   nullptr == pRenderCom 
					|| nullptr == pRenderCom->GetMaterial(0)
					|| nullptr == pRenderCom->GetMaterial(0)->GetShader())
					continue;

				m_vecStencilDeploy.push_back(vecObject[j]);
			}
		}
	}
}

void CCamera::render()
{
	// 행렬 업데이트
	g_transform.matView = m_matView;
	g_transform.matViewInv = m_matViewInv;
	g_transform.matProj = m_matProj;
	g_transform.matProjInv = m_matProjInv;

	// 스텐실 설정

	//CRenderMgr::GetInst()->GetMRT(MRT_TYPE::STENCIL_CULL)->OMSet();
	//render_stencilcull();

	//CRenderMgr::GetInst()->GetMRT(MRT_TYPE::STENCIL_CULL_DEPLOY)->OMSet();
	//render_stencildeploy();

	//pMtrl2->GetShader()->SetDSType(DS_TYPE::STENCIL_CULL_DEPLOY);
	//pMtrl2->UpdateData();
	//pRectMesh2->render();

	// Deferred MRT
	CRenderMgr::GetInst()->GetMRT(MRT_TYPE::DEFERRED)->OMSet(true);
	render_deferred();

	// Deferred Light
	CRenderMgr::GetInst()->GetMRT(MRT_TYPE::LIGHT)->OMSet(false);
	const vector<CLight3D*>& vecLight3D = CRenderMgr::GetInst()->GetLight3D();
	for (size_t i = 0; i < vecLight3D.size(); ++i)
	{
		vecLight3D[i]->render();
	}

	// SwapChain MRT 로 변경
	CRenderMgr::GetInst()->GetMRT(MRT_TYPE::SWAPCHAIN)->OMSet();

	static Ptr<CMesh> pRectMesh = CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh");
	static Ptr<CMaterial> pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"MergeMtrl");

	static bool bSet = false;
	if (!bSet)
	{
		bSet = true;
		pMtrl->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"ColorTargetTex"));
		pMtrl->SetTexParam(TEX_1, CResMgr::GetInst()->FindRes<CTexture>(L"DiffuseTargetTex"));
		pMtrl->SetTexParam(TEX_2, CResMgr::GetInst()->FindRes<CTexture>(L"SpecularTargetTex"));
		pMtrl->SetTexParam(TEX_3, CResMgr::GetInst()->FindRes<CTexture>(L"EmissiveTargetTex"));
		pMtrl->SetTexParam(TEX_4, CResMgr::GetInst()->FindRes<CTexture>(L"ShadowTargetTex"));
		pMtrl->SetScalarParam(FLOAT_0, &m_fT[0]);
	}

	pMtrl->UpdateData();
	pRectMesh->render(0);


	// Deferred MRT 에 그린 물체들을 다시 SwapChain 으로 옮기기

	// 쉐이더 도메인에 따라서 순차적으로 그리기
	render_opaque();
	render_mask();
	render_decal();
	render_transparent();

	// PostProcess - 후처리
	render_postprocess();

	// UI
	render_ui();
}


void CCamera::clear()
{
	m_vecDeferred.clear();
	m_vecDeferredDecal.clear();

	m_vecOpaque.clear();
	m_vecMask.clear();
	m_vecDecal.clear();
	m_vecTransparent.clear();
	m_vecPost.clear();
	m_vecUI.clear();
}

void CCamera::clear_shadow()
{
	m_vecShadow.clear();
}

void CCamera::clear_stencil()
{
	m_vecStencil.clear();
}

void CCamera::clear_stencildeploy()
{
	m_vecStencilDeploy.clear();
}

void CCamera::render_deferred()
{
	for (size_t i = 0; i < m_vecDeferred.size(); ++i)
	{
		m_vecDeferred[i]->render();
	}

	CRenderMgr::GetInst()->GetMRT(MRT_TYPE::DEFERRED_DECAL)->OMSet();

	for (size_t i = 0; i < m_vecDeferredDecal.size(); ++i)
	{
		m_vecDeferredDecal[i]->render();
	}
}

void CCamera::render_shadowmap()
{
	g_transform.matView = m_matView;
	g_transform.matProj = m_matProj;

	for (size_t i = 0; i < m_vecShadow.size(); ++i)
	{
		m_vecShadow[i]->SetESM(m_bESM);
		m_vecShadow[i]->render_shadowmap();
	}
}

void CCamera::render_stencilcull()
{
	for (size_t i = 0; i < m_vecStencil.size(); ++i)
	{
		m_vecStencilDeploy[i]->render_stencilcull();
	}
}

void CCamera::render_stencildeploy()
{
		for (size_t i = 0; i < m_vecStencil.size(); ++i)
	{
		m_vecStencilDeploy[i]->render_stencildeploy();
	}
}

void CCamera::render_opaque()
{
	for (size_t i = 0; i < m_vecOpaque.size(); ++i)
	{
		m_vecOpaque[i]->render();
	}
}

void CCamera::render_mask()
{
	for (size_t i = 0; i < m_vecMask.size(); ++i)
	{
		m_vecMask[i]->render();
	}
}

void CCamera::render_decal()
{
	for (size_t i = 0; i < m_vecDecal.size(); ++i)
	{
		m_vecDecal[i]->render();
	}
}

void CCamera::render_transparent()
{
	for (size_t i = 0; i < m_vecTransparent.size(); ++i)
	{
		m_vecTransparent[i]->render();
	}
}

void CCamera::render_postprocess()
{
	for (size_t i = 0; i < m_vecPost.size(); ++i)
	{
		CRenderMgr::GetInst()->CopyRenderTarget();
		m_vecPost[i]->render();
	}
}

void CCamera::render_ui()
{
	for (size_t i = 0; i < m_vecUI.size(); ++i)
	{
		m_vecUI[i]->render();
	}
}

void CCamera::SaveToLevelFile(FILE* _File)
{
	fwrite(&m_fAspectRatio, sizeof(float), 1, _File);
	fwrite(&m_fScale, sizeof(float), 1, _File);
	fwrite(&m_ProjType, sizeof(UINT), 1, _File);
	fwrite(&m_iLayerMask, sizeof(UINT), 1, _File);
	fwrite(&m_iCamIdx, sizeof(int), 1, _File);
}

void CCamera::LoadFromLevelFile(FILE* _File)
{
	fread(&m_fAspectRatio, sizeof(float), 1, _File);
	fread(&m_fScale, sizeof(float), 1, _File);
	fread(&m_ProjType, sizeof(UINT), 1, _File);
	fread(&m_iLayerMask, sizeof(UINT), 1, _File);
	fread(&m_iCamIdx, sizeof(int), 1, _File);
}
