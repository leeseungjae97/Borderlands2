#include "CCamera.h"

#include "CameraMgr.h"
#include "CAnimator2D.h"
#include "CAnimator3D.h"
#include "CConstBuffer.h"
#include "CDevice.h"
#include "CEngine.h"
#include "CRenderMgr.h"
#include "CTransform.h"

#include "CLevelMgr.h"
#include "CLevel.h"
#include "CLayer.h"
#include "CGameObject.h"
#include "CRenderComponent.h"
#include "CMaterial.h"
#include "CGraphicsShader.h"
#include "CInstancingBuffer.h"
#include "InstancingAnimatorMgr.h"
#include "CKeyMgr.h"
#include "CLight3D.h"
#include "CMeshRender.h"
#include "CResMgr.h"
#include "CTimeMgr.h"
#include "FieldUIMgr.h"
#include "mMRT.h"
#include <unordered_set>


CCamera::CCamera()
	: CComponent(COMPONENT_TYPE::CAMERA)
	, m_fAspectRatio(1.f)
	, m_fScale(1.f)
	, m_Frustum(this)
	, m_ProjType(PROJ_TYPE::ORTHOGRAPHIC)
	, m_iLayerMask(0)
	, m_FOV(0.7f)
	, m_OrthoWidth(0.f)
	, m_OrthoHeight(0.f)
	, m_iCamIdx(-1)
	, m_NearZ(1.f)
	, m_FarZ(1000000.f)
	, m_fT{}
	, m_Outline(true)
	, m_fFadeAcc(0.0f)
	, m_fFadeTime(0.0f)
	, m_bFade(false)
	, m_ray{}
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
	, m_fFadeAcc(0.0f)
	, m_fFadeTime(0.0f)
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
		CameraMgr::GetInst()->AddCamObj(GetOwner()->GetName(), GetOwner());
	}
}

void CCamera::finaltick()
{
	CalcViewMat();
	CalcProjMat();
	m_Frustum.finaltick();
	CalRay();
}

void CCamera::CalcViewMat()
{
	m_matView = XMMatrixIdentity();

	Vec3 vCamPos = Transform()->GetRelativePos();
	Matrix matViewTrans = XMMatrixTranslation(-vCamPos.x, -vCamPos.y, -vCamPos.z);

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
	m_matProj = XMMatrixIdentity();

	if (PROJ_TYPE::ORTHOGRAPHIC == m_ProjType)
	{
		m_matProj = XMMatrixOrthographicLH(m_OrthoWidth * (1.f / m_fScale), m_OrthoHeight * (1.f / m_fScale), m_NearZ, m_FarZ);
	}
	else
	{
		m_matProj = XMMatrixPerspectiveFovLH(m_FOV, m_fAspectRatio, m_NearZ, m_FarZ);
	}
	m_matProjInv = XMMatrixInverse(nullptr, m_matProj);
}

