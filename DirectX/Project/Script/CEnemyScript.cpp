#include "pch.h"
#include "CEnemyScript.h"

#include <Engine/CameraMgr.h>
#include <Engine/NavigationMgr.h>
#include <Engine/RaycastMgr.h>
#include <Engine\LandScapeMgr.h>

#include <Engine\func.h>
#include <Engine\PlayerMgr.h>
#include <Engine\CRenderMgr.h>
#include <Engine\AnimationMgr.h>
#include <Engine\WeaponMgr.h>
#include <Engine\physx_util.h>

#include "CPlayerScript.h"


CEnemyScript::CEnemyScript()
	: CScript((UINT)SCRIPT_TYPE::ENEMY_SCRIPT)
	, fEnemySpeed(300.f)
	, fRotateSpeed(2.f)
	, iEnemyHp(100.f)
	, iEnemyHpCapa(100.f)
	, fRateOfFire(0.1f)
	, fRateOfFireAcc(0.0f)
	, iAmmo(300)
	, bRotate(true)
	, pHeadCollider(nullptr)
	, tState(EnemyMgr::ENEMY_STATE::IDLE)
{
	AddScriptParam(SCRIPT_PARAM::FLOAT, &fRotateSpeed, "Rotate Speed");
	AddScriptParam(SCRIPT_PARAM::FLOAT, &fEnemySpeed, "Speed");
	AddScriptParam(SCRIPT_PARAM::FLOAT, &iEnemyHp, "Monster Hp");
}

CEnemyScript::~CEnemyScript()
{
}
void CEnemyScript::begin()
{
	CGameObject* pEnemy = GetOwner();

	if (!pEnemy->Animator3D())
		return;

	pEnemy->Animator3D()->EndEvent((UINT)ENEMY_ANIMATION_TYPE::RELOAD)
		= std::make_shared<std::function<void()>>([=]()
			{
				tState = EnemyMgr::ENEMY_STATE::IDLE;
	iAmmo = 30;
			});

	pEnemy->Animator3D()->StartEvent((UINT)ENEMY_ANIMATION_TYPE::DIE)
		= std::make_shared<std::function<void()>>([=]()
			{
				
			});

	makeCollider();
}

void CEnemyScript::Move()
{
	if (tState == EnemyMgr::ENEMY_STATE::RELOAD)
		return;

	CGameObject* pEnemy = GetOwner();
	Vec3 vPos = pEnemy->Transform()->GetRelativePos();
	Vec3 vRot = pEnemy->Transform()->GetRelativeRot();
	Vec3 vScale = pEnemy->Transform()->GetRelativeScale();
	if(m_vecQueryPath.empty())
	{
		vDestPos = vPos;
		return;
	}
	Vec3 vPathPos = m_vecQueryPath.front();
	vPathPos.y += vScale.y / 2.f;
	Vec3 vDir = Vec3::Zero;
	if(vPathPos != vDestPos)
	{
		vPrevPos = vPos;
		vDestPos = vPathPos;
		fDestDist = fabs((vDestPos - vPos).Length());
	}
	Vec3 vEnemyRight= pEnemy->Transform()->GetRelativeDir(DIR_TYPE::RIGHT);

	float fRemainDist = fabs((vPrevPos - vPos).Length());

	vDir = vDestPos - vPos;	
	vDir.Normalize();

	if (fRemainDist < fDestDist)
	{
		vPos += vDir * DT * fEnemySpeed;	
	}
	else
	{
		auto iter = m_vecQueryPath.begin();
		iter = m_vecQueryPath.erase(iter);
	}
	float includeAngle = vDir.Dot(vEnemyRight);
	vRot.y += includeAngle * DT * fRotateSpeed;
	pEnemy->Transform()->SetRelativePos(vPos);
	pEnemy->Animator3D()->Play((UINT)ENEMY_ANIMATION_TYPE::WALK_FORWARD, true);
	pEnemy->Transform()->SetRelativeRot(vRot);
	//pEnemy->RigidBody()->SetLinearVelocity(final_velocity * fEnemySpeed);

	//Vec3 vRot = pEnemy->Transform()->GetRelativeRot();
	//vRot.y = 0.f;
	//pEnemy->Transform()->SetRelativeRot(vRot);

	tState = EnemyMgr::ENEMY_STATE::WALK;
}

