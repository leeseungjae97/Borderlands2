#include "pch.h"
#include "CMonsterScript.h"

#include <Engine/CameraMgr.h>
#include <Engine/RaycastMgr.h>
#include <Engine\LandScapeMgr.h>

#include <Engine\func.h>
#include <Engine\PlayerMgr.h>
#include <Engine\CRenderMgr.h>
#include <Engine\physx_util.h>


CMonsterScript::CMonsterScript()
	: CScript((UINT)SCRIPT_TYPE::MONSTERSCRIPT)
	, fMonsterSpeed(100.f)
	, fRotateSpeed(2.f)
	, iMonsterHp(100.f)
	, fRateOfFire(0.2f)
	, fRateOfFireAcc(0.0f)
	, bRotate(true)
{
	AddScriptParam(SCRIPT_PARAM::FLOAT, &fRotateSpeed, "Rotate Speed");
	AddScriptParam(SCRIPT_PARAM::FLOAT, &fMonsterSpeed, "Speed");
	AddScriptParam(SCRIPT_PARAM::FLOAT, &iMonsterHp, "Monster Hp");
}

CMonsterScript::~CMonsterScript()
{
}

void CMonsterScript::Move()
{
	CGameObject* pPlayer = PlayerMgr::GetInst()->GetPlayer();
	CGameObject* pMonster = GetOwner();

	Vec3 vPlayerPos = pPlayer->Transform()->GetRelativePos();
	Vec3 vMonsterPos = pMonster->Transform()->GetRelativePos();

	Vec3 vMonsterFront = pMonster->Transform()->GetRelativeDir(DIR_TYPE::FRONT);
	Vec3 vMonsterUp = pMonster->Transform()->GetRelativeDir(DIR_TYPE::UP);
	Vec3 vMonsterRight = pMonster->Transform()->GetRelativeDir(DIR_TYPE::RIGHT);

	//Vec3 vDir = vPlayerPos - vMonsterPos;
	//vDir.Normalize();

	Vec3 final_velocity = Vec3(0.f, 0.f, 0.f);
	float fSpeed = fMonsterSpeed;

	final_velocity += vMonsterFront * DT * fSpeed;
	//pMonster->Animator3D()->Play(ANIMATION_TYPE::WALK_FORWARD, true);
	pMonster->RigidBody()->SetLinearVelocity(final_velocity * fSpeed);
}

bool CMonsterScript::Rotate()
{
	CGameObject* pPlayer = PlayerMgr::GetInst()->GetPlayer();
	if (nullptr == pPlayer) return false;

	CGameObject* pMonster = GetOwner();

	Vec3 vPlayerPos = pPlayer->Transform()->GetRelativePos();
	Vec3 vMonsterPos = pMonster->Transform()->GetRelativePos();


	Vec3 vMonsterRight = pMonster->Transform()->GetRelativeDir(DIR_TYPE::RIGHT);

	Vec3 vDir = vPlayerPos - vMonsterPos;
	vDir.Normalize();
	vMonsterRight.Normalize();

	// 벡터의 내적은 90도 일때 0 Dot(A,B)=∣A∣⋅∣B∣⋅cos(θ)
	float includeAngle = vDir.Dot(vMonsterRight);

	pMonster->RigidBody()->SetAngularVelocity(Vec3(0.f, includeAngle * fRotateSpeed, 0.f));

	return true;
}

void CMonsterScript::tick()
{
	CatchRaycast();

	if(bRotate)
		Rotate();

	if (KEY_PRESSED(KEY::_0))
	{
		Move();
	}
	if (KEY_PRESSED(KEY::_9))
	{
		Shoot();
		//pMonster->RigidBody()->SetAngularVelocity(Vec3(0.f, tt, 0.f));
	}
	if (fRateOfFireAcc >= fRateOfFire)
	{
		
	}
}

void CMonsterScript::BeginOverlap(CCollider3D* _Other)
{

}

void CMonsterScript::Raycast(tRayInfo _RaycastInfo)
{

}

void CMonsterScript::Attacked(float fDamage)
{
	if (iMonsterHp < fDamage)
		iMonsterHp = 0;
	else
		iMonsterHp -= fDamage;
}

