#include "pch.h"
#include "CPlanetMove.h"

#include "CTimeMgr.h"
#include "CTransform.h"

CPlanetMove::CPlanetMove()
	: CComponent(COMPONENT_TYPE::PLANET_MOVE)
	, m_bRotate(false)
	, m_bXRotate(false)
	, m_bReverseRotate(false)
	, m_bOrbit(false)
	, m_fOrbitTheta(XM_PI)
	, m_fRotateSpeed(1.f)
	, m_fOrbitSpeed(0.f)
	, m_fOrbitRadius(0.f)
{
}

CPlanetMove::~CPlanetMove()
{
}

void CPlanetMove::finaltick()
{
	if (m_bRotate)
		Rotate();
	if (m_bOrbit)
		Orbit();
}

void CPlanetMove::Rotate()
{
	Vec3 vRot = GetOwner()->Transform()->GetRelativeRot();


	if(m_bXRotate)
	{
		if(m_bReverseRotate)
			vRot.y += sin(m_fRotateSpeed * CTimeMgr::GetInst()->GetDeltaTime());
		else 
			vRot.y -= sin(m_fRotateSpeed * CTimeMgr::GetInst()->GetDeltaTime());
	}
	else
	{
		if(m_bReverseRotate)
			vRot.y += sin(m_fRotateSpeed * CTimeMgr::GetInst()->GetDeltaTime());
		else 
			vRot.y -= sin(m_fRotateSpeed * CTimeMgr::GetInst()->GetDeltaTime());	
	}
		

	GetOwner()->Transform()->SetRelativeRot(vRot);
}

void CPlanetMove::Orbit()
{
	Vec3 vPos = GetOwner()->Transform()->GetRelativePos();
	
	vPos.x += sinf(m_fOrbitTheta) * m_fOrbitSpeed * CTimeMgr::GetInst()->GetDeltaTime();
	vPos.z -= cosf(m_fOrbitTheta) * m_fOrbitSpeed * CTimeMgr::GetInst()->GetDeltaTime();

	m_fOrbitTheta += (m_fOrbitSpeed * CTimeMgr::GetInst()->GetDeltaTime()) / m_fOrbitRadius;

	GetOwner()->Transform()->SetRelativePos(vPos);
}

void CPlanetMove::SaveToLevelFile(FILE* _File)
{
}

void CPlanetMove::LoadFromLevelFile(FILE* _FILE)
{
}
