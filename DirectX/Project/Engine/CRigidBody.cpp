#include "pch.h"
#include "CRigidBody.h"

#include "CLevel.h"
#include "CLevelMgr.h"
#include "CMeshRender.h"
#include "CTransform.h"
#include "physx_util.h"

//CRigidBody::CRigidBody()
//	: CComponent(COMPONENT_TYPE::RIGIDBODY)
//{
//	//m_tRigidShapeType = RIGID_BODY_SHAPE_TYPE::BOX;
//	//m_tRigidType = RIGID_BODY_TYPE::STATIC;
//	if(nullptr == m_pMaterial)
//		m_pMaterial = PhysXMgr::GetInst()->GPhysics()->createMaterial(1.0f, 1.0f, 0.1f);
//}

CRigidBody::CRigidBody(RIGID_BODY_SHAPE_TYPE _Type, RIGID_BODY_TYPE _Type2)
	: CComponent(COMPONENT_TYPE::RIGIDBODY)
	, m_tRigidShapeType(_Type)
	, m_tRigidType(_Type2)
	, m_vRigidScale(Vec3(1.f, 1.f, 1.f))
	, m_bCreature(false)
	, m_bInit(false)
	, m_bInitData(false)
	, m_debugMeshName(L"")
{
	m_pMaterial = PhysXMgr::GetInst()->GPhysics()->createMaterial(1.0f, 1.0f, 0.1f);
	m_pMaterial->setDamping(0.f);
	if (m_tRigidType == RIGID_BODY_TYPE::DYNAMIC)
	{
		//m_pMaterial->setFrictionCombineMode(static_cast<physx::PxCombineMode::Enum>(physx::PxCombineMode::eMAX));
		//m_pMaterial->setRestitutionCombineMode(static_cast<physx::PxCombineMode::Enum>(physx::PxCombineMode::eMAX));
	}
}

CRigidBody::~CRigidBody()
{
	if(m_pDynamicBody)
	{
		m_pDynamicBody->userData = nullptr;
		PX_RELEASE(m_pDynamicBody);
	}
	
	if(m_pStaticBody)
	{
		m_pStaticBody->userData = nullptr;
		PX_RELEASE(m_pStaticBody);
	}
		
}

void CRigidBody::finaltick()
{
	if(!m_bInit)
	{
		addToScene();
		m_bInit = true;
	}
	if(!m_bInitData)
	{
		setUserData();
	}
	

	drawDebugRigid();
}

PxTransform CRigidBody::GetRigidBodyPos()
{
	if (m_pDynamicBody)
		return m_pDynamicBody->getGlobalPose();
	if (m_pStaticBody)
		return m_pStaticBody->getGlobalPose();

	return PxTransform();
}

void CRigidBody::AttachShape(PxShape* _Attach)
{
	if (m_pDynamicBody)
		m_pDynamicBody->attachShape(*_Attach);
	if (m_pStaticBody)
		m_pStaticBody->attachShape(*_Attach);
}

void CRigidBody::SetRigidBodyTrans(const PxTransform& trans)
{
	if (m_pDynamicBody)
		m_pDynamicBody->setGlobalPose(trans);
	if (m_pStaticBody)
		m_pStaticBody->setGlobalPose(trans);
}

void CRigidBody::SetLinearVelocity(Vec3 _Velocity)
{
	if (m_tRigidType == RIGID_BODY_TYPE::STATIC) return;

	PxVec3 prev_velocity = m_pDynamicBody->getLinearVelocity();
	
	_Velocity.y = prev_velocity.y < 0 ? _Velocity.y + prev_velocity.y : prev_velocity.y;

	m_pDynamicBody->setLinearVelocity(PxVec3(_Velocity.x, _Velocity.y, _Velocity.z));
}


void CRigidBody::SetLinearVelocityZero()
{
	if (m_tRigidType == RIGID_BODY_TYPE::STATIC) return;
	m_pDynamicBody->setLinearVelocity(PxVec3(0.f, 0.f, 0.f));
}

void CRigidBody::SetAngularVelocity(Vec3 _Angular)
{
	if (m_tRigidType == RIGID_BODY_TYPE::STATIC) return;

	m_pDynamicBody->setAngularVelocity(PxVec3(_Angular.x, _Angular.y, _Angular.z));
}

void CRigidBody::SetAngularVelocityZero()
{
}

bool CRigidBody::IsRigidBodyCreate()
{
	if (m_pDynamicBody)
		return true;
	if (m_pStaticBody)
		return true;

	return false;
}