void CCamera::UpdateMatrix()
{
	g_transform.matView = m_matView;
	g_transform.matViewInv = m_matViewInv;
	g_transform.matProj = m_matProj;
	g_transform.matProjInv = m_matProjInv;
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

void CCamera::CalRay()
{
	MRT* pMRT = CRenderMgr::GetInst()->GetMRT(MRT_TYPE::SWAPCHAIN);
	D3D11_VIEWPORT tVP = pMRT->GetViewPort();

	Vec2 vMousePos = CKeyMgr::GetInst()->GetMousePos();

	m_ray.vStart = Transform()->GetWorldPos();

	m_ray.vDir.x = ((((vMousePos.x - tVP.TopLeftX) * 2.f / tVP.Width) - 1.f) - m_matProj._31) / m_matProj._11;
	m_ray.vDir.y = (-(((vMousePos.y - tVP.TopLeftY) * 2.f / tVP.Height) - 1.f) - m_matProj._32) / m_matProj._22;
	m_ray.vDir.z = 1.f;

	m_ray.vDir = XMVector3TransformNormal(m_ray.vDir, m_matViewInv);
	m_ray.vDir.Normalize();
}

void CCamera::SortObject()
{
	clear();
	CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurLevel();
	if (!pCurLevel) return;
	const bool bEditorMode = (pCurLevel->GetState() != LEVEL_STATE::PLAY);

    // 프레임별 중복 등록 방지를 위한 맵 (Animator -> InstIdx)
    static std::unordered_map<CAnimator3D*, int> mapAnimIdx;
	static std::unordered_set<CAnimator3D*> s_editorRandomPlayed;
    mapAnimIdx.clear();
	if (!bEditorMode)
	{
		s_editorRandomPlayed.clear();
	}

	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		if (!(m_iLayerMask & (1 << i))) continue;

		CLayer* pLayer = pCurLevel->GetLayer(i);
		const vector<CGameObject*>& vecObject = pLayer->GetObjects();

		for (CGameObject* pObj : vecObject)
		{
			if (!IsValidToRender(pObj)) continue;

			CRenderComponent* pRenderCom = pObj->GetRenderComponent();

			UINT iMtrlCount = pRenderCom->GetMtrlCount();
			for (UINT iMtrl = 0; iMtrl < iMtrlCount; ++iMtrl)
			{
				Ptr<CMaterial> pMtrl = pRenderCom->GetMaterial(iMtrl);
				if (nullptr == pMtrl || nullptr == pMtrl->GetShader()) continue;
				const SHADER_DOMAIN eDomain = pMtrl->GetShader()->GetDomain();

                // 이미 등록된 애니메이터인지 확인
                int iAnimIdx = -1;
                if (pObj->Animator3D())
                {
                    auto it = mapAnimIdx.find(pObj->Animator3D());
                    if (it == mapAnimIdx.end())
                    {
                        iAnimIdx = InstancingAnimatorMgr::GetInst()->AddAnimator3D(pObj->Animator3D());
                        mapAnimIdx[pObj->Animator3D()] = iAnimIdx;
                    }
                    else
                    {
                        iAnimIdx = it->second;
                    }

					if (bEditorMode
						&& (eDomain == SHADER_DOMAIN::DOMAIN_DEFERRED
							|| eDomain == SHADER_DOMAIN::DOMAIN_OPAQUE
							|| eDomain == SHADER_DOMAIN::DOMAIN_MASK)
						&& s_editorRandomPlayed.find(pObj->Animator3D()) == s_editorRandomPlayed.end())
					{
						const auto& clips = pObj->Animator3D()->GetAnimClips();
						if (!clips.empty())
						{
							const int randomIndex = rand() % (int)clips.size();
							auto clipIter = clips.begin();
							std::advance(clipIter, randomIndex);
							pObj->Animator3D()->Play(clipIter->first, true);
							s_editorRandomPlayed.insert(pObj->Animator3D());
						}
					}
                }

				ClassifyByDomain(pObj, pRenderCom, pMtrl, iMtrl, iAnimIdx);
			}
		}
	}
}

bool CCamera::IsValidToRender(CGameObject* _pObj)
{
	if (!_pObj || _pObj->GetObjectState() == CGameObject::OBJECT_STATE::INVISIBLE)
		return false;

	if (nullptr == _pObj->GetRenderComponent())
	{
		return false;
	}

	return true;
}

