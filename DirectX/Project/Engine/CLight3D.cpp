#include "pch.h"
#include "CLight3D.h"

#include "CCamera.h"
#include "CRenderMgr.h"
#include "CTransform.h"

#include "CMesh.h"
#include "CMaterial.h"
#include "CResMgr.h"
#include "CTimeMgr.h"

CLight3D::CLight3D()
    : CComponent(COMPONENT_TYPE::LIGHT3D)
	, m_bShowRange(true)
	, m_LightIdx(-1)
	, m_pCamObj(nullptr)
	, m_bShadow(false)
	, m_bGaus(false)
	, m_fLightDepthCoeff(0.01f)
	, m_f{}
	, m_bTimeConst(false)
	, m_fLifeSpan(0.0f)
{
	//SetLightType(LIGHT_TYPE::DIRECTIONAL);
}
CLight3D::CLight3D(const CLight3D& _Origin)
	: CComponent(_Origin)
	, m_bShowRange(_Origin.m_bShowRange)
	, m_LightIdx(_Origin.m_LightIdx)
	, m_pCamObj(nullptr)
	, m_bShadow(_Origin.m_bShadow)
	, m_bGaus(_Origin.m_bGaus)
	, m_fLightDepthCoeff(_Origin.m_fLightDepthCoeff)
{
	m_pCamObj = new CGameObject(*_Origin.m_pCamObj);
	CopyArray(m_f, _Origin.m_f);
}

CLight3D::~CLight3D()
{
	if (nullptr != m_pCamObj)
		delete m_pCamObj;
}
void CLight3D::finaltick()
{
	if (m_fLifeSpan != 0.0f)
		m_fLifeSpan -= DT;

	if(m_bTimeConst && m_fLifeSpan <= 0.0f)
	{
		DestroyObject(GetOwner());
	}

	m_LightInfo.vWorldPos = Transform()->GetWorldPos();
	m_LightInfo.vWorldDir = Transform()->GetWorldDir(DIR_TYPE::FRONT);

	m_LightIdx = CRenderMgr::GetInst()->RegisterLight3D(this, m_LightInfo);
	

	if(m_bShowRange)
	{
		if((UINT)LIGHT_TYPE::DIRECTIONAL== m_LightInfo.LightType)
			DrawDebugSphere(Transform()->GetWorldMat(), Vec4(0.2f, 1.f, 0.2f, 1.f), 0.f, false);
		//if((UINT)LIGHT_TYPE::POINT == m_LightInfo.LightType)
		//	DrawDebugSphere(Transform()->GetWorldMat(), Vec4(0.2f, 1.f, 0.2f, 1.f), 0.f, false);
		//else if((UINT)LIGHT_TYPE::SPOT == m_LightInfo.LightType)
		//	DrawDebugSphere(Transform()->GetWorldMat(), Vec4(0.2f, 1.f, 0.2f, 1.f), 0.f, true);	
	}

	// 광원에 부착한 카메라 오브젝트도 위치를 광원 위치랑 동일하게..
	// finaltick 호출시켜서 카메라 오브젝트의 카메라 컴포넌트의 view, proj 행렬 연산할수 있게 함
	if ((UINT)LIGHT_TYPE::DIRECTIONAL == m_LightInfo.LightType)
	{
		*m_pCamObj->Transform() = *Transform();
		m_pCamObj->finaltick_module();
	}	
}
void CLight3D::SetRadius(float _fRadius)
{
	m_LightInfo.Radius = _fRadius;

	Transform()->SetRelativeScale(Vec3(_fRadius* 2.f, _fRadius* 2.f, _fRadius* 2.f));
}

void CLight3D::SetLightType(LIGHT_TYPE _type)
{
	m_LightInfo.LightType = (int)_type;
	
	if (LIGHT_TYPE::DIRECTIONAL == _type)
	{
		m_pCamObj = new CGameObject;
		m_pCamObj->AddComponent(new CTransform);
		m_pCamObj->AddComponent(new CCamera);

		m_pCamObj->Camera()->SetLayerMaskAll(true);
		m_pCamObj->Camera()->SetLayerMask((int)LAYER_TYPE::ViewPortUI, false);

		m_Mesh = CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh");
		m_Mtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"DirLightMtrl");

		m_pCamObj->Camera()->SetFarZ(100000.f);
		m_pCamObj->Camera()->SetProjType(PROJ_TYPE::ORTHOGRAPHIC);
		m_pCamObj->Camera()->SetOrthoWidth(800000.f);
		m_pCamObj->Camera()->SetOrthoHeight(800000.f);
		m_pCamObj->Camera()->SetESM(false);
	}
	if (LIGHT_TYPE::POINT == _type)
	{
		m_Mesh = CResMgr::GetInst()->FindRes<CMesh>(L"SphereMesh");
		m_Mtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"PointLightMtrl");
	}

	//if (LIGHT_TYPE::SPOT == _type)
	//{
	//	m_Mesh = CResMgr::GetInst()->FindRes<CMesh>(L"ConeMesh");
	//	m_Mtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"SpotLightMtrl");
	//}
	if (nullptr != m_Mtrl)
	{
		m_Mtrl->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"NormalTargetTex"));
		m_Mtrl->SetTexParam(TEX_1, CResMgr::GetInst()->FindRes<CTexture>(L"PositionTargetTex"));
	}
}

