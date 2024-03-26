#include "pch.h"
#include "CCamera.h"

#include "CameraMgr.h"
#include "CAnimator2D.h"
#include "CAnimator3D.h"
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
#include "CKeyMgr.h"
#include "CLight3D.h"
#include "CMeshRender.h"
#include "CResMgr.h"
#include "FieldUIMgr.h"
#include "mMRT.h"


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
	, m_bESM(false)
	, m_fT{}
	, m_Outline(true)
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
		CameraMgr::GetInst()->AddCamObj(GetOwner()->GetName(), GetOwner());
		//CRenderMgr::GetInst()->RegisterCamera(this, m_iCamIdx);
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
	// ==============
	// View ��� ���
	// ==============
	m_matView = XMMatrixIdentity();

	// ī�޶� ��ǥ�� �������� �̵�
	Vec3 vCamPos = Transform()->GetRelativePos();
	Matrix matViewTrans = XMMatrixTranslation(-vCamPos.x, -vCamPos.y, -vCamPos.z);

	// ī�޶� �ٶ󺸴� ������ Z ��� �����ϰ� ����� ȸ�� ����� ����
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
	// ���� ��� ���
	// =============
	m_matProj = XMMatrixIdentity();

	if (PROJ_TYPE::ORTHOGRAPHIC == m_ProjType)
	{
		// ���� ����
		m_matProj = XMMatrixOrthographicLH(m_OrthoWidth * (1.f / m_fScale), m_OrthoHeight * (1.f / m_fScale), m_NearZ, m_FarZ);
	}
	else
	{
		// ���� ����
		m_matProj = XMMatrixPerspectiveFovLH(m_FOV, m_fAspectRatio, m_NearZ, m_FarZ);
	}
	m_matProjInv = XMMatrixInverse(nullptr, m_matProj);
}

void CCamera::UpdateMatrix()
{
	// ��� ������Ʈ
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
	// ���콺 ������ ���ϴ� Ray ���ϱ�
	// SwapChain Ÿ���� ViewPort ����
	MRT* pMRT = CRenderMgr::GetInst()->GetMRT(MRT_TYPE::SWAPCHAIN);
	D3D11_VIEWPORT tVP = pMRT->GetViewPort();

	//  ���� ���콺 ��ǥ
	Vec2 vMousePos = CKeyMgr::GetInst()->GetMousePos();

	// ������ ī�޶��� ��ǥ�� �ݵ�� ������.
	m_ray.vStart = Transform()->GetWorldPos();

	m_ray.vDir.x = ((((vMousePos.x - tVP.TopLeftX) * 2.f / tVP.Width) - 1.f) - m_matProj._31) / m_matProj._11;
	m_ray.vDir.y = (-(((vMousePos.y - tVP.TopLeftY) * 2.f / tVP.Height) - 1.f) - m_matProj._32) / m_matProj._22;
	m_ray.vDir.z = 1.f;

	m_ray.vDir = XMVector3TransformNormal(m_ray.vDir, m_matViewInv);
	m_ray.vDir.Normalize();
}