void CRigidBody::convertMeshToGeom()
{
	Vtx v; PxU32 idx; PxVec3 pV; vector<UINT> inds;
	Ptr<CMesh> pMesh = GetOwner()->MeshRender()->GetMesh();
	Vec3 vScale = GetOwner()->Transform()->GetRelativeScale();
	m_debugMeshName = pMesh->GetKey();
	for (auto indexInfo : pMesh->GetIndexInfo())
	{
		inds.clear();
		vector<UINT>().swap(inds);

		inds.resize(indexInfo.iIdxCount);
		memcpy(inds.data(), indexInfo.pIdxSysMem, indexInfo.iIdxCount * sizeof(UINT));
		for (int idxIdx = 0; idxIdx < indexInfo.iIdxCount; ++idxIdx)
		{
			idx = inds[idxIdx];
			m_vecIndis.push_back(idx);
		}
	}

	Vtx* vecVtx = pMesh->GetVtxSysMem();
	for (int vtxIdx = 0; vtxIdx < pMesh->GetVtxCount(); ++vtxIdx)
	{
		v = vecVtx[vtxIdx];
		pV = PxVec3(v.vPos.x * vScale.x, v.vPos.y * vScale.y, v.vPos.z * vScale.z);
		m_vecVerts.push_back(pV);
	}
}

void CRigidBody::setRigidPos()
{
	Vec3 vPos = GetOwner()->Transform()->GetRelativePos();
	Vec3 vScale = GetOwner()->Transform()->GetRelativeScale();
	Vec3 vRot = GetOwner()->Transform()->GetRelativeRot();

	Quat quat; quat = Util::Vector3ToQuaternion(vRot);

	PxTransform localTm(PxVec3(vPos.x, vPos.y, vPos.z), PxQuat(quat.x, quat.y, quat.z, quat.w));

	if (m_pDynamicBody)
	{
		m_pDynamicBody->setGlobalPose(localTm);
		m_pDynamicBody->setLinearVelocity(PxVec3(0.f, 0.f, 0.f));
	}
	if (m_pStaticBody)
		m_pStaticBody->setGlobalPose(localTm);

	
}

void CRigidBody::createShape()
{
	m_vRigidScale = m_vRigidScale == Vec3(1.f, 1.f, 1.f) ? GetOwner()->Transform()->GetRelativeScale() : m_vRigidScale;

	if (m_tRigidShapeType == RIGID_BODY_SHAPE_TYPE::BOX)
	{
		Vec3 vHalfScale = m_vRigidScale / 2.f;
		m_pShape = PhysXMgr::GetInst()->GPhysics()->createShape(
			physx::PxBoxGeometry(vHalfScale.x, vHalfScale.y, vHalfScale.z)
			, *m_pMaterial
			, true);
	}
	else if (m_tRigidShapeType == RIGID_BODY_SHAPE_TYPE::RECT)
	{
		Vec3 vHalfScale = m_vRigidScale / 2.f;
		m_pShape = PhysXMgr::GetInst()->GPhysics()->createShape(
			physx::PxBoxGeometry(vHalfScale.x, vHalfScale.z, vHalfScale.y)
			, *m_pMaterial
			, true);
	}
	else if (m_tRigidShapeType == RIGID_BODY_SHAPE_TYPE::SPHERE)
	{
		//float _fMax = fmax(m_vRigidScale.x, m_vRigidScale.y);
		//_fMax = fmax(_fMax, m_vRigidScale.z);
		m_pShape = PhysXMgr::GetInst()->GPhysics()->createShape(
			physx::PxSphereGeometry(m_vRigidScale.x / 2.f)
			, *m_pMaterial
			, true);
	}
}

void CRigidBody::createTriangleMesh()
{
	convertMeshToGeom();
	PxCookingParams params(PhysXMgr::GetInst()->GPhysics()->getTolerancesScale());
	params.midphaseDesc.setToDefault(PxMeshMidPhase::eBVH34);
	params.meshPreprocessParams |= PxMeshPreprocessingFlag::eDISABLE_ACTIVE_EDGES_PRECOMPUTE;
	params.meshPreprocessParams |= PxMeshPreprocessingFlag::eDISABLE_CLEAN_MESH;

	PxTriangleMeshDesc triDesc;
	triDesc.points.count = m_vecVerts.size();
	triDesc.points.stride = sizeof(PxVec3);
	triDesc.points.data = m_vecVerts.data();

	triDesc.triangles.count = m_vecIndis.size() / 3;
	triDesc.triangles.stride = sizeof(PxU32) * 3;
	triDesc.triangles.data = m_vecIndis.data();

	PxTriangleMesh* triMesh = PxCreateTriangleMesh(params, triDesc);
	m_pShape = PhysXMgr::GetInst()->GPhysics()->createShape(PxTriangleMeshGeometry(triMesh), *m_pMaterial, true);

	PX_RELEASE(triMesh);
}

