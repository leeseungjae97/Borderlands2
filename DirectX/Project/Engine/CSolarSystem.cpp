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
			vParentScale.x += 10000.f;
			vParentScale.y += 10000.f;
			vParentScale.z += 10000.f;
			pCoverImage = new CGameObject;
			pCoverImage->SetName(L"Cover");
			pCoverImage->AddComponent(new CTransform);
			pCoverImage->AddComponent(new CSolarSystem);
			pCoverImage->AddComponent(new CPlanetMove);
			//pCoverImage->PlanetMove()->SetOrbit(true);
			//pCoverImage->PlanetMove()->SetRotate(true);
			pCoverImage->Transform()->SetRelativeScale(vParentScale);
			pCoverImage->SolarSystem()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"SphereMesh"));
			pCoverImage->SolarSystem()->SetPlanetTex(m_PlanetCoverTexture);
			pCoverImage->Transform()->SetAbsolute(true);

			GetOwner()->AddChild(pCoverImage);
		}
		//GetMaterial()->SetTexParam(TEX_3, m_PlanetCoverTexture);
	}
}

void CSolarSystem::render()
{

	if (nullptr == GetMesh() || nullptr == GetMaterial())
		return;

	// Transform 에 UpdateData 요청
	Transform()->UpdateData();
	if(nullptr != m_PlanetTexture)
		GetMaterial()->SetTexParam(TEX_0, m_PlanetTexture);

	if(nullptr != m_PlanetNormalTexture)
		GetMaterial()->SetTexParam(TEX_1, m_PlanetNormalTexture);

	if(nullptr != m_PlanetTexture2)
		GetMaterial()->SetTexParam(TEX_2, m_PlanetTexture2);

	GetMaterial()->SetScalarParam(INT_0, &m_bEmissive);
	// 재질 업데이트
	GetMaterial()->UpdateData();

	// 렌더
	GetMesh()->render();
}

void CSolarSystem::SaveToLevelFile(FILE* _File)
{
}

void CSolarSystem::LoadFromLevelFile(FILE* _File)
{
}