void CCamera::SortObject()
{
	// ���� ������ �з����� ����
	clear();

	// ���� ���� �����ͼ� �з�
	CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurLevel();

	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		// ���̾� ����ũ Ȯ��
		if (m_iLayerMask & (1 << i))
		{
			CLayer* pLayer = pCurLevel->GetLayer(i);
			const vector<CGameObject*>& vecObject = pLayer->GetObjects();

			for (size_t j = 0; j < vecObject.size(); ++j)
			{
				CRenderComponent* pRenderCom = vecObject[j]->GetRenderComponent();

				if (nullptr == pRenderCom)
				{
					CUI* cui = static_cast<CUI*>(vecObject[j]);
					if(cui)
						m_vecText.push_back(vecObject[j]);
					continue;
				}
					
				if (vecObject[j]->GetObjectState() == CGameObject::OBJECT_STATE::INVISIBLE)
					continue;

				//if (pRenderCom->IsFrustumCheck() 
				//	&& false == m_Frustum.FrustumCheck(vecObject[j]->Transform()->GetWorldPos(), vecObject[j]->Transform()->GetRelativeScale().x / 5.f))
				//	continue;

					// ���׸��� ������ŭ �ݺ�
				UINT iMtrlCount = pRenderCom->GetMtrlCount();

				for (UINT iMtrl = 0; iMtrl < iMtrlCount; ++iMtrl)
				{
					// ������ ���ų�, ������ ���̴��� ������ �ȵ� ���
					if (nullptr == pRenderCom->GetMaterial(iMtrl)
						|| nullptr == pRenderCom->GetMaterial(iMtrl)->GetShader())
					{
						continue;
					}

					// ���̴� �����ο� ���� �з�
					SHADER_DOMAIN eDomain = pRenderCom->GetMaterial(iMtrl)->GetShader()->GetDomain();
					Ptr<CGraphicsShader> pShader = pRenderCom->GetMaterial(iMtrl)->GetShader();

					switch (eDomain)
					{
					case SHADER_DOMAIN::DOMAIN_DEFERRED:
					case SHADER_DOMAIN::DOMAIN_OPAQUE:
					case SHADER_DOMAIN::DOMAIN_MASK:
					{
						// Shader �� POV �� ���� �ν��Ͻ� �׷��� �з��Ѵ�.
						map<ULONG64, vector<tInstObj>>* pMap = NULL;
						Ptr<CMaterial> pMtrl = pRenderCom->GetMaterial(iMtrl);

						if (pShader->GetDomain() == SHADER_DOMAIN::DOMAIN_DEFERRED)
						{
							pMap = &m_mapInstGroup_D;
						}
						else if (pShader->GetDomain() == SHADER_DOMAIN::DOMAIN_OPAQUE
							|| pShader->GetDomain() == SHADER_DOMAIN::DOMAIN_MASK)
						{
							pMap = &m_mapInstGroup_F;
						}
						//else if(pShader->GetDomain() == SHADER_DOMAIN::DOMAIN_DEFERRED_DECAL)
						//{
						//	pMap = &m_mapInstGroup_D;
						//}
						else
						{
							assert(nullptr);
							continue;
						}

						uInstID uID = {};
						uID.llID = pRenderCom->GetInstID(iMtrl);

						// ID �� 0 �� ==> Mesh �� Material �� ���õ��� �ʾҴ�.
						if (0 == uID.llID)
							continue;

						map<ULONG64, vector<tInstObj>>::iterator iter = pMap->find(uID.llID);
						if (iter == pMap->end())
						{
							pMap->insert(make_pair(uID.llID, vector<tInstObj>{tInstObj{ vecObject[j], iMtrl }}));
						}
						else
						{
							iter->second.push_back(tInstObj{ vecObject[j], iMtrl });
						}
					}
					break;
					case SHADER_DOMAIN::DOMAIN_DEFERRED_DECAL:
						m_vecDeferredDecal.push_back(vecObject[j]);
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
}

void CCamera::SortObject_Shadow()
{
	// ���� ������ �з����� ����
	clear_shadow();

	// ���� ���� �����ͼ� �з�
	CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurLevel();

	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		// ���̾� ����ũ Ȯ��
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

				// ������ ����� ���� ������Ʈ�� ����
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
	CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurLevel();
	if (pCurLevel->GetState() == LEVEL_STATE::NO_UPDATE_RENDER) return;

	

	static Ptr<CMesh> pScreen = CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh");

	static Ptr<CMaterial> pMerge = CResMgr::GetInst()->FindRes<CMaterial>(L"MergeMtrl");
	static Ptr<CMaterial> pBlurV = CResMgr::GetInst()->FindRes<CMaterial>(L"BlurVMtrl");
	static Ptr<CMaterial> pBlurH = CResMgr::GetInst()->FindRes<CMaterial>(L"BlurHMtrl");
	static Ptr<CMaterial> pGaussianBlur = CResMgr::GetInst()->FindRes<CMaterial>(L"GaussianBlurMtrl");
	static Ptr<CMaterial> pEmissiveBloom = CResMgr::GetInst()->FindRes<CMaterial>(L"BloomMtrl");
	static Ptr<CMaterial> pSpecularBloom = CResMgr::GetInst()->FindRes<CMaterial>(L"BloomMtrl");
	static Ptr<CMaterial> pToneMapping = CResMgr::GetInst()->FindRes<CMaterial>(L"ToneMappingMtrl");
	static Ptr<CMaterial> pLaplacian = CResMgr::GetInst()->FindRes<CMaterial>(L"LaplacianMtrl");

	if (m_iCamIdx == 0)
	{
		//UpdateMatrix();
		CRenderMgr::GetInst()->GetMRT(MRT_TYPE::SWAPCHAIN)->OMSet();


		CRenderMgr::GetInst()->GetMRT(MRT_TYPE::DEFERRED)->OMSet(true);
		render_deferred();

		CRenderMgr::GetInst()->GetMRT(MRT_TYPE::LIGHT)->OMSet(false);
		const vector<CLight3D*>& vecLight3D = CRenderMgr::GetInst()->GetLight3D();
		for (size_t i = 0; i < vecLight3D.size(); ++i)
		{
			vecLight3D[i]->render(m_iCamIdx);
		}

		CRenderMgr::GetInst()->GetMRT(MRT_TYPE::LUMINANCE)->OMSet();
		

		pBlurV->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"EmissiveTargetTex"));
		pBlurH->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"EmissiveTargetTex"));

		// X
		pBlurV->UpdateData();
		pScreen->render(0);

		// Y
		pBlurH->UpdateData();
		pScreen->render(0);

		CRenderMgr::GetInst()->GetMRT(MRT_TYPE::HDR)->OMSet();

		pMerge->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"ColorTargetTex"));
		pMerge->SetTexParam(TEX_1, CResMgr::GetInst()->FindRes<CTexture>(L"DiffuseTargetTex"));
		pMerge->SetTexParam(TEX_2, CResMgr::GetInst()->FindRes<CTexture>(L"EmissiveTargetTex"));
		pMerge->SetTexParam(TEX_3, CResMgr::GetInst()->FindRes<CTexture>(L"SpecularTargetTex"));
		pMerge->SetTexParam(TEX_4, CResMgr::GetInst()->FindRes<CTexture>(L"ShadowTargetTex"));

		// Merge
		pMerge->UpdateData();
		pScreen->render(0);

		// Outline
		if (m_Outline)
		{
			CRenderMgr::GetInst()->GetMRT(MRT_TYPE::HDR_LINE)->OMSet();
			pLaplacian->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"NormalTargetTex"));
			pLaplacian->SetTexParam(TEX_1, CResMgr::GetInst()->FindRes<CTexture>(L"HDRTargetTex"));
			pLaplacian->UpdateData();
			pScreen->render(0);
		}


		CRenderMgr::GetInst()->GetMRT(MRT_TYPE::BLOOMED_HDR)->OMSet();

		if (m_Outline)
			pEmissiveBloom->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"OutlineHDRTargetTex"));
		else
			pEmissiveBloom->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"HDRTargetTex"));

		pEmissiveBloom->SetTexParam(TEX_1, CResMgr::GetInst()->FindRes<CTexture>(L"EmissiveBlurredTargetTex"));
		pEmissiveBloom->SetTexParam(TEX_2, CResMgr::GetInst()->FindRes<CTexture>(L"EmissiveTargetTex"));

		// Bloom
		pEmissiveBloom->UpdateData();
		pScreen->render(0);

		//pSpecularBloom->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"HDRTargetTex"));
		//pSpecularBloom->SetTexParam(TEX_1, CResMgr::GetInst()->FindRes<CTexture>(L"SpecularBlurredTargetTex"));
		//pSpecularBloom->SetTexParam(TEX_2, CResMgr::GetInst()->FindRes<CTexture>(L"SpecularTargetTex"));
		//pSpecularBloom->UpdateData();
		//pScreen->render(0);

		CRenderMgr::GetInst()->GetMRT(MRT_TYPE::SWAPCHAIN)->OMSet();
		// Bloomed HDR ToneMapping -> LDR
		pToneMapping->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"BloomedHDRTargetTex"));

		pToneMapping->UpdateData();
		pScreen->render(0);

		render_forward();
		render_decal();
		render_transparent();

		render_postprocess();

		CRenderMgr::GetInst()->CopyRenderTarget();
	}
	if (m_iCamIdx == 1)
	{
		CRenderMgr::GetInst()->GetMRT(MRT_TYPE::SWAPCHAIN)->OMSet();

		render_ui();
		render_text();
	}

	if (m_iCamIdx == 2)
	{
		CRenderMgr::GetInst()->GetMRT(MRT_TYPE::SCOPE_RENDER)->OMSet();
		CRenderMgr::GetInst()->GetMRT(MRT_TYPE::SCOPE_DEFERRED_RENDER)->OMSet(true);
		render_deferred();

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

		// Merge
		pMerge->UpdateData();
		pScreen->render(0);

		CRenderMgr::GetInst()->GetMRT(MRT_TYPE::SCOPE_RENDER)->OMSet();

		render_forward();
		render_decal();
		render_transparent();
		render_postprocess();

		CRenderMgr::GetInst()->CopyRenderTarget();

		CRenderMgr::GetInst()->GetMRT(MRT_TYPE::SWAPCHAIN)->OMSet();
	}
}