void CCamera::ClassifyByDomain(CGameObject* _pObj, CRenderComponent* _pRC, Ptr<CMaterial> _pMtrl, UINT _iMtrlIdx, int _iAnimIdx)
{
	SHADER_DOMAIN eDomain = _pMtrl->GetShader()->GetDomain();

	switch (eDomain)
	{
	case SHADER_DOMAIN::DOMAIN_DEFERRED:
	case SHADER_DOMAIN::DOMAIN_OPAQUE:
	case SHADER_DOMAIN::DOMAIN_MASK:
	{
		auto& targetMap = (eDomain == SHADER_DOMAIN::DOMAIN_DEFERRED) ? m_mapInstGroup_D : m_mapInstGroup_F;

		ULONG64 llID = _pRC->GetInstID(_iMtrlIdx);
		if (0 == llID) return;

		targetMap[llID].push_back(tInstObj{ _pObj, _iMtrlIdx, _iAnimIdx });
	}
	break;

	case SHADER_DOMAIN::DOMAIN_DEFERRED_DECAL: m_vecDeferredDecal.push_back(_pObj); break;
	case SHADER_DOMAIN::DOMAIN_DECAL:          m_vecDecal.push_back(_pObj);          break;
	case SHADER_DOMAIN::DOMAIN_TRANSPARENT:    m_vecTransparent.push_back(_pObj);    break;
	case SHADER_DOMAIN::DOMAIN_POSTPROCESS:    m_vecPost.push_back(_pObj);           break;
	case SHADER_DOMAIN::DOMAIN_UI:             m_vecUI.push_back(_pObj);             break;
	}
}

void CCamera::SortObject_Shadow()
{
	clear_shadow();

	CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurLevel();

	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		if (i == (int)LAYER_TYPE::Camera) continue;
		if (i == (int)LAYER_TYPE::Default) continue; 
		if (i == (int)LAYER_TYPE::ViewPortUI) continue;

		if (m_iLayerMask & (1 << i))
		{
			CLayer* pLayer = pCurLevel->GetLayer(i);
			const vector<CGameObject*>& vecObject = pLayer->GetObjects();

			for (size_t j = 0; j < vecObject.size(); ++j)
			{
				CRenderComponent* pRenderCom = vecObject[j]->GetRenderComponent();

				if (vecObject[j]->GetObjectState() == CGameObject::OBJECT_STATE::INVISIBLE)
					continue;

				if (vecObject[j]->Camera())
					continue;

				if (nullptr == pRenderCom
					|| nullptr == pRenderCom->GetMaterial(0)
					|| nullptr == pRenderCom->GetMaterial(0)->GetShader())
					continue;

				m_vecShadow.push_back(vecObject[j]);
			}
		}
	}
}

void CCamera::render()
{
	CInstancingBuffer::GetInst()->Clear();
	InstancingAnimatorMgr::GetInst()->BeginFrame();

	UpdateMatrix();

	CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurLevel();
	if (pCurLevel->GetState() == LEVEL_STATE::NO_UPDATE_RENDER)
		return;

	static Ptr<CMaterial>	pMerge = CResMgr::GetInst()->FindRes<CMaterial>(L"MergeMtrl");
	static Ptr<CMaterial>	pLaplacian = CResMgr::GetInst()->FindRes<CMaterial>(L"LaplacianMtrl");
	static Ptr<CMaterial>	pFade = CResMgr::GetInst()->FindRes<CMaterial>(L"FadeMtrl");

	SortObject();

	InstancingAnimatorMgr::GetInst()->SetData();

	switch (m_iCamIdx)
	{
	case 0:
		RenderMain(pMerge, pFade, pLaplacian);
		break;
	case 1:
		RenderMinimap(pLaplacian);
		break;
	case 2:
		RenderScope(pMerge);
		break;
	case 3:
		RenderUI();
		break;

	}

	for (auto& pair : m_mapInstGroup_F)
	{
		ClearThisFrameRenderInfo(pair);
	}
	for (auto& pair : m_mapInstGroup_D)
	{
		ClearThisFrameRenderInfo(pair);
	}
}

