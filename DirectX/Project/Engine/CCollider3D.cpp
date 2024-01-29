#include "pch.h"
#include "CCollider3D.h"

#include "CScript.h"
#include "CTransform.h"
#include "PhysXMgr.h"
#include "physx_util.h"

CCollider3D::CCollider3D()
	: CComponent(COMPONENT_TYPE::COLLIDER3D)
	, m_PxColliderShape(nullptr)
	, m_fColliderX(1.f)
	, m_fColliderY(1.f)
	, m_fColliderZ(1.f)
{
	// 마찰 계수 설정
	// https://nvidia-omniverse.github.io/PhysX/physx/5.3.1/_api_build/class_px_physics.html?highlight=createMaterial#_CPPv4N9PxPhysics14createMaterialE6PxReal6PxReal6PxReal
	m_PxMaterial = PhysXMgr::GetInst()->GPhysics()->createMaterial(0.5f, 0.5f, 0.6f);

	m_PxMaterial->setFrictionCombineMode((PxCombineMode::eAVERAGE));
	m_PxMaterial->setRestitutionCombineMode((PxCombineMode::eAVERAGE));
}

CCollider3D::~CCollider3D()
{
	//m_PxRigidBodyShape->release();
}

void CCollider3D::initialize()
{
	createColliderShape();
}

void CCollider3D::finaltick()
{
	static bool m = false;
	if (!m)
	{
		setShapeToRigidBody();
		m = true;
	}
	if (CLevelMgr::GetInst()->GetCurLevel()->GetState() != LEVEL_STATE::PLAY)
		colliderDebugDraw();
}

void CCollider3D::EndOverlap(CCollider3D* _OhterCol)
{
	for (auto script : GetOwner()->GetScripts())
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

void CCollider3D::setShapeToRigidBody()
{
	CRigidBody* _db= GetOwner()->RigidBody();
	_db->AttachShape(m_PxColliderShape);
}

void CCollider3D::createColliderShape()
{
	Vec3 vScale = GetOwner()->RigidBody()->GetRigidScale();
	
	m_PxColliderShape = createTriggerShape(PxBoxGeometry(vScale.x, vScale.y, vScale.z), *m_PxMaterial, REAL_TRIGGERS, true);
}

void CCollider3D::colliderDebugDraw()
{
	physx::PxTransform pos = GetOwner()->RigidBody()->GetRigidBodyPos();

	Matrix worldMat = physx::Util::WorldMatFromGlobalPose(pos
		, Vec3(m_fColliderX
					, m_fColliderY
					, m_fColliderZ)
	);
	DrawDebugCube(worldMat, Vec4(0.f, 1.f, 0.f, 1.f), 0.f, true);
}

void CCollider3D::LoadFromLevelFile(FILE* _FILE)
{
}

void CCollider3D::SaveToLevelFile(FILE* _File)
{
}