void CCamera::clear()
{
	for (auto& pair : m_mapInstGroup_D)
		pair.second.clear();
	for (auto& pair : m_mapInstGroup_F)
		pair.second.clear();

	//m_vecDeferred.clear();
	m_vecDeferredDecal.clear();

	m_vecOpaque.clear();
	m_vecMask.clear();
	m_vecDecal.clear();
	m_vecTransparent.clear();
	m_vecPost.clear();
	m_vecUI.clear();
	m_vecText.clear();
}

void CCamera::clear_shadow()
{
	m_vecShadow.clear();
}

void CCamera::render_deferred()
{
	UpdateMatrix();

	for (auto& pair : m_mapSingleObj)
	{
		pair.second.clear();
	}

	// Deferred object render
	tInstancingData tInstData = {};

	for (auto& pair : m_mapInstGroup_D)
	{
		if (pair.second.empty())
			continue;

		if (pair.second.size() <= 2
			|| pair.second[0].pObj->Animator2D()
			|| pair.second[0].pObj->GetRenderComponent()->GetMaterial(pair.second[0].iMtrlIdx)->GetShader()->GetVSInst() == nullptr)
		{
			for (UINT i = 0; i < pair.second.size(); ++i)
			{
				map<INT_PTR, vector<tInstObj>>::iterator iter
					= m_mapSingleObj.find((INT_PTR)pair.second[i].pObj);

				if (iter != m_mapSingleObj.end())
					iter->second.push_back(pair.second[i]);
				else
				{
					m_mapSingleObj.insert(make_pair((INT_PTR)pair.second[i].pObj, vector<tInstObj>{pair.second[i]}));
				}
			}
			continue;
		}

		CGameObject* pObj = pair.second[0].pObj;
		Ptr<CMesh> pMesh = pObj->GetRenderComponent()->GetMesh();
		Ptr<CMaterial> pMtrl = pObj->GetRenderComponent()->GetMaterial(pair.second[0].iMtrlIdx);

		// Instancing ���� Ŭ����
		CInstancingBuffer::GetInst()->Clear();

		int iRowIdx = 0;
		bool bHasAnim3D = false;
		for (UINT i = 0; i < pair.second.size(); ++i)
		{
			tInstData.matWorld = pair.second[i].pObj->Transform()->GetWorldMat();
			tInstData.matWV = tInstData.matWorld * m_matView;
			tInstData.matWVP = tInstData.matWV * m_matProj;

			if (pair.second[i].pObj->Animator3D())
			{
				pair.second[i].pObj->Animator3D()->UpdateData();
				tInstData.iRowIdx = iRowIdx++;
				CInstancingBuffer::GetInst()->AddInstancingBoneMat(pair.second[i].pObj->Animator3D()->GetFinalBoneMat());
				bHasAnim3D = true;
			}
			else
				tInstData.iRowIdx = -1;

			CInstancingBuffer::GetInst()->AddInstancingData(tInstData);
		}

		CInstancingBuffer::GetInst()->SetData();

		if (bHasAnim3D)
		{
			pMtrl->SetAnim3D(true); // Animation Mesh �˸���
			pMtrl->SetBoneCount(pMesh->GetBoneCount());
		}

		pMtrl->UpdateData_Instancing();
		pMesh->render_instancing(pair.second[0].iMtrlIdx);

		// ����
		if (bHasAnim3D)
		{
			pMtrl->SetAnim3D(false); // Animation Mesh �˸���
			pMtrl->SetBoneCount(0);
		}
	}

	// ���� ������
	for (auto& pair : m_mapSingleObj)
	{
		if (pair.second.empty())
			continue;

		//pair.second[0].pObj->Transform()->UpdateData();

		for (auto& instObj : pair.second)
		{
			instObj.pObj->GetRenderComponent()->render(instObj.iMtrlIdx, true);
		}
		//if (pair.second[0].pObj->Animator2D())
		//{
		//	pair.second[0].pObj->Animator2D()->ClearData();
		//}

		//if (pair.second[0].pObj->Animator3D())
		//{
		//	pair.second[0].pObj->Animator3D()->ClearData();
		//}
	}

	//CRenderMgr::GetInst()->GetMRT(MRT_TYPE::DEFERRED_DECAL)->OMSet();

	//for (size_t i = 0; i < m_vecDeferredDecal.size(); ++i)
	//{
	//	m_vecDeferredDecal[i]->render();
	//}
}