void CCamera::RenderScope(Ptr<CMaterial>& pMerge)
{
	CRenderMgr::GetInst()->GetMRT(MRT_TYPE::SCOPE_RENDER)->OMSet();
	CRenderMgr::GetInst()->GetMRT(MRT_TYPE::SCOPE_DEFERRED_RENDER)->OMSet(true);

	CRenderMgr::GetInst()->GetMRT(MRT_TYPE::SCOPE_LIGHT_RENDER)->OMSet(false);
	const vector<CLight3D*>& vecLight3D = CRenderMgr::GetInst()->GetLight3D();
	for (size_t i = 0; i < vecLight3D.size(); ++i)
	{
		vecLight3D[i]->render(m_iCamIdx);
	}
	CRenderMgr::GetInst()->GetMRT(MRT_TYPE::SCOPE_RENDER)->OMSet();

	pMerge->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"ScopeColorTargetTex"));
	pMerge->SetTexParam(TEX_1, CResMgr::GetInst()->FindRes<CTexture>(L"ScopeDiffuseTargetTex"));
	pMerge->SetTexParam(TEX_2, CResMgr::GetInst()->FindRes<CTexture>(L"ScopeEmissiveTargetTex"));
	pMerge->SetTexParam(TEX_3, CResMgr::GetInst()->FindRes<CTexture>(L"ScopeSpecularTargetTex"));
	pMerge->SetTexParam(TEX_4, CResMgr::GetInst()->FindRes<CTexture>(L"ScopeShadowTargetTex"));

	render_screen_quad(pMerge);

	CRenderMgr::GetInst()->GetMRT(MRT_TYPE::SCOPE_RENDER)->OMSet();

	render_forward();
	render_decal();
	render_transparent();
	render_postprocess();

	CRenderMgr::GetInst()->CopyRenderTarget();
	CRenderMgr::GetInst()->GetMRT(MRT_TYPE::SWAPCHAIN)->OMSet();
}

void CCamera::RenderMinimap(Ptr<CMaterial>& pLaplacian)
{
	CRenderMgr::GetInst()->GetMRT(MRT_TYPE::MAP_RENDER)->OMSet();
	CRenderMgr::GetInst()->GetMRT(MRT_TYPE::MAP_DEFERRED_RENDER)->OMSet(true);

	CRenderMgr::GetInst()->GetMRT(MRT_TYPE::MAP_RENDER)->OMSet();
	pLaplacian->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"MapNormalTargetTex"));
	pLaplacian->SetTexParam(TEX_1, CResMgr::GetInst()->FindRes<CTexture>(L"MapColorTargetTex"));

	int branch = 1;
	pLaplacian->SetScalarParam(INT_0, &branch);
	render_screen_quad(pLaplacian);

	render_map_marker();

	CRenderMgr::GetInst()->CopyRenderTarget();
	CRenderMgr::GetInst()->GetMRT(MRT_TYPE::SWAPCHAIN)->OMSet();
}

void CCamera::RenderUI()
{
	CRenderMgr::GetInst()->GetMRT(MRT_TYPE::SWAPCHAIN)->OMSet();

	render_ui();
	render_text();

	CRenderMgr::GetInst()->GetMRT(MRT_TYPE::SWAPCHAIN)->OMSet();
}

void CCamera::RenderMain(Ptr<CMaterial>& pMerge, Ptr<CMaterial>& pFade, Ptr<CMaterial>& pLaplacian)
{
    // MRT 설정 전 텍스처 슬롯 비우기 (Slot 0-7)
    for (UINT i = 0; i < 8; ++i) {
        ID3D11ShaderResourceView* pNullSRV = nullptr;
        CONTEXT->PSSetShaderResources(i, 1, &pNullSRV);
    }

	CRenderMgr::GetInst()->GetMRT(MRT_TYPE::DEFERRED)->OMSet();

	render_deferred();

    // 2. 라이트 패스 전 G-Buffer를 SRV로 바인딩하기 위해 MRT 해제
	CRenderMgr::GetInst()->GetMRT(MRT_TYPE::LIGHT)->OMSet(false);
	const vector<CLight3D*>& vecLight3D = CRenderMgr::GetInst()->GetLight3D();
	for (size_t i = 0; i < vecLight3D.size(); ++i)
	{
		vecLight3D[i]->render(m_iCamIdx);
	}
	post_process_bloom();

	CRenderMgr::GetInst()->GetMRT(MRT_TYPE::HDR)->OMSet();
	render_merge(pMerge);

	CRenderMgr::GetInst()->GetMRT(MRT_TYPE::HDR_LINE)->OMSet();
	post_process_hdr(pLaplacian);

	CRenderMgr::GetInst()->GetMRT(MRT_TYPE::BLOOMED_HDR)->OMSet();
	post_process_emissive();

	CRenderMgr::GetInst()->GetMRT(MRT_TYPE::SWAPCHAIN)->OMSet();
	post_process_bloom_hdr();

	render_forward();
	render_decal();
	render_transparent();
	render_postprocess();

	CRenderMgr::GetInst()->CopyRenderTarget();

	if (m_bFade)
	{
		post_process_fade(pFade);
	}
}

