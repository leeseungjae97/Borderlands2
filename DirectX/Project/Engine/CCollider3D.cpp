#include "pch.h"
#include "CCollider3D.h"

#include "CScript.h"
#include "CTransform.h"
#include "PhysXMgr.h"
#include "physx_util.h"

CCollider3D::CCollider3D()
	: CComponent(COMPONENT_TYPE::COLLIDER3D)
	, m_PxColliderShape(nullptr)
	, m_fColliderX(10.f)
	, m_fColliderY(10.f)
	, m_fColliderZ(10.f)
{
	// 마찰 계수 설정
	// https://nvidia-omniverse.github.io/PhysX/physx/5.3.1/_api_build/class_px_physics.html?highlight=createMaterial#_CPPv4N9PxPhysics14createMaterialE6PxReal6PxReal6PxReal
	m_PxMaterial= PhysXMgr::GetInst()->GPhysics()->createMaterial(0.5f, 0.5f, 0.6f);

	m_PxMaterial->setFrictionCombineMode((PxCombineMode::eAVERAGE));
	m_PxMaterial->setRestitutionCombineMode((PxCombineMode::eAVERAGE));
}

CCollider3D::~CCollider3D()
{
	//m_PxColliderShape->release();
}

void CCollider3D::addToScene()
{
	Vec3 vPos = GetOwner()->Transform()->GetRelativePos();
	Vec3 vScale = GetOwner()->Transform()->GetRelativeScale();

	m_PxColliderShape = createTriggerShape(PxBoxGeometry(vScale.x, vScale.y, vScale.z), *m_PxMaterial, REAL_TRIGGERS, true);

	PxTransform localTm(PxVec3(vPos.x, vPos.y, vPos.z));
	m_RigidBody = PhysXMgr::GetInst()->GPhysics()->createRigidDynamic(localTm);
	m_RigidBody->attachShape(*m_PxColliderShape);

	//PxRigidActorExt::
	//PhysXMgr::GetInst()->create
	//PxRigidBodyExt::updateMassAndInertia(*m_RigidBody, 10.0f);

	PhysXMgr::GetInst()->GScene()->addActor(*m_RigidBody);
}

void CCollider3D::setOwnerPosToPx()
{
	//Vec3 vPos = GetOwner()->Transform()->GetRelativePos();
	//Vec3 vRot = GetOwner()->Transform()->GetRelativeRot();
	//FXMVECTOR vVRot = vRot;
	//Quaternion rotation = vVRot;
	//// PxTransform : 유클리드 변환을 쿼터니언과 벡터로 나타내는 클래스
	//physx::PxTransform t(
	//	PxVec3(vPos.x, vPos.y, vPos.z),
	//	PxQuat(rotation.x, rotation.y, rotation.z, rotation.w)
	//);
	//m_RigidBody->setGlobalPose(t);
}

void CCollider3D::colliderDebugDraw()
{
	if (nullptr == m_RigidBody) return;

	Vec3 vScale = GetOwner()->Transform()->GetRelativeScale();
	Matrix worldMat = physx::Util::WorldMatFromGlobalPose(m_RigidBody->getGlobalPose(), vScale);
	DrawDebugCube(worldMat, Vec4(0.f, 1.f, 0.f, 1.f), 0.f, true);
	//DrawDebugCube(Transform()->GetWorldMat(), Vec4(0.f, 1.f, 0.f, 1.f), 0.f, true);
}

void CCollider3D::finaltick()
{
	static bool m = false;
	if(!m)
	{
		addToScene();
		setOwnerPosToPx();
		m = true;
	}

	colliderDebugDraw();
}

void CCollider3D::EndOverlap(CCollider3D* _OhterCol)
{
	for(auto script : GetOwner()->GetScripts())
	{
		script->EndOverlap(_OhterCol);
	}
}

void CCollider3D::OnOverlap(CCollider3D* _OhterCol)
{
	for (auto script : GetOwner()->GetScripts())
	{
		script->OnOverlap(_OhterCol);
	}
}

void CCollider3D::BeginOverlap(CCollider3D* _OhterCol)
{
	for (auto script : GetOwner()->GetScripts())
	{
		script->BeginOverlap(_OhterCol);
	}
}

void CCollider3D::LoadFromLevelFile(FILE* _FILE)
{
}

void CCollider3D::SaveToLevelFile(FILE* _File)
{
}
