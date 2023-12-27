#include "pch.h"
#include "CSolarSystem.h"

#include "CTransform.h"

CSolarSystem::CSolarSystem()
	: CRenderComponent(COMPONENT_TYPE::SOLOAR_SYSTEM)
	, m_PlanetTexture(nullptr)
    , m_bEmissive(false)
{

}

CSolarSystem::~CSolarSystem()
{
}

void CSolarSystem::SetPlanetTex(const Ptr<CTexture>& _Tex)
{
	m_PlanetTexture = _Tex;
}

void CSolarSystem::finaltick()
{
}

void CSolarSystem::render()
{

	if (nullptr == GetMesh() || nullptr == GetMaterial())
		return;

	// Transform 에 UpdateData 요청
	Transform()->UpdateData();
	if(nullptr != m_PlanetTexture)
		GetMaterial()->SetTexParam(TEX_0, m_PlanetTexture);

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