void CCamera::post_process_fade(Ptr<CMaterial>& pFade)
{
	pFade->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"RTCopyTex3"));

	float fadeRatio = (m_fFadeAcc / m_fFadeTime);

	m_fFadeAcc += DT;

	if (m_bFadeIn)
		fadeRatio = 1.f - fadeRatio;

	pFade->SetScalarParam(FLOAT_0, &fadeRatio);
	render_screen_quad(pFade);


	if (m_fFadeTime < m_fFadeAcc)
	{
		m_fFadeAcc = m_fFadeTime;
		m_fFadeAcc = 0.f;
		m_bFade = false;
	}
}

void CCamera::post_process_bloom_hdr()
{
	static Ptr<CMaterial>	pToneMapping = CResMgr::GetInst()->FindRes<CMaterial>(L"ToneMappingMtrl");

	int m = 1;
	pToneMapping->SetScalarParam(INT_0, &m);
	pToneMapping->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"BloomedHDRTargetTex"));
	render_screen_quad(pToneMapping);
}

void CCamera::post_process_emissive()
{
	static Ptr<CMaterial>	pEmissiveBloom = CResMgr::GetInst()->FindRes<CMaterial>(L"BloomMtrl");

	pEmissiveBloom->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"OutlineHDRTargetTex"));
	pEmissiveBloom->SetTexParam(TEX_1, CResMgr::GetInst()->FindRes<CTexture>(L"EmissiveVerticalBlurredTargetTex"));
	pEmissiveBloom->SetTexParam(TEX_2, CResMgr::GetInst()->FindRes<CTexture>(L"EmissiveTargetTex"));
	render_screen_quad(pEmissiveBloom);
}

void CCamera::post_process_hdr(Ptr<CMaterial>& pLaplacian)
{
	pLaplacian->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"NormalTargetTex"));
	pLaplacian->SetTexParam(TEX_1, CResMgr::GetInst()->FindRes<CTexture>(L"HDRTargetTex"));
	int branch = 0;
	pLaplacian->SetScalarParam(INT_0, &branch);
	render_screen_quad(pLaplacian);
}

void CCamera::render_merge(Ptr<CMaterial>& pMerge)
{
	if (nullptr == pMerge) return;

	Ptr<CTexture> pTex0 = CResMgr::GetInst()->FindRes<CTexture>(L"ColorTargetTex");
	Ptr<CTexture> pTex1 = CResMgr::GetInst()->FindRes<CTexture>(L"DiffuseTargetTex");
	Ptr<CTexture> pTex2 = CResMgr::GetInst()->FindRes<CTexture>(L"EmissiveTargetTex");
	Ptr<CTexture> pTex3 = CResMgr::GetInst()->FindRes<CTexture>(L"SpecularTargetTex");
	Ptr<CTexture> pTex4 = CResMgr::GetInst()->FindRes<CTexture>(L"ShadowTargetTex");

	pMerge->SetTexParam(TEX_0, pTex0);
	pMerge->SetTexParam(TEX_1, pTex1);
	pMerge->SetTexParam(TEX_2, pTex2);
	pMerge->SetTexParam(TEX_3, pTex3);
	pMerge->SetTexParam(TEX_4, pTex4);

	render_screen_quad(pMerge);
}

