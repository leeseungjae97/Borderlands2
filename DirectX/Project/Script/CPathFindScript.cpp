#include "pch.h"
#include "CPathFindScript.h"

#include <Engine/NavigationMgr.h>

#include "DetourNavMeshQuery.h"

CPathFindScript::CPathFindScript()
	: CScript((UINT)SCRIPT_TYPE::PATH_FIND_SCRIPT)
{
}

CPathFindScript::~CPathFindScript()
{
}

void CPathFindScript::begin()
{

}

void CPathFindScript::tick()
{

}

void CPathFindScript::Move()
{
	CGameObject* pOwner = GetOwner();
	Vec3 vPos = pOwner->Transform()->GetRelativePos();
	Vec3 vFront = pOwner->Transform()->GetRelativeDir(DIR_TYPE::FRONT);
	if (m_vecQueryPath.empty())
	{
		vDestPos = vPos;
		return;
	}
	Vec3 vPathPos = m_vecQueryPath.front();

	if (vPathPos != vDestPos)
	{
		vPrevPos = vPos;
		vDestPos = vPathPos;
		fDestDist = fabs((vDestPos - vPos).Length());
	}

	float fRemainDist = fabs((vPrevPos - vPos).Length());

	if (fRemainDist < fDestDist)
	{
		Vec3 vDir = vDestPos - vPos;
		vDir.Normalize();

		vPos += vDir * DT * 200.f;
	}
	else
	{
		auto iter = m_vecQueryPath.begin();
		iter = m_vecQueryPath.erase(iter);
	}

	pOwner->Transform()->SetRelativePos(vPos);
	//Vec3 vOffsetPos = vDestPos;
	//Vec3 vDir = vOffsetPos - vPos;
	//vDir.Normalize();
	//vDir.x = 0;


}

void CPathFindScript::finaltick()
{
	//if(KEY_TAP(KEY::Q))
	//{
	//	DoQuery();
	//}
	//Move();
}
void CPathFindScript::BeginOverlap(CCollider3D* _Other)
{
}

void CPathFindScript::DoQuery()
{
	m_vecQueryPath.clear();
	Vec3 vPos = GetOwner()->Transform()->GetRelativePos();
	Vec3 vMousePos = NavigationMgr::GetInst()->GetMapRaycastPos();
	int size = 0;
	NavigationMgr::GetInst()->DoQuery(vPos, vMousePos, &m_vecQueryPath, &size);
}

void CPathFindScript::LoadFromLevelFile(FILE* _FILE)
{
}
void CPathFindScript::SaveToLevelFile(FILE* _File)
{
}