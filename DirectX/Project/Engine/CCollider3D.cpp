#include "pch.h"
#include "CCollider3D.h"

#include "CScript.h"
#include "CTransform.h"
#include "PhysXMgr.h"
#include "physx_util.h"

CCollider3D::CCollider3D(bool _AttachRigid, bool _Unity, COLLIDER_SHAPE_TYPE _Shape)
	: CComponent(COMPONENT_TYPE::COLLIDER3D)
	, m_PxColliderShape(nullptr)
	, m_bFirstInit(false)
	, m_bCenter(false)
	, m_bRaycastable(true)
	, m_tColliderShapeType(_Shape)
	, m_bAttachToRigidBody(_AttachRigid)
	, m_vScale(Vec3(1.f, 1.f, 1.f))
	, m_bBeginOverlap(false)
	, m_bOnOverlap(false)
	, m_bEndOverlap(false)
	, m_bRaycast(false)
	, m_bUnity(_Unity)
	, m_Release(false)
{
	m_PxMaterial = PhysXMgr::GetInst()->GPhysics()->createMaterial(0.0f, 0.0f, 0.0f);

	//m_PxMaterial->setFrictionCombineMode((PxCombineMode::eAVERAGE));
	//m_PxMaterial->setRestitutionCombineMode((PxCombineMode::eAVERAGE));
}

CCollider3D::~CCollider3D()
{

}

void CCollider3D::Release()
{
	if (m_PxColliderRigid)
	{
		m_PxColliderRigid->userData = nullptr;
		PX_RELEASE(m_PxColliderRigid);
	}
	if (m_PxColliderShape)
	{
		PX_RELEASE(m_PxColliderShape);
	}
	m_Release = true;
}

void CCollider3D::finaltick()
{
	if (m_Release)
		return;

	setShapeToRigidBody();
	//if (CLevelMgr::GetInst()->GetCurLevel()->GetState() != LEVEL_STATE::PLAY)

	colliderDebugDraw();
}