void CCamera::post_process_bloom()
{
	static Ptr<CMaterial>	pBlurV = CResMgr::GetInst()->FindRes<CMaterial>(L"BlurVMtrl");
	static Ptr<CMaterial>	pBlurH = CResMgr::GetInst()->FindRes<CMaterial>(L"BlurHMtrl");

	CRenderMgr::GetInst()->GetMRT(MRT_TYPE::BLUR_H)->OMSet();
	pBlurV->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"EmissiveTargetTex"));
	render_screen_quad(pBlurV);

	CRenderMgr::GetInst()->GetMRT(MRT_TYPE::BLUR_V)->OMSet();
	pBlurH->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"EmissiveTargetTex"));
	pBlurH->SetTexParam(TEX_1, CResMgr::GetInst()->FindRes<CTexture>(L"EmissiveHorizontalBlurredTargetTex"));
	render_screen_quad(pBlurH);
}

void CCamera::render_screen_quad(Ptr<CMaterial>& pMtrl)
{
	static Ptr<CMesh> pScreenMesh = CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh");

	pMtrl->UpdateData();
	pScreenMesh->render(0);
}

void CCamera::ClearThisFrameRenderInfo(std::pair<const ULONG64, std::vector<tInstObj>>& pair)
{
	auto& vecInstObjs = pair.second;
	if (vecInstObjs.empty()) return;

	CGameObject* pFirstObj = vecInstObjs[0].pObj;
	if (!pFirstObj || !pFirstObj->GetRenderComponent()) return;

	Ptr<CMesh> pMesh = pFirstObj->GetRenderComponent()->GetMesh();
	Ptr<CMaterial> pMtrl = pFirstObj->GetRenderComponent()->GetMaterial(vecInstObjs[0].iMtrlIdx);

	if (nullptr == pMesh || nullptr == pMtrl) return;

	bool bHasAnim3D = false;

	for (const auto& instObj : vecInstObjs)
	{
		CGameObject* pObj = instObj.pObj;
		if (!pObj) continue;

		if (pObj->Animator3D())
		{
			bHasAnim3D = true;
			break;
		}
	}

	if (bHasAnim3D)
	{
		pMtrl->SetAnim3D(false);
		pMtrl->SetBoneCount(0);
	}
}


void CCamera::InsertInstancingData(std::pair<const ULONG64, std::vector<tInstObj>>& pair, tInstancingData& tInstData)
{
	auto& vecInstObjs = pair.second;
	if (vecInstObjs.empty()) return;

	CGameObject* pFirstObj = vecInstObjs[0].pObj;
	if (!pFirstObj || !pFirstObj->GetRenderComponent()) return;

	Ptr<CMesh> pMesh = pFirstObj->GetRenderComponent()->GetMesh();
	Ptr<CMaterial> pMtrl = pFirstObj->GetRenderComponent()->GetMaterial(vecInstObjs[0].iMtrlIdx);

	if (nullptr == pMesh || nullptr == pMtrl) return;

	CInstancingBuffer* pInstBuffer = CInstancingBuffer::GetInst();

	bool bHasAnim3D = false;

	for (const auto& instObj : vecInstObjs)
	{
		CGameObject* pObj = instObj.pObj;
		if (!pObj) continue;

		tInstData.matWorld = pObj->Transform()->GetWorldMat();
		tInstData.matWV = tInstData.matWorld * m_matView;
		tInstData.matWVP = tInstData.matWV * m_matProj;

		if (pObj->Animator3D() && pObj->Animator3D()->GetCurAnimClip() != nullptr && instObj.iAnimInstIdx >= 0)
		{
			tInstData.iRowIdx = instObj.iAnimInstIdx;
			bHasAnim3D = true;
		}
		else
		{
			tInstData.iRowIdx = -1;
		}

		pInstBuffer->AddInstancingData(tInstData);
	}

	if (bHasAnim3D)
	{
		pMtrl->SetAnim3D(true);
		pMtrl->SetBoneCount(pMesh->GetBoneCount());
	}
	else
	{
		pMtrl->SetAnim3D(false);
		pMtrl->SetBoneCount(0);
	}
}