void CLight3D::render()
{
	Transform()->UpdateData();

	m_Mtrl->SetScalarParam(INT_0, &m_LightIdx);

	if (m_LightInfo.LightType == (UINT)LIGHT_TYPE::DIRECTIONAL)
	{
		Matrix matVP = m_pCamObj->Camera()->GetViewMat() * m_pCamObj->Camera()->GetProjMat();
		m_Mtrl->SetScalarParam(MAT_0, &matVP);
		m_Mtrl->SetTexParam(TEX_2, CResMgr::GetInst()->FindRes<CTexture>(L"DynamicShadowMapTex"));
		int IsShadow = m_bShadow;
		int IsGau = m_bGaus;
		m_Mtrl->SetScalarParam(INT_1, &IsShadow);
		m_Mtrl->SetScalarParam(INT_2, &IsGau);
		m_Mtrl->SetScalarParam(FLOAT_0, &m_fLightDepthCoeff);
		m_Mtrl->SetScalarParam(FLOAT_1, &m_f[0]);
		m_Mtrl->SetScalarParam(FLOAT_2, &m_f[1]);
		m_Mtrl->SetScalarParam(FLOAT_3, &m_f[2]);
	}
	//if(m_LightInfo.LightType == (UINT)LIGHT_TYPE::SPOT)
	//{
	//	Matrix matVP = m_pCamObj->Camera()->GetViewMat() * m_pCamObj->Camera()->GetProjMat();
	//	m_Mtrl->SetScalarParam(MAT_0, &matVP);
	//	m_Mtrl->SetTexParam(TEX_2, CResMgr::GetInst()->FindRes<CTexture>(L"DynamicShadowMapTex"));
	//}

	m_Mtrl->UpdateData();

	m_Mesh->render(0);
}

void CLight3D::SetLifeSpan(float _Duration)
{
	m_fLifeSpan = _Duration;
	m_bTimeConst = true;
}

void CLight3D::render_shadowmap()
{
	if(m_pCamObj)
	{
		m_pCamObj->Camera()->SortObject_Shadow();
		m_pCamObj->Camera()->render_shadowmap();
	}
}

void CLight3D::SaveToLevelFile(FILE* _File)
{
	fwrite(&m_LightInfo, sizeof(tLightInfo), 1, _File);

	SaveResRef(m_Mesh.Get(), _File);
	SaveResRef(m_Mtrl.Get(), _File);

	fwrite(&m_LightIdx, sizeof(UINT), 1, _File);
	fwrite(&m_bShowRange, sizeof(int), 1, _File);

	fwrite(&m_bShadow, sizeof(int), 1, _File);
	fwrite(&m_bGaus, sizeof(int), 1, _File);
	fwrite(&m_fLightDepthCoeff, sizeof(float), 1, _File);

	fwrite(&m_f[0], sizeof(float), 1, _File);
	fwrite(&m_f[1], sizeof(float), 1, _File);
	fwrite(&m_f[2], sizeof(float), 1, _File);
}

void CLight3D::LoadFromLevelFile(FILE* _File)
{
	fread(&m_LightInfo, sizeof(tLightInfo), 1, _File);

	LoadResRef(m_Mesh, _File);
	LoadResRef(m_Mtrl, _File);

	fread(&m_LightIdx, sizeof(UINT), 1, _File);
	fread(&m_bShowRange, sizeof(int), 1, _File);

	fread(&m_bShadow, sizeof(int), 1, _File);
	fread(&m_bGaus, sizeof(int), 1, _File);
	fread(&m_fLightDepthCoeff, sizeof(float), 1, _File);

	fread(&m_f[0], sizeof(float), 1, _File);
	fread(&m_f[1], sizeof(float), 1, _File);
	fread(&m_f[2], sizeof(float), 1, _File);

	//m_pCamObj = new CGameObject;
	//m_pCamObj->AddComponent(new CTransform);
	//m_pCamObj->AddComponent(new CCamera);

	//m_pCamObj->Camera()->SetLayerMaskAll(true);
	//m_pCamObj->Camera()->SetLayerMask(31, false);

	//SetLightType(LIGHT_TYPE::DIRECTIONAL);
}