bool CEnemyScript::Rotate()
{
	CGameObject* pPlayer = PlayerMgr::GetInst()->GetPlayer();
	if (nullptr == pPlayer) return false;

	CGameObject* pEnemy = GetOwner();

	Vec3 vPlayerPos = pPlayer->Transform()->GetRelativePos();
	Vec3 vMonsterPos = pEnemy->Transform()->GetRelativePos();


	Vec3 vMonsterRight = pEnemy->Transform()->GetRelativeDir(DIR_TYPE::RIGHT);

	Vec3 vDir = vPlayerPos - vMonsterPos;
	vDir.Normalize();
	vMonsterRight.Normalize();

	// 벡터의 내적은 90도 일때 0 Dot(A,B)=∣A∣⋅∣B∣⋅cos(θ)
	float includeAngle = vDir.Dot(vMonsterRight);
	pEnemy->Transform()->SetRelativeRot(Vec3(0.f, includeAngle * DT * fRotateSpeed, 0.f));
	//pEnemy->RigidBody()->SetAngularVelocity(Vec3(0.f, includeAngle * fRotateSpeed, 0.f));

	if (areAlmostEqual(includeAngle, 0.0f, 5.f))
	{
		return true;
	}
	return false;
}

bool CEnemyScript::RotateDest()
{
	CGameObject* pEnemy = GetOwner();
	Vec3 vMonsterPos = pEnemy->Transform()->GetRelativePos();
	Vec3 vMonsterRight = pEnemy->Transform()->GetRelativeDir(DIR_TYPE::RIGHT);

	Vec3 vDir = vDestPos - vMonsterPos;
	vDir.Normalize();
	vMonsterRight.Normalize();

	// 벡터의 내적은 90도 일때 0 Dot(A,B)=∣A∣⋅∣B∣⋅cos(θ)
	float includeAngle = vDir.Dot(vMonsterRight);

	//pEnemy->RigidBody()->SetAngularVelocity(Vec3(0.f, includeAngle * fRotateSpeed, 0.f));
	if (areAlmostEqual(includeAngle, 0.0f, 1.f))
	{
		return true;
	}
	return false;
}

void CEnemyScript::Reload()
{
	if (tState == EnemyMgr::ENEMY_STATE::RELOAD)
		return;

	CGameObject* pEnemy = GetOwner();

	CGameObject* pGun = pEnemy->GetGun(0);
	if (nullptr == pGun)
		return;

	pEnemy->Animator3D()->Play((UINT)ENEMY_ANIMATION_TYPE::RELOAD, false);
	//pGun->Animator3D()->Play((UINT)GUN_ANIMATION_TYPE::RELOAD, false);
	//AnimationMgr::GetInst()->AnimationSync(pGun->Animator3D(), pMonster->Animator3D());

	tState = EnemyMgr::ENEMY_STATE::RELOAD;
}

void CEnemyScript::makeCollider()
{
	{
		CGameObject* pObj = new CGameObject;
		pObj->AddComponent(new CCollider3D(false, true));
		pObj->Collider3D()->SetUnityOwner(GetOwner());
		pObj->Collider3D()->SetScale(Vec3(50.f, 50.f, 50.f));

		pObj->SetName(GetOwner()->GetName() + L" head collider");

		SpawnGameObject(pObj, Vec3(0.f, 100.f, 0.f), LAYER_TYPE::Enemy, true);
		pHeadCollider = pObj;
	}
}

void CEnemyScript::moveCollider()
{
	CGameObject* pEnemy = GetOwner();
	CAnimator3D* pAnimator = pEnemy->Animator3D();
	{
		int headIdx = pAnimator->GetHeadIdx();
		Vec3 vPos = AnimationMgr::GetInst()->BonePos(headIdx, pEnemy);
		Vec3 vRot = AnimationMgr::GetInst()->BoneRot(headIdx, pEnemy);

		pHeadCollider->Collider3D()->SetColliderPos(vPos);
		pHeadCollider->Collider3D()->SetColliderRot(vRot);
	}
}

void CEnemyScript::doPathQuery()
{
	m_vecQueryPath.clear();
	Vec3 vPos = GetOwner()->Transform()->GetRelativePos();
	vPos.y -= GetOwner()->Transform()->GetRelativeScale().y / 2.f;
	Vec3 vMousePos = NavigationMgr::GetInst()->GetMapRaycastPos();
	int size = 0;
	NavigationMgr::GetInst()->DoQuery(vPos, vMousePos, &m_vecQueryPath, &size);
}

void CEnemyScript::tick()
{
	//static bool init = false;
	//if(!init)
	//{
	//	makeCollider();
	//	init = true;
	//}
	CGameObject* pEnemy = GetOwner();
	//CatchRaycast();

	if (!pEnemy->Animator3D())
		return;
	if (iEnemyHp == 0)
	{
		pEnemy->Animator3D()->Play((UINT)ENEMY_ANIMATION_TYPE::DIE, false, false);
		PaperBurn();
		tState = EnemyMgr::ENEMY_STATE::DIE;
	}

	//Look();

	if (iAmmo <= 0)
	{
		Reload();
	}

	moveCollider();

	fRateOfFireAcc += DT;
	if (bRotate)
	{
		if (Rotate())
		{
			if (fRateOfFireAcc >= fRateOfFire)
			{
				Shoot();
			}
		}
	}


	if (KEY_PRESSED(KEY::_9))
	{

		//pMonster->RigidBody()->SetAngularVelocity(Vec3(0.f, tt, 0.f));
	}
	if (tState == EnemyMgr::ENEMY_STATE::IDLE)
	{
		pEnemy->Animator3D()->Play((UINT)ENEMY_ANIMATION_TYPE::IDLE, true);
	}

	IsDie();
}