void CCamera::RenderInstancingData(std::pair<const ULONG64, std::vector<tInstObj>>& pair)
{
	auto& vecInstObjs = pair.second;
	if (vecInstObjs.empty()) return;

	CGameObject* pFirstObj = vecInstObjs[0].pObj;
	if (!pFirstObj || !pFirstObj->GetRenderComponent()) return;

	Ptr<CMesh> pMesh = pFirstObj->GetRenderComponent()->GetMesh();
	Ptr<CMaterial> pMtrl = pFirstObj->GetRenderComponent()->GetMaterial(vecInstObjs[0].iMtrlIdx);

	if (nullptr == pMesh || nullptr == pMtrl) return;

    // 전역 본 버퍼 바인딩 (개별 객체 렌더링에 의해 덮어씌워졌을 가능성 차단)
    if (auto* pBone = InstancingAnimatorMgr::GetInst()->GetFinalMatBuffer())
    {
        pBone->UpdateData(30, PIPELINE_STAGE::PS_VERTEX);
    }

	pMtrl->UpdateData_Instancing();
	pMesh->render_instancing(vecInstObjs[0].iMtrlIdx);
}

void CCamera::InsertSingleData(std::pair<const ULONG64, std::vector<tInstObj>>& _pair)
{
	auto& vecInstObjs = _pair.second;
	if (vecInstObjs.empty()) return;

	CGameObject* pObj = vecInstObjs[0].pObj;
	if (!pObj || !pObj->GetRenderComponent()) return;

	bool bNeedSingleDraw = false;

	if (pObj->Animator3D())
	{
		bNeedSingleDraw = false;
	}
	else if (vecInstObjs.size() <= 2 || pObj->Animator2D())
	{
		bNeedSingleDraw = true;
	}
	else
	{
		Ptr<CMaterial> pMtrl = pObj->GetRenderComponent()->GetMaterial(vecInstObjs[0].iMtrlIdx);
		if (pMtrl == nullptr || pMtrl->GetShader()->GetVSInst() == nullptr)
		{
			bNeedSingleDraw = true;
		}
	}
	if (bNeedSingleDraw)
	{
		for (const auto& instObj : vecInstObjs)
		{
			m_mapSingleObj[(INT_PTR)instObj.pObj].push_back(instObj);
		}
	}
}

void CCamera::clear_shadow()
{
	m_vecShadow.clear();
}
void CCamera::clear()
{
	for (auto& pair : m_mapInstGroup_D)
	{
		if(!pair.second.empty())
			pair.second.clear();
	}
	for (auto& pair : m_mapInstGroup_F)
	{
		if (!pair.second.empty())
			pair.second.clear();
	}
	m_mapInstGroup_D.clear();
	m_mapInstGroup_F.clear();
	m_mapSingleObj.clear();

	m_vecDeferredDecal.clear();

	m_vecOpaque.clear();
	m_vecMask.clear();
	m_vecDecal.clear();
	m_vecTransparent.clear();
	m_vecPost.clear();
	m_vecUI.clear();
	m_vecText.clear();
}