void CRigidBody::addToScene()
{
	Vec3 vPos = GetOwner()->Transform()->GetRelativePos();
	Vec3 vRot = GetOwner()->Transform()->GetRelativeRot();

	Quat quat; quat = Util::Vector3ToQuaternion(vRot);

	PxTransform localTm(PxVec3(vPos.x, vPos.y, vPos.z), PxQuat(quat.x, quat.y, quat.z, quat.w));

	if (m_tRigidShapeType == RIGID_BODY_SHAPE_TYPE::MESH)
	{
		createTriangleMesh();
	}
	else
	{
		createShape();
	}

	//PxFilterData triggerFilterData(GetOwner()->GetLayerIndex(), 0, 0, 0);
	//m_pShape->setSimulationFilterData(triggerFilterData);

	if (m_tRigidType == RIGID_BODY_TYPE::DYNAMIC)
	{
		// RigidBody로 TriangleMesh를 쓸 수 없음
		m_pDynamicBody = PhysXMgr::GetInst()->GPhysics()->createRigidDynamic(localTm);
		m_pDynamicBody->userData = GetOwner()->Collider3D();
		m_pDynamicBody->attachShape(*m_pShape);

		//m_pDynamicBody->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
		//m_pDynamicBody->setLinearDamping(10.f);
		PxRigidBodyExt::updateMassAndInertia(*m_pDynamicBody, 0.1f);
		PhysXMgr::GetInst()->GCurScene()->addActor(*m_pDynamicBody);
	}
	else
	{
		if (m_tRigidShapeType == RIGID_BODY_SHAPE_TYPE::MESH)
		{
			//TODO : TriangleMesh 처리
		}
		
		m_pStaticBody = PhysXMgr::GetInst()->GPhysics()->createRigidStatic(localTm);
		m_pStaticBody->userData = GetOwner()->Collider3D();
		m_pStaticBody->attachShape(*m_pShape);
		PhysXMgr::GetInst()->GCurScene()->addActor(*m_pStaticBody);
	}

}

void CRigidBody::setUserData()
{
	if (nullptr == GetOwner()->Collider3D())
		return;

	if(m_pStaticBody)
		m_pStaticBody->userData = GetOwner()->Collider3D();
	if(m_pDynamicBody)
		m_pDynamicBody->userData = GetOwner()->Collider3D();

	m_bInitData = true;
}

void CRigidBody::drawDebugRigid()
{
	//if (CLevelMgr::GetInst()->GetCurLevel()->GetState() == LEVEL_STATE::PLAY)
	//	return;

	Vec3 vScale = GetOwner()->Transform()->GetRelativeScale();
	PxTransform trans;
	if (m_pDynamicBody)
		trans = m_pDynamicBody->getGlobalPose();
	else
		trans = m_pStaticBody->getGlobalPose();

	Matrix worldMat = physx::Util::WorldMatFromGlobalPose(trans, vScale);
	switch (m_tRigidShapeType)
	{
	case RIGID_BODY_SHAPE_TYPE::BOX:
	case RIGID_BODY_SHAPE_TYPE::RECT:
	{
		DrawDebugCube(worldMat, Vec4(1.f, 1.f, 1.f, 1.f), 0.f, true);
	}
	break;
	case RIGID_BODY_SHAPE_TYPE::SPHERE:
	{
		DrawDebugSphere(worldMat, Vec4(1.f, 1.f, 1.f, 1.f), 0.f, true);
	}
	break;
	case RIGID_BODY_SHAPE_TYPE::MESH:
	{
		DrawDebugMesh(worldMat, m_debugMeshName, Vec4(1.f, 1.f, 1.f, 1.f), 0.f, true);
	}
	break;
	}
}

void CRigidBody::LoadFromLevelFile(FILE* _FILE)
{
	fread(&m_tRigidShapeType, sizeof(UINT), 1, _FILE);

	if(m_tRigidShapeType == RIGID_BODY_SHAPE_TYPE::MESH)
		LoadWString(m_debugMeshName, _FILE);
	
	fread(&m_tRigidType, sizeof(UINT), 1, _FILE);

	fread(&m_vRigidScale, sizeof(Vec3), 1, _FILE);

	fread(&m_bCreature, sizeof(bool), 1, _FILE);
}

void CRigidBody::SaveToLevelFile(FILE* _File)
{
	fwrite(&m_tRigidShapeType, sizeof(UINT), 1, _File);

	if (m_tRigidShapeType == RIGID_BODY_SHAPE_TYPE::MESH)
		SaveWString(m_debugMeshName, _File);

	UINT rigidType = (UINT)m_tRigidType;
	fwrite(&rigidType, sizeof(UINT), 1, _File);

	fwrite(&m_vRigidScale, sizeof(Vec3), 1, _File);

	fwrite(&m_bCreature, sizeof(bool), 1, _File);
}