#include "pch.h"
#include "CPathFindScript.h"

#include <Engine/NavigationMgr.h>

#include "DetourNavMeshQuery.h"

// Test Script
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
	CGameObject* pCamera = GetOwner();

	//if (pCamera->GetName() != L"MainMenuCamera")
	//	return;

	Vec3 vPos = pCamera->Transform()->GetRelativePos();
	Vec3 vUp = pCamera->Transform()->GetRelativeDir(DIR_TYPE::UP);
	Vec3 vRight = pCamera->Transform()->GetRelativeDir(DIR_TYPE::RIGHT);

	vPos.x += sinf(m_Theta) * DT * 2000.f;
	vPos.z -= cosf(m_Theta) * DT * 2000.f;

	m_Theta += (2000.f * DT)/ m_fRadius;

	Vec3 vDiff = m_vTargetPos - vPos;
	Vec3 vDir = vDiff;
	vDir.Normalize();
	vRight.Normalize();

	float fAngle = vDir.Dot(vRight);

	//Matrix rotMat = XMMatrixLookToLH(vPos, vDir, vUp);

	//Quat quat;
	//Vec3 vS, vT;
	//rotMat.Decompose(vS, quat, vT);

	//Vec3 vRot = physx::Util::QuaternionToVector3(quat);
	Vec3 vRot = pCamera->Transform()->GetRelativeRot();
	vRot.y += fAngle;
	//vRot.y += (2000.f * DT) / m_fRadius;
	pCamera->Transform()->SetRelativeRot(vRot);
	pCamera->Transform()->SetRelativePos(vPos);
}

void CPathFindScript::finaltick()
{
	if (KEY_TAP(KEY::G))
	{
		CGameObject* pCamera = GetOwner();
		Vec3 vPos = pCamera->Transform()->GetRelativePos();
		Vec3 vFront = pCamera->Transform()->GetRelativeDir(DIR_TYPE::FRONT);
		m_vTargetPos = Vec3(0.f , vPos.y, 0.f);
		m_fRadius = (m_vTargetPos - vPos).Length();
		m_bMove = !m_bMove;
	}

	if (m_bMove)
		Move();
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