#include "CEnemyScript.h"

#include <Engine/CameraMgr.h>
#include <Engine/RaycastMgr.h>
#include <Engine\LandScapeMgr.h>

#include <Engine\func.h>
#include <Engine\PlayerMgr.h>
#include <Engine\CRenderMgr.h>
#include <Engine\AnimationMgr.h>
#include <Engine\WeaponMgr.h>
#include <Engine\physx_util.h>


CEnemyScript::CEnemyScript()
	: CScript((UINT)SCRIPT_TYPE::ENEMYSCRIPT)
	, fMonsterSpeed(100.f)
	, fRotateSpeed(2.f)
	, iMonsterHp(100.f)
	, fRateOfFire(0.1f)
	, fRateOfFireAcc(0.0f)
	, iAmmo(300)
	, bRotate(true)
	, tState(EnemyMgr::ENEMY_STATE::IDLE)
{
	AddScriptParam(SCRIPT_PARAM::FLOAT, &fRotateSpeed, "Rotate Speed");
	AddScriptParam(SCRIPT_PARAM::FLOAT, &fMonsterSpeed, "Speed");
	AddScriptParam(SCRIPT_PARAM::FLOAT, &iMonsterHp, "Monster Hp");
}

CEnemyScript::~CEnemyScript()
{
}
void CEnemyScript::begin()
{
	CGameObject* pMonster = GetOwner();

	if (!pMonster->Animator3D())
		return;

	pMonster->Animator3D()->EndEvent((UINT)ENEMY_ANIMATION_TYPE::RELOAD)
		= std::make_shared<std::function<void()>>([=]()
			{
				tState = EnemyMgr::ENEMY_STATE::IDLE;
	iAmmo = 30;
			});

	pMonster->Animator3D()->EndEvent((UINT)ENEMY_ANIMATION_TYPE::DIE)
		= std::make_shared<std::function<void()>>([=]()
			{
				pMonster->SetDead(true);
			});
}

void CEnemyScript::Move()
{
	if (tState == EnemyMgr::ENEMY_STATE::RELOAD)
		return;

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
	pMonster->Animator3D()->Play((UINT)ENEMY_ANIMATION_TYPE::WALK_FORWARD, true);
	pMonster->RigidBody()->SetLinearVelocity(final_velocity * fSpeed);

	tState = EnemyMgr::ENEMY_STATE::WALK;
}

bool CEnemyScript::Rotate()
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
	if(areAlmostEqual(includeAngle, 0.0f, 5.f))
	{
		return true;
	}
	return false;
}

void CEnemyScript::Reload()
{
	if (tState == EnemyMgr::ENEMY_STATE::RELOAD)
		return;
	
	CGameObject* pMonster = GetOwner();

	CGameObject* pGun = pMonster->GetGun(0);
	if (nullptr == pGun)
		return;

	pMonster->Animator3D()->Play((UINT)ENEMY_ANIMATION_TYPE::RELOAD, false);
	//pGun->Animator3D()->Play((UINT)GUN_ANIMATION_TYPE::RELOAD, false);
	//AnimationMgr::GetInst()->AnimationSync(pGun->Animator3D(), pMonster->Animator3D());

	tState = EnemyMgr::ENEMY_STATE::RELOAD;
}

void CEnemyScript::tick()
{
	CGameObject* pMonster = GetOwner();
	//CatchRaycast();

	if (!pMonster->Animator3D())
		return;
	if(iMonsterHp == 0)
	{
		pMonster->Animator3D()->Play((UINT)ENEMY_ANIMATION_TYPE::DIE, true);
		tState = EnemyMgr::ENEMY_STATE::DIE;
	}

	//Look();

	if(iAmmo <= 0)
	{
		Reload();
	}

	fRateOfFireAcc += DT;
	if (bRotate)
	{
		if(Rotate())
		{
			if (fRateOfFireAcc >= fRateOfFire)
			{
				Shoot();
			}
		}
	}
	
	if (KEY_PRESSED(KEY::_0))
	{
		Move();
	}
	if (KEY_PRESSED(KEY::_9))
	{
		
		//pMonster->RigidBody()->SetAngularVelocity(Vec3(0.f, tt, 0.f));
	}
	if(tState == EnemyMgr::ENEMY_STATE::IDLE)
	{
		pMonster->Animator3D()->Play((UINT)ENEMY_ANIMATION_TYPE::IDLE, true);
	}
}

void CEnemyScript::BeginOverlap(CCollider3D* _Other)
{

}

