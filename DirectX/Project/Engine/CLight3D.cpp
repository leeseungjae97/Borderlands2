#include "pch.h"
#include "CLight3D.h"

#include "CCamera.h"
#include "CRenderMgr.h"
#include "CTransform.h"

#include "CMesh.h"
#include "CMaterial.h"
#include "CResMgr.h"

CLight3D::CLight3D()
    : CComponent(COMPONENT_TYPE::LIGHT3D)
	, m_bShowRange(true)
	, m_LightIdx(-1)
	, m_pCamObj(nullptr)
{
	SetLightType(LIGHT_TYPE::DIRECTIONAL);

	m_pCamObj = new CGameObject;
	m_pCamObj->AddComponent(new CTransform);
	m_pCamObj->AddComponent(new CCamera);

	m_pCamObj->Camera()->SetLayerMaskAll(true);
	m_pCamObj->Camera()->SetLayerMask(31, false);
}
CLight3D::CLight3D(const CLight3D& _Origin)
	: CComponent(_Origin)
	, m_bShowRange(_Origin.m_bShowRange)
	, m_LightIdx(-1)
	, m_pCamObj(nullptr)
{
	m_pCamObj = new CGameObject(*_Origin.m_pCamObj);
}

CLight3D::~CLight3D()
{
	if (nullptr != m_pCamObj)
		delete m_pCamObj;
}

void CLight3D::finaltick()
{
	m_LightInfo.vWorldPos = Transform()->GetWorldPos();
	m_LightInfo.vWorldDir = Transform()->GetWorldDir(DIR_TYPE::FRONT);

	m_LightIdx = CRenderMgr::GetInst()->RegisterLight3D(this, m_LightInfo);
	

	if(m_bShowRange)
	{
		if((UINT)LIGHT_TYPE::POINT == m_LightInfo.LightType)
			DrawDebugSphere(Transform()->GetWorldMat(), Vec4(0.2f, 1.f, 0.2f, 1.f), 0.f, true);
		//else if((UINT)LIGHT_TYPE::SPOT == m_LightInfo.LightType)
		//	DrawDebugSphere(Transform()->GetWorldMat(), Vec4(0.2f, 1.f, 0.2f, 1.f), 0.f, true);	
	}

		// ������ ������ ī�޶� ������Ʈ�� ��ġ�� ���� ��ġ�� �����ϰ�..
	// finaltick ȣ����Ѽ� ī�޶� ������Ʈ�� ī�޶� ������Ʈ�� view, proj ��� �����Ҽ� �ְ� ��
	*m_pCamObj->Transform() = *Transform();
	m_pCamObj->finaltick_module();
}
void CLight3D::SetRadius(float _fRadius)
{
	m_LightInfo.Radius = _fRadius;

	// SphereMesh �� ���� �������� 0.5f �̱� ������ 2��� ����
	Transform()->SetRelativeScale(Vec3(_fRadius* 2.f, _fRadius* 2.f, _fRadius* 2.f));
}

void CLight3D::SetLightType(LIGHT_TYPE _type)
{
	m_LightInfo.LightType = (int)_type;

	if (LIGHT_TYPE::DIRECTIONAL == (LIGHT_TYPE)m_LightInfo.LightType)
	{
		// ������ ������ �� ��, ������ ��������� ����ȭ �� �� �ִ� �޽�(�����޽�) �� ����
		m_Mesh = CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh");
		m_Mtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"DirLightMtrl");
	}

	else if (LIGHT_TYPE::POINT == (LIGHT_TYPE)m_LightInfo.LightType)
	{
		m_Mesh = CResMgr::GetInst()->FindRes<CMesh>(L"SphereMesh");
		m_Mtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"PointLightMtrl");
	}

	else
	{
		m_Mesh = CResMgr::GetInst()->FindRes<CMesh>(L"ConeMesh");
		m_Mtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"SpotLightMtrl");
	}

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
	m_Mtrl->UpdateData();

	m_Mesh->render();
}

void CLight3D::render_shadowmap()
{
	m_pCamObj->Camera()->SortObject_Shadow();

	m_pCamObj->Camera()->render_shadowmap();
}

void CLight3D::SaveToLevelFile(FILE* _File)
{
}

void CLight3D::LoadFromLevelFile(FILE* _File)
{
}