void CCamera::render_forward()
{
	UpdateMatrix();

	for (auto& pair : m_mapSingleObj)
	{
		pair.second.clear();
	}

	// Deferred object render
	tInstancingData tInstData = {};

	for (auto& pair : m_mapInstGroup_F)
	{
		// �׷� ������Ʈ�� ���ų�, ���̴��� ���� ���
		if (pair.second.empty())
			continue;

		// instancing ���� ���� �̸��̰ų�
		// Animation2D ������Ʈ�ų�(��������Ʈ �ִϸ��̼� ������Ʈ)
		// Shader �� Instancing �� �������� �ʴ°��
		if (pair.second.size() <= 2
			|| pair.second[0].pObj->Animator2D()
			|| pair.second[0].pObj->GetRenderComponent()->GetMaterial(pair.second[0].iMtrlIdx)->GetShader()->GetVSInst() == nullptr)
		{
			// �ش� ��ü���� ���� ���������� ��ȯ
			for (UINT i = 0; i < pair.second.size(); ++i)
			{
				map<INT_PTR, vector<tInstObj>>::iterator iter
					= m_mapSingleObj.find((INT_PTR)pair.second[i].pObj);

				if (iter != m_mapSingleObj.end())
					iter->second.push_back(pair.second[i]);
				else
				{
					m_mapSingleObj.insert(make_pair((INT_PTR)pair.second[i].pObj, vector<tInstObj>{pair.second[i]}));
				}
			}
			continue;
		}

		CGameObject* pObj = pair.second[0].pObj;
		Ptr<CMesh> pMesh = pObj->GetRenderComponent()->GetMesh();
		Ptr<CMaterial> pMtrl = pObj->GetRenderComponent()->GetMaterial(pair.second[0].iMtrlIdx);

		// Instancing ���� Ŭ����
		CInstancingBuffer::GetInst()->Clear();

		int iRowIdx = 0;
		bool bHasAnim3D = false;
		for (UINT i = 0; i < pair.second.size(); ++i)
		{
			tInstData.matWorld = pair.second[i].pObj->Transform()->GetWorldMat();
			tInstData.matWV = tInstData.matWorld * m_matView;
			tInstData.matWVP = tInstData.matWV * m_matProj;

			if (pair.second[i].pObj->Animator3D())
			{
				pair.second[i].pObj->Animator3D()->UpdateData();
				tInstData.iRowIdx = iRowIdx++;
				CInstancingBuffer::GetInst()->AddInstancingBoneMat(pair.second[i].pObj->Animator3D()->GetFinalBoneMat());
				bHasAnim3D = true;
			}
			else
				tInstData.iRowIdx = -1;

			CInstancingBuffer::GetInst()->AddInstancingData(tInstData);
		}

		// �ν��Ͻ̿� �ʿ��� �����͸� ����(SysMem -> GPU Mem)
		CInstancingBuffer::GetInst()->SetData();

		if (bHasAnim3D)
		{
			pMtrl->SetAnim3D(true); // Animation Mesh �˸���
			pMtrl->SetBoneCount(pMesh->GetBoneCount());
		}

		pMtrl->UpdateData_Instancing();
		pMesh->render_instancing(pair.second[0].iMtrlIdx);

		// ����
		if (bHasAnim3D)
		{
			pMtrl->SetAnim3D(false); // Animation Mesh �˸���
			pMtrl->SetBoneCount(0);
		}
	}

	// ���� ������
	for (auto& pair : m_mapSingleObj)
	{
		if (pair.second.empty())
			continue;

		//pair.second[0].pObj->Transform()->UpdateData();

		for (auto& instObj : pair.second)
		{
			instObj.pObj->GetRenderComponent()->render(instObj.iMtrlIdx, false);
		}
		//if (pair.second[0].pObj->Animator2D())
		//{
		//	pair.second[0].pObj->Animator2D()->ClearData();
		//}

		//if (pair.second[0].pObj->Animator3D())
		//{
		//	pair.second[0].pObj->Animator3D()->ClearData();
		//}
	}
}

void CCamera::render_shadowmap()
{
	UpdateMatrix();
	//g_transform.matView = m_matView;
	//g_transform.matProj = m_matProj;

	for (size_t i = 0; i < m_vecShadow.size(); ++i)
	{
		m_vecShadow[i]->render_shadowmap();
	}
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
