#include "pch.h"
#include "CSolarSystem.h"

#include "CPlanetMove.h"
#include "CResMgr.h"
#include "CTransform.h"

CSolarSystem::CSolarSystem()
	: CRenderComponent(COMPONENT_TYPE::SOLOAR_SYSTEM)
	, m_PlanetTexture(nullptr)
	, m_PlanetTexture2(nullptr)
	, m_PlanetNormalTexture(nullptr)
	, m_PlanetCoverTexture(nullptr)
	, pCoverImage(nullptr)
    , m_bEmissive(false)
{

}

CSolarSystem::~CSolarSystem()	
{
}

void CSolarSystem::finaltick()
{
	if(nullptr != m_PlanetCoverTexture)
	{
		if(nullptr == pCoverImage)
		{
			Vec3 vParentScale =GetOwner()->Transform()->GetRelativeScale();
			vParentScale.x += 10.f;
			vParentScale.y += 10.f;
			vParentScale.z += 10.f;
			pCoverImage = new CGameObject;
			pCoverImage->SetName(L"Cover");
			pCoverImage->AddComponent(new CTransform);
			pCoverImage->AddComponent(new CSolarSystem);
			pCoverImage->AddComponent(new CPlanetMove);
			pCoverImage->PlanetMove()->SetRotate(true);
			pCoverImage->Transform()->SetRelativeScale(vParentScale);
			pCoverImage->SolarSystem()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"SphereMesh"));
			pCoverImage->SolarSystem()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Planet3D_DeferredAlphaMtrl"), 0);
			pCoverImage->SolarSystem()->SetPlanetTex(m_PlanetCoverTexture);
			pCoverImage->Transform()->SetAbsolute(true);

			GetOwner()->AddChild(pCoverImage);
		}
		//GetMaterial()->SetTexParam(TEX_3, m_PlanetCoverTexture);
	}
}

void CSolarSystem::render()
{

	if (nullptr == GetMesh() || nullptr == GetMaterial(0))
		return;

	// Transform 에 UpdateData 요청
	Transform()->UpdateData();
	if(nullptr != m_PlanetTexture)
		GetMaterial(0)->SetTexParam(TEX_0, m_PlanetTexture);

	if(nullptr != m_PlanetNormalTexture)
		GetMaterial(0)->SetTexParam(TEX_1, m_PlanetNormalTexture);

	if(nullptr != m_PlanetTexture2)
		GetMaterial(0)->SetTexParam(TEX_2, m_PlanetTexture2);

	GetMaterial(0)->SetScalarParam(INT_0, &m_bEmissive);
	// 재질 업데이트
	GetMaterial(0)->UpdateData();

	// 렌더
	GetMesh()->render(0);
}

void CSolarSystem::SaveToLevelFile(FILE* _File)
{
}

void CSolarSystem::LoadFromLevelFile(FILE* _File)
{
}