void CEnemyScript::finaltick()
{
	if (KEY_PRESSED(KEY::Q))
	{
		doPathQuery();
	}
	Move();
	//if(RotateDest())
	//{
	//	
	//}
	
}

void CEnemyScript::BeginOverlap(CCollider3D* _Other)
{

}

void CEnemyScript::Raycast(tRayInfo _RaycastInfo)
{
	if (pHeadCollider->Collider3D()->IsRaycast())
	{
		Attacked(_RaycastInfo.fDamage * 2.f);
	}
	else
	{
		if (iEnemyHp < _RaycastInfo.fDamage)
			iEnemyHp = 0;
		else
			iEnemyHp -= _RaycastInfo.fDamage;
	}
}

void CEnemyScript::Attacked(float fDamage)
{
	if (iEnemyHp < fDamage)
		iEnemyHp = 0;
	else
		iEnemyHp -= fDamage;
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

	CGameObject* pEnemy = GetOwner();

	CGameObject* pGun = pEnemy->GetGun(0);
	if (nullptr == pGun)
		return;

	// ================================================================================
	Vec3 vFront = pEnemy->Transform()->GetRelativeDir(DIR_TYPE::FRONT);
	Vec3 vPos = pEnemy->Transform()->GetRelativePos();
	Vec3 vScale = pEnemy->Transform()->GetRelativeScale();

	vPos += vFront * vScale;

	tRayInfo raycast{};
	raycast.fDamage = 10.f;
	raycast.iLayerIdx = pEnemy->GetLayerIndex();
	raycast.vStart = vPos;
	raycast.vDir = vFront;
	raycast.vDir.y = 0.f;
	raycast.iID = pEnemy->GetID();
	raycast.tRayType = (UINT)RAYCAST_TYPE::SHOOT;
	raycast.matWorld = pEnemy->Transform()->GetDrawRayMat();
	if (RaycastMgr::GetInst()->DoOneHitRaycast(raycast, RAYCAST_GROUP_TYPE::Enemy))
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
	CGameObject* pEnemy = GetOwner();

	Vec3 vFront = pEnemy->Transform()->GetRelativeDir(DIR_TYPE::FRONT);
	Vec3 vPos = pEnemy->Transform()->GetRelativePos();

	tRayInfo raycast{};
	raycast.fDamage = 0.f;
	raycast.iLayerIdx = pEnemy->GetLayerIndex();
	raycast.vStart = vPos;
	raycast.vDir = vFront;
	raycast.vDir.y = 0.f;
	raycast.iID = pEnemy->GetID();
	raycast.tRayType = (UINT)RAYCAST_TYPE::LOOK;
	raycast.matWorld = pEnemy->Transform()->GetDrawRayMat();
	//RaycastMgr::GetInst()->AddRaycast(raycast);
	RaycastMgr::GetInst()->DoOneHitRaycast(raycast, RAYCAST_GROUP_TYPE::Enemy);
}

bool CEnemyScript::IsDetect()
{
	CGameObject* pEnemy = GetOwner();
	vector<tRayInfo> monsRayInfo = RaycastMgr::GetInst()->GetRayInfo();
	tRayInfo rayInfo = RaycastMgr::GetInst()->GetPlayerRayInfo();

	if (rayInfo.tRayType == (UINT)RAYCAST_TYPE::REPLY && rayInfo.iID == pEnemy->GetID())
	{
		return true;
	}

	return false;
}

void CEnemyScript::PaperBurn()
{
	if (tState == EnemyMgr::ENEMY_STATE::DIE)
		return;

	PlayerMgr::GetInst()->GetPlayer()->GetScript<CPlayerScript>()->AddExp(1);

	CGameObject* pEnemy = GetOwner();
	double dTime = AnimationMgr::GetInst()->GetCurAnimationTime(pEnemy->Animator3D());
	pEnemy->MeshRender()->SetPaparBurn(true, (float)dTime);
}

void CEnemyScript::IsDie()
{
	CGameObject* pEnemy = GetOwner();
	if(pEnemy->MeshRender()->GetMaterial(0)->IsPaperBurnEnd())
	{
		DestroyObject(pEnemy);
		if (pEnemy->GetGuns().size() != 0)
		{
			for (int i = 0; i < pEnemy->GetGuns().size(); ++i)
			{
				DestroyObject(pEnemy->GetGuns()[i]);
			}
		}
		if (pHeadCollider)
			DestroyObject(pHeadCollider);

		pEnemy->SetObjectState(CGameObject::OBJECT_STATE::INVISIBLE);
	}
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

