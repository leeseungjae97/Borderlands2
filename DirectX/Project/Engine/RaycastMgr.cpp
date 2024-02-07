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

void RaycastMgr::DoRaycast(tRayInfo _RaycastInfo)
{
	//tPlayerRayInfo = _RaycastInfo;
	//m_vecMonsterRayInfo.push_back(_RaycastInfo);
	//m_bDoRaycast = true;

	//PxScene* mScene = PhysXMgr::GetInst()->GCurScene();
	//PxVec3 _vOr = PxVec3(_vOrigin.x, _vOrigin.y, _vOrigin.z);
	//PxVec3 _vDr = PxVec3(_vDir.x, _vDir.y, _vDir.z);
	//_vDr.normalize();
	//PxRaycastBuffer hit2;
	//if(mScene->raycast(_vOr, _vDr, 10000.f, hit2))
	//{
	//	AddRaycastDraw(_vDir, _vOrigin, Vec4(1.f, 0.f, 1.f, 1.f));
	//}

	//PxOverlapBuffer OverLaphit;
	//PxTransform pose = physx::Util::PxTransformFromVec3PosRot(_vOr, _vDr);
	//PxRaycastBuffer hit;
	//PxQueryFilterData filterData;
	//filterData.flags = PxQueryFlag::eDYNAMIC;
	//int iBulletLayerIdx = CollisionMgr::GetInst()->GetBulletLayerIdx();
	//if (mScene->raycast(_vOr, _vDr, 100000.f, hit, PxHitFlag::eDEFAULT, filterData))
	//{
	//	if (hit.block.actor->userData)
	//	{
	//		CCollider3D* ohterCol = static_cast<CCollider3D*>(hit.block.actor->userData);

	//		if (ohterCol)
	//		{
	//			int iOtherLayoutIdx = ohterCol->GetOwner()->GetLayerIndex();

	//			if (!CollisionMgr::GetInst()->IsLayerIntersect(iOtherLayoutIdx, iBulletLayerIdx))
	//			{
	//				wstring m = L"Raycast Skipped (" + std::to_wstring(iOtherLayoutIdx) + L")\n";
	//				OutputDebugStringW(m.c_str());
	//				return;
	//			}

	//			if (col != ohterCol)
	//			{
	//				ohterCol->Raycast(_RaycastInfo);
	//			}
	//		}
	//	}
	//}
}

void RaycastMgr::AddRaycast(tRayInfo _RaycastInfo)
{
	m_vecMonsterRayInfo.push_back(_RaycastInfo);
}

void RaycastMgr::ClearRayInfo()
{
	m_vecMonsterRayInfo.clear();
	vector<tRayInfo>().swap(m_vecMonsterRayInfo);
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
