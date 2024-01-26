#include "pch.h"
#include "CRigidBody.h"

#include "CTransform.h"
#include "PhysXMgr.h"
#include "physx_util.h"

CRigidBody::CRigidBody()
	: CComponent(COMPONENT_TYPE::RIGIDBODY)
	, m_PxColliderShape(nullptr)
	, m_fColliderX(10.f)
	, m_fColliderY(10.f)
	, m_fColliderZ(10.f)
{
	m_PxMaterial = PhysXMgr::GetInst()->GPhysics()->createMaterial(0.5f, 0.5f, 0.6f);

	m_PxMaterial->setFrictionCombineMode((PxCombineMode::eAVERAGE));
	m_PxMaterial->setRestitutionCombineMode((PxCombineMode::eAVERAGE));
}

CRigidBody::~CRigidBody()
{
}

void CRigidBody::addToScene()
{
	Vec3 vPos = GetOwner()->Transform()->GetRelativePos();
	Vec3 vScale = GetOwner()->Transform()->GetRelativeScale();

	//m_PxColliderShape = createTriggerShape(physx::PxBoxGeometry(vScale.x, vScale.y, vScale.z), *m_PxMaterial, Util::FILTER_SHADER, true);
	m_PxColliderShape = PhysXMgr::GetInst()->GPhysics()->createShape(physx::PxBoxGeometry(vScale.x, vScale.y, vScale.z), *m_PxMaterial, true);
	PxTransform localTm(PxVec3(vPos.x, vPos.y, vPos.z));
	m_RigidBody = PhysXMgr::GetInst()->GPhysics()->createRigidDynamic(localTm);
	m_RigidBody->attachShape(*m_PxColliderShape);

	PhysXMgr::GetInst()->GScene()->addActor(*m_RigidBody);
}

void CRigidBody::setOwnerPosToPx()
{
}

void CRigidBody::rigidDebugDraw()
{
	if (nullptr == m_RigidBody) return;

	Vec3 vScale = GetOwner()->Transform()->GetRelativeScale();
	Matrix worldMat = physx::Util::WorldMatFromGlobalPose(m_RigidBody->getGlobalPose(), vScale);
	DrawDebugCube(worldMat, Vec4(1.f, 0.f, 0.f, 1.f), 0.f, true);
}

void CRigidBody::finaltick()
{
	static bool m = false;
	if (!m)
	{
		addToScene();
		setOwnerPosToPx();
		m = true;
	}

	rigidDebugDraw();
}

void CRigidBody::LoadFromLevelFile(FILE* _FILE)
{
}

void CRigidBody::SaveToLevelFile(FILE* _File)
{
}
