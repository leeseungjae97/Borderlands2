#include "pch.h"
#include "CCollider3D.h"

#include "CScript.h"
#include "CTransform.h"
#include "PhysXMgr.h"
#include "physx_util.h"

CCollider3D::CCollider3D(bool _AttachRigid, bool _Intergrated)
	: CComponent(COMPONENT_TYPE::COLLIDER3D)
	, m_PxColliderShape(nullptr)
	, m_bFirstInit(false)
	, m_bCenter(false)
	, m_tColliderShapeType(COLLIDER_SHAPE_TYPE::BOX)
	, m_bAttachToRigidBody(_AttachRigid)
	, m_vScale(Vec3(1.f, 1.f, 1.f))
	, m_bBeginOverlap(false)
	, m_bOnOverlap(false)
	, m_bEndOverlap(false)
	, m_bIntergrated(_Intergrated)
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

Matrix CCollider3D::GetColliderWorldMat()
{
	PxTransform trans = m_PxColliderRigid->getGlobalPose();
	
	return physx::Util::WorldMatFromGlobalPose(trans, m_vScale);
}

void CCollider3D::EndOverlap(CCollider3D* _OhterCol)
{
	m_bBeginOverlap = false;
	m_bOnOverlap = false;
	m_bEndOverlap = true;

	for (auto script : GetOwner()->GetScripts())
	{
		script->EndOverlap(_OhterCol);
	}
}

void CCollider3D::OnOverlap(CCollider3D* _OhterCol)
{
	m_bBeginOverlap = false;
	m_bOnOverlap = true;
	m_bEndOverlap = false;

	for (auto script : GetOwner()->GetScripts())
	{
		script->OnOverlap(_OhterCol);
	}
}

void CCollider3D::BeginOverlap(CCollider3D* _OhterCol)
{
	m_bBeginOverlap = true;
	m_bOnOverlap = false;
	m_bEndOverlap = false;

	for (auto script : GetOwner()->GetScripts())
	{
		script->BeginOverlap(_OhterCol);
	}
}

void CCollider3D::Raycast(tRayInfo _RaycastInfo)
{
	for (auto script : GetOwner()->GetScripts())
	{
		script->Raycast(_RaycastInfo);
	}
}

void CCollider3D::setShapeToRigidBody()
{
	CRigidBody* _rb = GetOwner()->RigidBody();

	if (m_bAttachToRigidBody && (nullptr == _rb || !_rb->IsRigidBodyCreate())) return;


	if (m_bFirstInit) return;

	if (!m_bFirstInit)
	{
		m_bFirstInit = true;
	}

	createColliderShape();

	if(!m_bAttachToRigidBody)
	{
		Vec3 vPos = Vec3::Zero;
		Vec3 vRot = Vec3::Zero;
		if(!m_bIntergrated)
		{
			vPos = GetOwner()->Transform()->GetRelativePos();
			vRot = GetOwner()->Transform()->GetRelativeRot();
		}
		
		//Vec3 vScale = GetOwner()->Transform()->GetRelativeScale();

		Quat quat; quat = Vector3ToQuaternion(vRot);
		//if (m_bCenter)
		//{
		//	m_vOffset.z = vScale.z / 2.f;
		//	vPos.z += m_vOffset.z;
		//}

		PxTransform localTm(PxVec3(vPos.x, vPos.y, vPos.z), PxQuat(quat.x, quat.y, quat.z, quat.w));

		m_PxColliderRigid = PhysXMgr::GetInst()->GPhysics()->createRigidStatic(localTm);

		m_PxColliderRigid->userData = this;
		m_PxColliderRigid->setActorFlags(PxActorFlag::eDISABLE_GRAVITY);
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
	CRigidBody* _rb = GetOwner()->RigidBody();

	if(m_vScale == Vec3(1.f, 1.f, 1.f))
	{	if(GetOwner()->Animator3D())
		{
			m_vScale = GetOwner()->Transform()->GetRelativeScale();
			//Matrix matWorld = GetOwner()->Transform()->GetWorldMat();
			//Vec4 headPos = XMVector3TransformCoord(GetOwner()->Animator3D()->GetMeshHeadPosition(), matWorld);
			//m_vScale.y = 100.f / 2.f;
		}
			
		else if (_rb && m_bAttachToRigidBody)
			m_vScale = GetOwner()->RigidBody()->GetRigidScale();
	}
	
	//m_PxColliderShape = createTriggerShape(PxBoxGeometry(m_vScale.x + 2, m_vScale.y + 2, m_vScale.z + 2), *m_PxMaterial, true);
	m_PxColliderShape = PhysXMgr::GetInst()->GPhysics()->createShape(
		physx::PxBoxGeometry(m_vScale.x / 2.f, m_vScale.y / 2.f, m_vScale.z / 2.f)
		, *m_PxMaterial
		, true);
	PxFilterData triggerFilterData;
	if(GetOwner()->GetLayerIndex() == (int)LAYER_TYPE::NoRaycastingCollider)
	{
		//triggerFilterData.word0 = 0x0000000f;
		//triggerFilterData.word1 = 0xf000000f;
		triggerFilterData.word0 = (1 << 1);
		triggerFilterData.word1 = 0x0000000f;
		triggerFilterData.word2 = 0x0000000f;
		triggerFilterData.word3 = 0x0000000f;
	}else
	{
		triggerFilterData.word0 = (1 << 0);
		triggerFilterData.word1 = 0x0000000f;
		triggerFilterData.word2 = 0x0000000f;
		triggerFilterData.word3 = 0x0000000f;
	}
	
	m_PxColliderShape->setSimulationFilterData(triggerFilterData);
	m_PxColliderShape->setQueryFilterData(triggerFilterData);
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

void CCollider3D::SetColliderPos(Vec3 _vPos)
{
	//_vPos += m_vOffset;

	if (m_PxColliderRigid)
	{
		PxTransform trans = m_PxColliderRigid->getGlobalPose();
		trans.p = PxVec3(_vPos.x, _vPos.y, _vPos.z);
		m_PxColliderRigid->setGlobalPose(trans);
	}
}

void CCollider3D::SetColliderRot(Vec3 _vRot)
{
	if (m_PxColliderRigid)
	{
		PxTransform trans = m_PxColliderRigid->getGlobalPose();
		Quat quat; quat = Util::Vector3ToQuaternion(_vRot);
		trans.q = PxQuat(quat.x, quat.y, quat.z, quat.w);
		m_PxColliderRigid->setGlobalPose(trans);
	}
}

void CCollider3D::LoadFromLevelFile(FILE* _FILE)
{
	fread(&m_vScale, sizeof(Vec3), 1, _FILE);
	fread(&m_vOffset, sizeof(Vec3), 1, _FILE);
	fread(&m_bAttachToRigidBody, sizeof(bool), 1, _FILE);
	fread(&m_bIntergrated, sizeof(bool), 1, _FILE);
	fread(&m_bCenter, sizeof(bool), 1, _FILE);
}

void CCollider3D::SaveToLevelFile(FILE* _File)
{
	fwrite(&m_vScale, sizeof(Vec3), 1, _File);
	fwrite(&m_vOffset, sizeof(Vec3), 1, _File);
	fwrite(&m_bAttachToRigidBody, sizeof(bool), 1, _File);
	fwrite(&m_bIntergrated, sizeof(bool), 1, _File);
	fwrite(&m_bCenter, sizeof(bool), 1, _File);
}
