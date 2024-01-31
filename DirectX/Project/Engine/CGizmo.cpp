#include "pch.h"
#include "CGizmo.h"

#include "CTransform.h"

CGizmo::CGizmo()
	: CComponent(COMPONENT_TYPE::GIZMO)
	, m_pUp(nullptr)
	, m_pRight(nullptr)
	, m_pFront(nullptr)
{
	//m_pFront = new CGameObject();
}

CGizmo::~CGizmo()
{
}

void CGizmo::finaltick()
{
	Vec3 vFront = GetOwner()->Transform()->GetRelativeDir(DIR_TYPE::FRONT);
	Vec3 vUp = GetOwner()->Transform()->GetRelativeDir(DIR_TYPE::UP);
	Vec3 vRight = GetOwner()->Transform()->GetRelativeDir(DIR_TYPE::RIGHT);

	Vec3 vRot = GetOwner()->Transform()->GetRelativeRot();
	Vec3 vPos = GetOwner()->Transform()->GetRelativePos();
	Matrix matWorld = GetOwner()->Transform()->GetWorldMat();
	Vec3 vScale = GetOwner()->Transform()->GetRelativeScale();

	float fMax = fmax(vScale.x, vScale.y);
	fMax = fmax(fMax, vScale.z);

	//DrawDebugLine(DIR_TYPE::UP, vPos, fMax, Vec4(1.f, 0.f, 0.f, 1.f), vFront);
	//DrawDebugLine(DIR_TYPE::UP, vPos, fMax, Vec4(0.f, 1.f, 0.f, 1.f), vRight);
	//DrawDebugLine(DIR_TYPE::UP, vPos, fMax, Vec4(0.f, 0.f, 1.f, 1.f), Vec3(0.f, 0.f ,0.f));
	//DrawDebugLine(DIR_TYPE::UP, vPos, fMax, Vec4(0.f, 0.f, 1.f, 1.f), vUp);
	//DrawDebugLine(DIR_TYPE::FRONT, vPos, fMax, Vec4(1.f, 0.f, 0.f, 1.f), vFront);
	//DrawDebugLine(DIR_TYPE::RIGHT, vPos, fMax, Vec4(1.f, 1.f, 0.f, 1.f), vRight);

	DrawDebugLine(DIR_TYPE::UP, matWorld, Vec4(0.f, 0.f, 1.f, 1.f));
	DrawDebugLine(DIR_TYPE::FRONT, matWorld, Vec4(1.f, 0.f, 0.f, 1.f));
	DrawDebugLine(DIR_TYPE::RIGHT, matWorld, Vec4(1.f, 1.f, 0.f, 1.f));
}

void CGizmo::LoadFromLevelFile(FILE* _FILE)
{
}

void CGizmo::SaveToLevelFile(FILE* _File)
{
}
