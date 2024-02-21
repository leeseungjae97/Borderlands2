#include "pch.h"
#include "RaycastMgr.h"

#include "CCamera.h"
#include "PhysXMgr.h"
#include "CCollider3D.h"
#include "CKeyMgr.h"
#include "CRenderMgr.h"
#include "CTransform.h"
#include "physx_util.h"

RaycastMgr::RaycastMgr()
	: m_fDrawRayDistance(1000.f)
	, tPlayerRayInfo{}
{
}

RaycastMgr::~RaycastMgr()
{
}

void RaycastMgr::tick()
{
	if (KEY_PRESSED(KEY::T))
	{
		m_vecRayDraw.clear();
		vector<tDrawRaycast>().swap(m_vecRayDraw);
	}

	for (int i = 0; i < m_vecRayDraw.size(); ++i)
	{
		tDrawRaycast _Draw = m_vecRayDraw[i];
		DrawDebugLine(DIR_TYPE::FRONT, _Draw.matWorld, _Draw.vColor);
	}
}

//void RaycastMgr::UseRayInfo()
//{
//	m_bDoRaycast = false;
//}

void RaycastMgr::AddRaycastDraw(Vec3 _vDir, Vec3 _vOrigin, Matrix matWorld, Vec4 _vColor)
{
	tDrawRaycast drawRay;
	drawRay.vDir = _vDir;
	drawRay.vOrign = _vOrigin;
	drawRay.vColor = _vColor;
	drawRay.matWorld = matWorld/* * XMMatrixScaling(1000.f, 1000.f, 1000.f)*/;
	//matWorld *= ;
	m_vecRayDraw.push_back(drawRay);
}


bool RaycastMgr::DoOneHitRaycast(tRayInfo _RaycastInfo, Vec3* HitPosition)
{
	Vec3 _vDir = _RaycastInfo.vDir;
	Vec3 _vOrigin = _RaycastInfo.vStart;

	PxScene* mScene = PhysXMgr::GetInst()->GCurScene();
	PxVec3 _vOr = PxVec3(_vOrigin.x, _vOrigin.y, _vOrigin.z);
	PxVec3 _vDr = PxVec3(_vDir.x, _vDir.y, _vDir.z);
	_vDr.normalize();
	PxOverlapBuffer OverLaphit;
	PxRaycastBuffer hit;
	PxQueryFilterData filterData;
	filterData.data.word0 = (1 << 0);

	filterData.flags |= PxQueryFlag::eDYNAMIC;
	//filterData.flags |= PxQueryFlag::eSTATIC;
	int iBulletLayoutIndex = (int)LAYER_TYPE::EnemyBullet;
	if(_RaycastInfo.iLayerIdx == (int)LAYER_TYPE::Player)
	{
		iBulletLayoutIndex = (int)LAYER_TYPE::PlayerBullet;
	}
	
	if (mScene->raycast(_vOr, _vDr, 100000.f, hit, PxHitFlag::eDEFAULT, filterData))
	{
		if (HitPosition)
			*HitPosition = Vec3(hit.block.position.x, hit.block.position.y, hit.block.distance);
		if (hit.block.actor->userData)
		{
			CCollider3D* ohterCol = static_cast<CCollider3D*>(hit.block.actor->userData);

			if (ohterCol)
			{
				int iOtherLayoutIdx = ohterCol->GetOwner()->GetLayerIndex();
				if (_RaycastInfo.iID == ohterCol->GetOwner()->GetID())
				{
					return false;
				}

				if (CollisionMgr::GetInst()->IsLayerIntersect(iOtherLayoutIdx, iBulletLayoutIndex))
				{
					ohterCol->Raycast(_RaycastInfo);
					return true;
				}
				else
				{
					return false;
				}
			}
		}
	}

	return false;
}

bool RaycastMgr::DoAllHitRaycast(tRayInfo _RaycastInfo, Vec3* HitPosition)
{
	Vec3 _vDir = _RaycastInfo.vDir;
	Vec3 _vOrigin = _RaycastInfo.vStart;

	PxScene* mScene = PhysXMgr::GetInst()->GCurScene();
	PxVec3 _vOr = PxVec3(_vOrigin.x, _vOrigin.y, _vOrigin.z);
	PxVec3 _vDr = PxVec3(_vDir.x, _vDir.y, _vDir.z);
	_vDr.normalize();
	PxOverlapBuffer OverLaphit;
	PxRaycastBuffer hit;
	PxQueryFilterData filterData;
	filterData.data.word0 = (1 << 0);

	filterData.flags |= PxQueryFlag::eDYNAMIC;
	//filterData.flags |= PxQueryFlag::eSTATIC;
	int iBulletLayoutIndex = (int)LAYER_TYPE::EnemyBullet;
	if (_RaycastInfo.iLayerIdx == (int)LAYER_TYPE::Player)
	{
		iBulletLayoutIndex = (int)LAYER_TYPE::PlayerBullet;
	}

	const PxU32 bufferSize = 256;        // [in] size of 'hitBuffer'
	PxRaycastHit hitBuffer[bufferSize];  // [out] User provided buffer for results
	PxRaycastBuffer buf(hitBuffer, bufferSize); // [out] Blocking and touching hits stored here

	// Raycast against all static & dynamic objects (no filtering)
	// The main result from this call are all hits along the ray, stored in 'hitBuffer'
	mScene->raycast(_vOr, _vDr, 100000.f, buf);
	for (PxU32 i = 0; i < buf.nbTouches; i++)
	{
		if (_vOr != buf.touches[i].position)
		{
			if(HitPosition)
				*HitPosition = Vec3(buf.touches[i].position.x, buf.touches[i].position.y, buf.touches[i].distance);
		}
	}
	return false;
}

void RaycastMgr::AddRaycast(tRayInfo _RaycastInfo)
{
	m_vecEnemyRayInfo.push_back(_RaycastInfo);
}
void RaycastMgr::ReplyRaycast(tRayInfo _RaycastInfo)
{
	m_vecEnemyRayInfo.push_back(_RaycastInfo);
}

void RaycastMgr::ClearRayInfo()
{
	m_vecEnemyRayInfo.clear();
	vector<tRayInfo>().swap(m_vecEnemyRayInfo);
}

PxVec3 RaycastMgr::GetShapeCenter(PxShape* shape, const PxTransform& pose)
{
	PxVec3 offset(0.0f);
	const PxGeometry& geom = shape->getGeometry();
	if (geom.getType() == PxGeometryType::eCONVEXMESH)
	{
		const PxConvexMeshGeometry& geometry = static_cast<const PxConvexMeshGeometry&>(geom);

		PxReal mass;
		PxMat33 localInertia;
		PxVec3 localCenterOfMass;
		geometry.convexMesh->getMassInformation(mass, localInertia, localCenterOfMass);

		offset += localCenterOfMass;
	}
	return pose.transform(offset);
}