void CMonsterScript::CatchRaycast()
{
	//CCamera* MainCam = CRenderMgr::GetInst()->GetMainCam();
	//if (nullptr == MainCam)
	//	MainCam = CameraMgr::GetInst()->GetCamObj(L"MainCamera")->Camera();

	tRayInfo raycast = RaycastMgr::GetInst()->GetPlayerRayInfo();
	// ================================================================================
	//Vec3 vFront = MainCam->Transform()->GetRelativeDir(DIR_TYPE::FRONT);
	//Vec3 vFront = MainCam->GetRay().vDir;
	Vec3 vFront = raycast.vDir;
	// 사용금지
	//Vec3 vPos = MainCam->GetRay().vStart;
	//Vec3 vPos = MainCam->Transform()->GetRelativePos();
	Vec3 vPos = raycast.vStart;

	//int iLayerIdx = GetOwner()->GetLayerIndex();

	//if(!CollisionMgr::GetInst()->IsLayerIntersect(raycast.iLayerIdx, iLayerIdx))
	//{
	//	return;
	//}


	PxVec3 vDir = PxVec3(vFront.x, vFront.y, vFront.z);
	PxVec3 origin = PxVec3(vPos.x, vPos.y, vPos.z);
	// ================================================================================
	CGameObject* pMonster = GetOwner();
	PxShape* shape = pMonster->RigidBody()->GetShape();
	// ================================================================================
	PxGeomRaycastHit hitInfo;
	const PxU32 maxHits = 1;
	const PxHitFlags hitFlags = PxHitFlag::eDEFAULT;
	const PxU32 stride = sizeof(PxGeomRaycastHit);
	const PxGeometryQueryFlags queryFlags = PxGeometryQueryFlag::eDEFAULT;
	PxRaycastThreadContext* threadContext = NULL;
	// ================================================================================
	vDir.normalize();

	PxTransform pose;
	// ================================================================================
	if (pMonster->RigidBody()->GetRigidType() == RIGID_BODY_TYPE::DYNAMIC)
		pose = PxShapeExt::getGlobalPose(*shape, *pMonster->RigidBody()->GetDynamicBody());
	else
		pose = PxShapeExt::getGlobalPose(*shape, *pMonster->RigidBody()->GetStaticBody());
	// ================================================================================
	PxU32 hitCount = PxGeometryQuery::raycast(origin, vDir,
		shape->getGeometry(), pose,
		1e6,
		hitFlags,
		maxHits, &hitInfo,
		stride, queryFlags, threadContext);
	if (hitCount != 0)
	{
		if(raycast.tRayType == (UINT)RAYCAST_TYPE::SHOOT)
		{
			RaycastMgr::GetInst()->AddRaycastDraw(vFront, vPos, raycast.matWorld, Vec4(1.f, 1.f, 0.f, 1.f));
			Attacked(raycast.fDamage);
		}
		else
		{
			Raycast(raycast);
		}
	}

	//if (RaycastMgr::GetInst()->IsRaycast())
	//{
	//	PxU32 hitCount = PxGeometryQuery::raycast(origin, vDir,
	//		shape->getGeometry(), pose,
	//		1e6,
	//		hitFlags,
	//		maxHits, &hitInfo,
	//		stride, queryFlags, threadContext);
	//	if (hitCount != 0)
	//	{
	//		RaycastMgr::GetInst()->UseRayInfo();
	//		RaycastMgr::GetInst()->AddRaycastDraw(vFront, vPos, Vec4(1.f, 1.f, 0.f, 1.f));
	//		Raycast(RaycastMgr::GetInst()->GetRaycastInfo());
	//	}
	//}
}

void CMonsterScript::Shoot()
{
	CGameObject* pMonster = GetOwner();

	// ================================================================================
	Vec3 vFront = pMonster->Transform()->GetRelativeDir(DIR_TYPE::FRONT);
	Vec3 vPos = pMonster->Transform()->GetRelativePos();

	tRayInfo raycast{};
	raycast.fDamage = 10.f;
	raycast.iLayerIdx = pMonster->GetLayerIndex();
	raycast.vStart = vPos;
	raycast.vDir = vFront;
	raycast.vDir.y = 0.f;
	raycast.matWorld = pMonster->Transform()->GetDrawRayMat();
	RaycastMgr::GetInst()->AddRaycast(raycast);
}

//void CMonsterScript::BeginOverlap(CCollider2D* _Other)
//{
//	if (L"Parent Object" == _Other->GetOwner()->GetName())
//	{
//		//Destroy();	
//
//		SetLifeSpan(2.f);
//	}
//}