void CCamera::render_map_marker()
{
	UpdateMatrix();

	CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurLevel();
	CLayer* pEnemyLayer = pCurLevel->GetLayer((UINT)LAYER_TYPE::Enemy);

	vector<CGameObject*> enemies = pEnemyLayer->GetParentObject();

	static Ptr<CMaterial> pMarkerMaterial = new CMaterial;
	pMarkerMaterial->SetShader(CResMgr::GetInst()->FindRes<CGraphicsShader>(L"UI2DShader").Get());
	pMarkerMaterial->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"texture\\UI\\enemy_marker.png"));

	Ptr<CMesh> pMarkerMesh = CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh");

	CConstBuffer* pTransformBuffer = CDevice::GetInst()->GetConstBuffer(CB_TYPE::TRANSFORM);
	

	Vec3 vPos = Vec3::Zero;
	CTransform* pTrans = nullptr;

	Vec3 vCamPos = Transform()->GetRelativePos();
	Vec3 vCamUp = Transform()->GetRelativeDir(DIR_TYPE::UP);

	for(int i =0 ; i < enemies.size(); ++i)
	{
		pTrans = enemies[i]->Transform();
		if (nullptr == pTrans)
			continue;

		vPos = enemies[i]->Transform()->GetRelativePos();
		vPos.y += 10000.f;

		Matrix matSc = XMMatrixScaling(500.f, 500.f, 500.f);
		Matrix matWorld = matSc;
		matWorld *= Matrix::CreateConstrainedBillboard(vPos, vCamPos, vCamUp);

		g_transform.matWorld = matWorld;

		g_transform.matWorldInv = XMMatrixInverse(nullptr, g_transform.matWorld);
		g_transform.matWV = g_transform.matWorld * g_transform.matView;
		g_transform.matWVP = g_transform.matWV * g_transform.matProj;

		pTransformBuffer->SetData(&g_transform);
		pTransformBuffer->UpdateData();

		pMarkerMaterial->UpdateData();
		pMarkerMesh->render(0);

		pTrans = nullptr;
	}
}
void CCamera::RenderInstancedBatch(std::pair<const ULONG64, std::vector<tInstObj>>& pair)
{
	auto& vecInstObjs = pair.second;
	if (vecInstObjs.empty()) return;

	CInstancingBuffer* pInstBuffer = CInstancingBuffer::GetInst();
	pInstBuffer->Clear();

	tInstancingData tInstData = {};
	InsertInstancingData(pair, tInstData);

	if (pInstBuffer->GetInstanceCount() == 0)
		return;

	pInstBuffer->SetData();

	RenderInstancingData(pair);

	// Prevent stale instance count from carrying to later passes/cameras.
	pInstBuffer->Clear();
}

void CCamera::render_deferred()
{
	UpdateMatrix();

	m_mapSingleObj.clear();

	for (auto& pair : m_mapInstGroup_D)
	{
		if (pair.second.empty())
			continue;

		if (pair.second.size() <= 2)
		{
			InsertSingleData(pair);
			continue;
		}

		RenderInstancedBatch(pair);
	}
	
	for (auto& pair : m_mapSingleObj)
	{
		if (pair.second.empty())
			continue;


		for (auto& instObj : pair.second)
		{
			instObj.pObj->GetRenderComponent()->render(instObj.iMtrlIdx, true);
		}
	}
}
void CCamera::render_forward()
{
	UpdateMatrix();

	m_mapSingleObj.clear();

	for (auto& pair : m_mapInstGroup_F)
	{
		if (pair.second.empty())
			continue;

		if (pair.second.size() <= 2)
		{
			InsertSingleData(pair);
			continue;
		}

		RenderInstancedBatch(pair);
	}
	
	for (auto& pair : m_mapSingleObj)
	{
		if (pair.second.empty())
			continue;


		for (auto& instObj : pair.second)
		{
			instObj.pObj->GetRenderComponent()->render(instObj.iMtrlIdx, false);
		}
	}
}

void CCamera::render_shadowmap()
{
	UpdateMatrix();
}

void CCamera::render_decal()
{
	UpdateMatrix();

	for (size_t i = 0; i < m_vecDecal.size(); ++i)
	{
		m_vecDecal[i]->render();
	}
}

void CCamera::render_transparent()
{
	UpdateMatrix();

	for (size_t i = 0; i < m_vecTransparent.size(); ++i)
	{
		m_vecTransparent[i]->render();
	}
}

void CCamera::render_postprocess()
{
	UpdateMatrix();

	for (size_t i = 0; i < m_vecPost.size(); ++i)
	{

		m_vecPost[i]->render();
	}
}

void CCamera::render_ui()
{
	UpdateMatrix();

	for (size_t i = 0; i < m_vecUI.size(); ++i)
	{
		m_vecUI[i]->render();
	}
}

void CCamera::render_text()
{
	for (size_t i = 0; i < m_vecText.size(); ++i)
	{
		m_vecText[i]->render();
	}

	FieldUIMgr::GetInst()->render();

	UpdateMatrix();
}

void CCamera::FixedTransform()
{
	UpdateMatrix();
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

