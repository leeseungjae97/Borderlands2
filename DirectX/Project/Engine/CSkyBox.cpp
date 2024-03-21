#include "pch.h"
#include "CSkyBox.h"

#include "CResMgr.h"
#include "CTransform.h"

CSkyBox::CSkyBox()
	: CRenderComponent(COMPONENT_TYPE::SKYBOX)
	, m_Type(SKYBOX_TYPE::SPHERE)
	, m_bFbxSkyBox(false)
{
	SetSkyBoxType(m_Type);

	SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"SkyBoxMtrl"), 0);
	SetFrustumCheck(false);
}
CSkyBox::CSkyBox(bool _FromMeshData)
	: CRenderComponent(COMPONENT_TYPE::SKYBOX)
	, m_bFbxSkyBox(_FromMeshData)
{
	//SetSkyBoxType(m_Type);

	//SetMaterial(, 0);
	Ptr<CGraphicsShader> skyBoxShader = CResMgr::GetInst()->FindRes<CGraphicsShader>(L"SkyBoxShader");
	m_SkyBoxMtrl = new CMaterial(true);
	m_SkyBoxMtrl->SetShader(skyBoxShader);
	CResMgr::GetInst()->AddRes(L"FbxSkyBox", m_SkyBoxMtrl);
	SetFrustumCheck(false);

	//m_SkyBoxMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"SkyBoxMtrl");

	//SetFrustumCheck(false);
}

CSkyBox::~CSkyBox()
{
}

void CSkyBox::SetSkyBoxType(SKYBOX_TYPE _Type)
{
	m_Type = _Type;
	if(m_bFbxSkyBox)
	{
		if (nullptr != GetMaterial(0))
		{
			m_SkyBoxTex = GetMaterial(0)->GetTexParam(TEX_0);
			m_SkyBoxMtrl->SetTexParam(TEX_0, m_SkyBoxTex);

			//SKYBOX_TYPE mT = SKYBOX_TYPE::HEMI_SPHERE;
			//m_SkyBoxMtrl->SetScalarParam(INT_0, &mT);
			SetMaterial(m_SkyBoxMtrl, 0);
		}
	}else
	{
		if (m_Type == SKYBOX_TYPE::SPHERE)
		{
			SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"SphereMesh"));
		}
		else if (m_Type == SKYBOX_TYPE::CUBE)
		{
			SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"CubeMesh"));
		}
		else if (m_Type == SKYBOX_TYPE::HEMI_SPHERE)
		{
			SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"mesh\\warrior_skybox.mesh"));
		}

		SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"SkyBoxMtrl"), 0);
	}


	//if (!m_bFbxSkyBox)
	//{
	//	
	//}
	//else
	//{
	//	//SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"SphereMesh"));
	//	SetMaterial(m_SkyBoxMtrl, 0);
	//}
}

void CSkyBox::SetSkyBoxTexture(Ptr<CTexture> _Tex)
{
	m_SkyBoxTex = _Tex;
}

void CSkyBox::render()
{
	//if (nullptr == GetMesh() || nullptr == GetMaterial(0))
	//	return;

	Transform()->UpdateData();

	if (!m_bFbxSkyBox)
	{
	}
	GetMaterial(0)->SetScalarParam(INT_0, &m_Type);
	if (nullptr != m_SkyBoxTex)
	{
		if (m_SkyBoxTex->IsCube())
		{
			GetMaterial(0)->SetTexParam(TEX_CUBE_0, m_SkyBoxTex);
		}
		else
		{
			GetMaterial(0)->SetTexParam(TEX_0, m_SkyBoxTex);
		}
	}

	GetMaterial(0)->UpdateData();

	//else
	//{
	//	m_SkyBoxMtrl->SetScalarParam(INT_0, &m_Type);
	//	m_SkyBoxMtrl->SetTexParam(TEX_0, m_SkyBoxTex);
	//	m_SkyBoxMtrl->UpdateData();
	//}
	//
	GetMesh()->render(0);
}

void CSkyBox::finaltick()
{
}

void CSkyBox::render(UINT _iSubset, bool _Deferred)
{
	render();
}

void CSkyBox::SaveToLevelFile(FILE* _File)
{
	fwrite(&m_Type, sizeof(UINT), 1, _File);
	SaveResRef(m_SkyBoxTex.Get(), _File);
	SaveResRef(m_SkyBoxMtrl.Get(), _File);
	fwrite(&m_bFbxSkyBox, sizeof(bool), 1, _File);
}

void CSkyBox::LoadFromLevelFile(FILE* _FILE)
{
	fread(&m_Type, sizeof(UINT), 1, _FILE);
	LoadResRef(m_SkyBoxTex, _FILE);
	LoadResRef(m_SkyBoxMtrl, _FILE);
	fread(&m_bFbxSkyBox, sizeof(bool), 1, _FILE);

	//if (m_bFbxSkyBox)
	//{
	//	//Ptr<CGraphicsShader> skyBoxShader = CResMgr::GetInst()->FindRes<CGraphicsShader>(L"SkyBoxShader");
	//	//m_SkyBoxMtrl = new CMaterial(true);
	//	//m_SkyBoxMtrl->SetShader(skyBoxShader);

	//	//CResMgr::GetInst()->AddRes(L"FbxSkyBox", m_SkyBoxMtrl);

	//	SetFrustumCheck(false);

	//	//m_SkyBoxMtrl->SetTexParam(TEX_0, m_SkyBoxTex);
	//	//SetMaterial(m_SkyBoxMtrl, 0);
	//	
	//	
	//}
	//else
	//{
	//	SetSkyBoxType(m_Type);
	//}

	SetSkyBoxType(m_Type);
}

//void CSkyBox::FbxSky()
//{
//	m_bFbxSkyBox = true;
//}