CGameObject* CCollider3D::GetColOwner()
{
	if (m_bUnity)
		return m_pUnityOwner;
	else
		return GetOwner();
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
	m_bRaycast = true;
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
		if(!m_bUnity)
		{
			vPos = GetOwner()->Transform()->GetRelativePos();
			vRot = GetOwner()->Transform()->GetRelativeRot();
		}
		
		//Vec3 vScale = GetOwner()->Transform()->GetRelativeScale();

		Quat quat; quat = Vector3ToQuaternion(vRot);

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
			Vec3 vPos = GetOwner()->Transform()->GetRelativePos();
			vPos.y -= GetOwner()->Animator3D()->GetHeadPos().y;
			m_vScale = GetOwner()->Transform()->GetRelativeScale();
			m_vScale.y += vPos.y;
		}
			
		else if (_rb && m_bAttachToRigidBody)
			m_vScale = GetOwner()->RigidBody()->GetRigidScale();

		else
		{
			m_vScale = GetOwner()->Transform()->GetRelativeScale();
		}
	}

	if(m_tColliderShapeType == COLLIDER_SHAPE_TYPE::BOX)
	{
		m_PxColliderShape = PhysXMgr::GetInst()->GPhysics()->createShape(
			physx::PxBoxGeometry((m_vScale.x / 2.f), (m_vScale.y / 2.f), (m_vScale.z / 2.f))
			, *m_PxMaterial
			, true);
	}
	if(m_tColliderShapeType == COLLIDER_SHAPE_TYPE::MESH)
	{
		createTriangleMesh();
	}
	//m_PxColliderShape = createTriggerShape(PxBoxGeometry(m_vScale.x + 2, m_vScale.y + 2, m_vScale.z + 2), *m_PxMaterial, true);
	PxFilterData triggerFilterData;

	if(GetOwner()->GetLayerIndex() == (int)LAYER_TYPE::NoRaycastingCollider)
	{
		triggerFilterData.word0 = (PxU32)RAYCAST_GROUP_TYPE::NoRaycastingCollider;
		triggerFilterData.word1 = 0x0000000f;
		triggerFilterData.word2 = 0x0000000f;
		triggerFilterData.word3 = 0x0000000f;
	}
	else if (GetOwner()->GetLayerIndex() == (int)LAYER_TYPE::Player)
	{
		triggerFilterData.word0 = (PxU32)RAYCAST_GROUP_TYPE::Player;
		triggerFilterData.word1 = 0x0000000f;
		triggerFilterData.word2 = 0x0000000f;
		triggerFilterData.word3 = 0x0000000f;
	}
	else if (GetOwner()->GetLayerIndex() == (int)LAYER_TYPE::Enemy)
	{
		triggerFilterData.word0 = (PxU32)RAYCAST_GROUP_TYPE::Enemy;
		triggerFilterData.word1 = 0x0000000f;
		triggerFilterData.word2 = 0x0000000f;
		triggerFilterData.word3 = 0x0000000f;
	}
	else
	{
		triggerFilterData.word0 = (PxU32)RAYCAST_GROUP_TYPE::DefaultRaycastingCollider;
		triggerFilterData.word1 = 0x0000000f;
		triggerFilterData.word2 = 0x0000000f;
		triggerFilterData.word3 = 0x0000000f;
	}

	if(!m_bRaycastable)
	{
		triggerFilterData.word0 = (PxU32)RAYCAST_GROUP_TYPE::NoRaycastingCollider;
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
	
	if (m_bAttachToRigidBody && (nullptr == _rb || !_rb->IsRigidBodyCreate())) return;
	if (!m_bFirstInit) return;

	physx::PxTransform pos;
	if (_rb)
		pos = GetOwner()->RigidBody()->GetRigidBodyPos();
	else
		pos = m_PxColliderRigid->getGlobalPose();
	

	if(m_tColliderShapeType == COLLIDER_SHAPE_TYPE::BOX)
	{
		Matrix worldMat = physx::Util::WorldMatFromGlobalPose(pos
			, Vec3((m_vScale.x / 2.f) + 5.f, (m_vScale.y / 2.f) + 5.f, (m_vScale.z / 2.f) + 5.f)
		);
		DrawDebugCube(worldMat, Vec4(0.f, 0.f, 1.f, 1.f), 0.f, true);
		
	}
	if (m_tColliderShapeType == COLLIDER_SHAPE_TYPE::MESH)
	{
		Matrix worldMat = GetOwner()->Transform()->GetWorldMat();
		DrawDebugMeshFace(worldMat, m_debugMeshName, GetOwner()->MeshRender()->GetMtrlCount(), Vec4(0.f, 0.f, 1.f, 1.f), 0.f, true);
	}
}

void CCollider3D::convertMeshToGeom()
{
	Vtx v; PxU32 idx; PxVec3 pV; vector<UINT> inds;
	Ptr<CMesh> pMesh = GetOwner()->MeshRender()->GetMesh();
	Vec3 vScale = GetOwner()->Transform()->GetRelativeScale();
	m_debugMeshName = pMesh->GetKey();

	if (pMesh->GetSubsetCount() > 1)
	{
		int a = 0;
	}
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

void CCollider3D::createTriangleMesh()
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
	m_PxColliderShape = PhysXMgr::GetInst()->GPhysics()->createShape(PxTriangleMeshGeometry(triMesh), *m_PxMaterial, true);

	PX_RELEASE(triMesh);
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
	fread(&m_bUnity, sizeof(bool), 1, _FILE);
	fread(&m_bCenter, sizeof(bool), 1, _FILE);
	fread(&m_tColliderShapeType, sizeof(UINT), 1, _FILE);
	if (m_tColliderShapeType == COLLIDER_SHAPE_TYPE::MESH)
		LoadWString(m_debugMeshName, _FILE);
}

void CCollider3D::SaveToLevelFile(FILE* _File)
{
	fwrite(&m_vScale, sizeof(Vec3), 1, _File);
	fwrite(&m_vOffset, sizeof(Vec3), 1, _File);
	fwrite(&m_bAttachToRigidBody, sizeof(bool), 1, _File);
	fwrite(&m_bUnity, sizeof(bool), 1, _File);
	fwrite(&m_bCenter, sizeof(bool), 1, _File);
	fwrite(&m_tColliderShapeType, sizeof(UINT), 1, _File);
	if (m_tColliderShapeType == COLLIDER_SHAPE_TYPE::MESH)
		SaveWString(m_debugMeshName, _File);
}