void CEnemyScript::Raycast(tRayInfo _RaycastInfo)
{
	if (iMonsterHp < _RaycastInfo.fDamage)
		iMonsterHp = 0;
	else
		iMonsterHp -= _RaycastInfo.fDamage;
}

void CEnemyScript::Attacked(float fDamage)
{
	if (iMonsterHp < fDamage)
		iMonsterHp = 0;
	else
		iMonsterHp -= fDamage;
}

void CEnemyScript::CatchRaycast()
{
	//CCamera* MainCam = CRenderMgr::GetInst()->GetMainCam();
	//if (nullptr == MainCam)
	//	MainCam = CameraMgr::GetInst()->GetCamObj(L"MainCamera")->Camera();

	tRayInfo raycast = RaycastMgr::GetInst()->GetPlayerRayInfo();
	// ================================================================================
	//Vec3 vFront = MainCam->Transform()->GetRelativeDir(DIR_TYPE::FRONT);
	//Vec3 vFront = MainCam->GetRay().vDir;
	Vec3 vFront = raycast.vDir;
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
		if (raycast.tRayType == (UINT)RAYCAST_TYPE::SHOOT)
		{
			RaycastMgr::GetInst()->AddRaycastDraw(vFront, vPos, raycast.matWorld, Vec4(1.f, 1.f, 0.f, 1.f));
			Attacked(raycast.fDamage);
		}
	}
}

void CEnemyScript::Shoot()
{
	if (tState == EnemyMgr::ENEMY_STATE::RELOAD)
		return;

	CGameObject* pMonster = GetOwner();

	CGameObject* pGun = pMonster->GetGun(0);
	if (nullptr == pGun)
		return;

	// ================================================================================
	Vec3 vFront = pMonster->Transform()->GetRelativeDir(DIR_TYPE::FRONT);
	Vec3 vPos = pMonster->Transform()->GetRelativePos();
	Vec3 vScale = pMonster->Transform()->GetRelativeScale();

	vPos += vFront * vScale;

	tRayInfo raycast{};
	raycast.fDamage = 10.f;
	raycast.iLayerIdx = pMonster->GetLayerIndex();
	raycast.vStart = vPos;
	raycast.vDir = vFront;
	raycast.vDir.y = 0.f;
	raycast.iID = pMonster->GetID();
	raycast.tRayType = (UINT)RAYCAST_TYPE::SHOOT;
	raycast.matWorld = pMonster->Transform()->GetDrawRayMat();
	if(RaycastMgr::GetInst()->DoRaycast(raycast))
	{
		fRateOfFireAcc = 0.0f;
		--iAmmo;

		Vec3 vMuzzlePos = WeaponMgr::GetInst()->GetWeaponMuzzlePos(pGun);
		Vec3 vWeaponRot = pGun->Transform()->GetRelativeRot();

		vWeaponRot.y -= 180.f;

		WeaponMgr::GetInst()->MuzzleFlash(vMuzzlePos, vWeaponRot);
	}
}

void CEnemyScript::Look()
{
	CGameObject* pMonster = GetOwner();

	Vec3 vFront = pMonster->Transform()->GetRelativeDir(DIR_TYPE::FRONT);
	Vec3 vPos = pMonster->Transform()->GetRelativePos();
	
	tRayInfo raycast{};
	raycast.fDamage = 0.f;
	raycast.iLayerIdx = pMonster->GetLayerIndex();
	raycast.vStart = vPos;
	raycast.vDir = vFront;
	raycast.vDir.y = 0.f;
	raycast.iID = pMonster->GetID();
	raycast.tRayType = (UINT)RAYCAST_TYPE::LOOK;
	raycast.matWorld = pMonster->Transform()->GetDrawRayMat();
	//RaycastMgr::GetInst()->AddRaycast(raycast);
	RaycastMgr::GetInst()->DoRaycast(raycast);
}

bool CEnemyScript::IsDetect()
{
	CGameObject* pMonster = GetOwner();
	vector<tRayInfo> monsRayInfo = RaycastMgr::GetInst()->GetRayInfo();
	tRayInfo rayInfo  = RaycastMgr::GetInst()->GetPlayerRayInfo();
	
	if (rayInfo.tRayType == (UINT)RAYCAST_TYPE::REPLY && rayInfo.iID == pMonster->GetID())
	{
		return true;
	}

	return false;
}


//void CEnemyScript::BeginOverlap(CCollider2D* _Other)
//{
//	if (L"Parent Object" == _Other->GetOwner()->GetName())
//	{
//		//Destroy();	
//
//		SetLifeSpan(2.f);
//	}
//}

