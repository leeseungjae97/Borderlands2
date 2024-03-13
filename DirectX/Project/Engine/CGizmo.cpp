#include "pch.h"
#include "CGizmo.h"

#include "CCollider3D.h"
#include "CTransform.h"

CGizmo::CGizmo(bool _UsePhysxPos)
	: CComponent(COMPONENT_TYPE::GIZMO)
	, m_bUsePhysxPos(_UsePhysxPos)
{
}

CGizmo::~CGizmo()
{
}

void CGizmo::finaltick()
{
	Matrix matWorld = XMMatrixIdentity();
	if (m_bUsePhysxPos)
	{
		matWorld = GetOwner()->Collider3D()->GetColliderWorldMat();
	}
	else
	{
		matWorld = GetOwner()->Transform()->GetWorldMat();
	}

	DrawDebugLine(DIR_TYPE::UP, matWorld, Vec4(0.f, 0.f, 1.f, 1.f));
	DrawDebugLine(DIR_TYPE::FRONT, matWorld, Vec4(1.f, 0.f, 0.f, 1.f));
	DrawDebugLine(DIR_TYPE::RIGHT, matWorld, Vec4(1.f, 1.f, 0.f, 1.f));
}

void CGizmo::LoadFromLevelFile(FILE* _FILE)
{
	fread(&m_bUsePhysxPos, sizeof(bool), 1, _FILE);
}

void CGizmo::SaveToLevelFile(FILE* _File)
{
	fwrite(&m_bUsePhysxPos, sizeof(bool), 1, _File);
}

