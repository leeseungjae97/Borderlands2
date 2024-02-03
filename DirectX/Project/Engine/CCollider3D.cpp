#include "pch.h"
#include "CCollider3D.h"

#include "CScript.h"
#include "CTransform.h"
#include "PhysXMgr.h"
#include "physx_util.h"

CCollider3D::CCollider3D(bool _AttachRigid)
	: CComponent(COMPONENT_TYPE::COLLIDER3D)
	, m_PxColliderShape(nullptr)
	, m_bFirstInit(false)
	, m_tColliderShapeType(COLLIDER_SHAPE_TYPE::BOX)
	, m_bAttachToRigidBody(_AttachRigid)
	, m_vScale(Vec3(1.f, 1.f, 1.f))
{
	m_PxMaterial = PhysXMgr::GetInst()->GPhysics()->createMaterial(0.0f, 0.0f, 0.0f);

	//m_PxMaterial->setFrictionCombineMode((PxCombineMode::eAVERAGE));
	//m_PxMaterial->setRestitutionCombineMode((PxCombineMode::eAVERAGE));
}

CCollider3D::~CCollider3D()
{
	//m_PxRigidBodyShape->release();
	if(m_PxColliderRigid)
	{
		m_PxColliderRigid->userData = nullptr;
		PX_RELEASE(m_PxColliderRigid);
	}
	
	//PX_RELEASE(m_PxRigidBodyShape);
}

void CCollider3D::finaltick()
{
	setShapeToRigidBody();
	//if (CLevelMgr::GetInst()->GetCurLevel()->GetState() != LEVEL_STATE::PLAY)

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
	CRigidBody* _rb = GetOwner()->RigidBody();

	if (!m_bAttachToRigidBody && nullptr == _rb
		|| !_rb->IsRigidBodyCreate()) return;

	if (m_bFirstInit) return;

	if (!m_bFirstInit)
	{
		m_bFirstInit = true;
	}

	createColliderShape();

	if(nullptr == _rb)
	{
		Vec3 vPos = GetOwner()->Transform()->GetRelativePos();
		Vec3 vRot = GetOwner()->Transform()->GetRelativeRot();

		Quat quat; quat = Vector3ToQuaternion(vRot);

		PxTransform localTm(PxVec3(vPos.x, vPos.y, vPos.z), PxQuat(quat.x, quat.y, quat.z, quat.w));

		m_PxColliderRigid = PhysXMgr::GetInst()->GPhysics()->createRigidStatic(localTm);

		m_PxColliderRigid->userData = this;

		m_PxColliderRigid->attachShape(*m_PxColliderShape);
		PhysXMgr::GetInst()->GCurScene()->addActor(*m_PxColliderRigid);
	}else
	{
		_rb->AttachShape(m_PxColliderShape);
	}

	//m_PxColliderShape->release();
		
}

void CCollider3D::createColliderShape()
{
	//CRigidBody* _rb = GetOwner()->RigidBody();

	//if(m_vScale == Vec3(1.f, 1.f, 1.f))
	//{
	//	if (_rb)
	//		m_vScale = GetOwner()->RigidBody()->GetRigidScale();
	//}
	
	m_PxColliderShape = createTriggerShape(PxBoxGeometry(m_vScale.x, m_vScale.y, m_vScale.z), *m_PxMaterial, true);
	PxFilterData triggerFilterData(0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
	m_PxColliderShape->setSimulationFilterData(triggerFilterData);
}

void CCollider3D::colliderDebugDraw()
{
	CRigidBody* _rb = GetOwner()->RigidBody();
	
	if (nullptr == _rb && nullptr == m_PxColliderRigid) return;

	physx::PxTransform pos;
	if (_rb)
		pos = GetOwner()->RigidBody()->GetRigidBodyPos();
	else
		pos = m_PxColliderRigid->getGlobalPose();

	Matrix worldMat = physx::Util::WorldMatFromGlobalPose(pos
		, Vec3(m_vScale.x
					, m_vScale.y
					, m_vScale.z)
	);
	DrawDebugCube(worldMat, Vec4(0.f, 1.f, 0.f, 1.f), 0.f, true);
}

void CCollider3D::LoadFromLevelFile(FILE* _FILE)
{
	fread(&m_vScale, sizeof(Vec3), 1, _FILE);
	fread(&m_bAttachToRigidBody, sizeof(bool), 1, _FILE);
}

void CCollider3D::SaveToLevelFile(FILE* _File)
{
	fwrite(&m_vScale, sizeof(Vec3), 1, _File);
	fwrite(&m_bAttachToRigidBody, sizeof(bool), 1, _File);
}
