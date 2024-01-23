#include "pch.h"
#include "CPathFind.h"

#include "CTimeMgr.h"
#include "CTransform.h"

CPathFind::CPathFind()
	: CComponent(COMPONENT_TYPE::PATH_FIND)
	, m_fSpeed(300.f)
{
}

CPathFind::~CPathFind()
{
}

void CPathFind::SetMVecEnd(Vec3 m_vec_end)
{
	Vec3 vPos = Transform()->GetRelativePos();
	m_vPrevPos = vPos;
	m_vEnd = m_vec_end;
}

void CPathFind::finaltick()
{
	Move();
}

void CPathFind::Move()
{
	Vec3 vPos = Transform()->GetRelativePos();
	Vec3 vDir = m_vPrevPos - m_vEnd;
	float fMovedDist = fabs((vPos - m_vPrevPos).Length());
	float fDestDist = fabs(vDir.Length());

	if (fMovedDist >= fDestDist)
		return;

	vDir.Normalize();

	vPos.x += m_fSpeed * vDir.x * DT;
	vPos.y += m_fSpeed * vDir.y * DT;
	vPos.z += m_fSpeed * vDir.z * DT;

	Transform()->SetRelativePos(vPos);
}

void CPathFind::LoadFromLevelFile(FILE* _FILE)
{
}

void CPathFind::SaveToLevelFile(FILE* _File)
{
}
