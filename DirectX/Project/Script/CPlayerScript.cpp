#include "pch.h"
#include "CPlayerScript.h"

#include <Engine/PlayerMgr.h>
#include <Engine\CMeshRender.h>
#include <Engine\CMaterial.h>
#include <Engine\CRenderMgr.h>
#include <Engine\CameraMgr.h>
#include <Engine\RaycastMgr.h>
#include <Engine\WeaponMgr.h>
#include <Engine\RandMgr.h>
#include <Engine\AnimationMgr.h>
#include <Engine\CFontMgr.h>

#include "CBulletScript.h"
#include "CMissileScript.h"


CPlayerScript::CPlayerScript()
	: CScript((UINT)SCRIPT_TYPE::PLAYERSCRIPT)
	, fSpeed(80.f)
	, fJump(80.f)
	, fRateOfFire(0.05f)
	, fRateOfFireAcc(0.0f)
	, fMouseAcces(1.f)
	, iPlayerHp(1000.f)
	, bReloading(false)
	, iAmmo(30)
	, mt(0.1f)
	, tState(PlayerMgr::PLAYER_STATE::IDLE)
{
	AddScriptParam(SCRIPT_PARAM::FLOAT, &fSpeed, "Player Speed");
	AddScriptParam(SCRIPT_PARAM::FLOAT, &fJump, "Player Jump");
	AddScriptParam(SCRIPT_PARAM::FLOAT, &fMouseAcces, "Mouse Speed");
	AddScriptParam(SCRIPT_PARAM::FLOAT, &mt, "MT");
}

CPlayerScript::~CPlayerScript()
{

}

void CPlayerScript::begin()
{
	CGameObject* pPlayer = GetOwner();

	//for (int i = 0; i < 3; ++i)
	//{
	//	pPlayer->Animator3D()->EndEvent((UINT)PLAYER_ANIMATION_TYPE::RELOAD + i)
	//		= std::make_shared<std::function<void()>>([=]()
	//			{
	//				tState = PlayerMgr::PLAYER_STATE::IDLE;
	//			});
	//	pPlayer->Animator3D()->StartEvent((UINT)PLAYER_ANIMATION_TYPE::DRAW + i)
	//		= std::make_shared<std::function<void()>>([=]()
	//			{
	//				tState = PlayerMgr::PLAYER_STATE::DRAW;
	//			});
	//	pPlayer->Animator3D()->EndEvent((UINT)PLAYER_ANIMATION_TYPE::DRAW + i)
	//		= std::make_shared<std::function<void()>>([=]()
	//			{
	//				tState = PlayerMgr::PLAYER_STATE::IDLE;
	//			});
	//}
	//pPlayer->Animator3D()->EndEvent((UINT)PLAYER_ANIMATION_TYPE::SNIPER_FIRE)
	//	= std::make_shared<std::function<void()>>([=]()
	//		{
	//			tState = PlayerMgr::PLAYER_STATE::IDLE;
	//		});

	// Test code
	pPlayer->Animator3D()->EndEvent((UINT)PLAYER_ANIMATION_TYPE::RELOAD)
		= std::make_shared<std::function<void()>>([=]()
			{
				tState = PlayerMgr::PLAYER_STATE::IDLE;
				iAmmo = 30;
			});
	pPlayer->Animator3D()->EndEvent((UINT)PLAYER_ANIMATION_TYPE::DRAW)
		= std::make_shared<std::function<void()>>([=]()
			{
				tState = PlayerMgr::PLAYER_STATE::IDLE;
			});
}

void CPlayerScript::tick()
{
	//Look();
	//CatchRaycast();
	Movement();
}

void CPlayerScript::finaltick()
{
	//Movement();

	wstring str = std::to_wstring(iAmmo) + L"/ 30";
	CFontMgr::GetInst()->DrawFont(str.c_str(), 20, 20, 20, FONT_RGBA(255, 0, 0, 255));
}

void CPlayerScript::Look()
{
	CCamera* MainCam = CRenderMgr::GetInst()->GetMainCam();
	if (nullptr == MainCam)
		MainCam = CameraMgr::GetInst()->GetCamObj(L"MainCamera")->Camera();

	tRayInfo rayInfo{};
	rayInfo.fDamage = 0.f;
	rayInfo.iLayerIdx = GetOwner()->GetLayerIndex();
	rayInfo.vDir = MainCam->Transform()->GetRelativeDir(DIR_TYPE::FRONT);;
	rayInfo.vStart = MainCam->Transform()->GetRelativePos();
	rayInfo.matWorld = MainCam->Transform()->GetDrawRayMat();
	rayInfo.tRayType = (UINT)RAYCAST_TYPE::LOOK;
	RaycastMgr::GetInst()->SetPlayerRayInfo(rayInfo);
}

void CPlayerScript::ShootBullet()
{
	if (tState == PlayerMgr::PLAYER_STATE::RELOAD || tState == PlayerMgr::PLAYER_STATE::DRAW 
		|| tState == PlayerMgr::PLAYER_STATE::FIRE || iAmmo <= 0)
		return;

	CCamera* MainCam = CRenderMgr::GetInst()->GetMainCam();
	if (nullptr == MainCam)
		MainCam = CameraMgr::GetInst()->GetCamObj(L"MainCamera")->Camera();
	CGameObject* pPlayer = GetOwner();

	//--iAmmo;

	tRayInfo rayInfo{};
	rayInfo.fDamage = 10.f;
	rayInfo.iLayerIdx = pPlayer->GetLayerIndex();
	rayInfo.vDir = MainCam->Transform()->GetRelativeDir(DIR_TYPE::FRONT);
	rayInfo.vStart = MainCam->Transform()->GetRelativePos();
	rayInfo.matWorld = MainCam->Transform()->GetDrawRayMat();
	rayInfo.tRayType = (UINT)RAYCAST_TYPE::SHOOT;
	//RaycastMgr::GetInst()->SetPlayerRayInfo(rayInfo);
	RaycastMgr::GetInst()->DoRaycast(rayInfo);
	Vec3 vPos = WeaponMgr::GetInst()->GetCurWeaponMuzzlePos();
	Vec3 vRot = WeaponMgr::GetInst()->GetCurWeapon()->Transform()->GetRelativeRot();

	WeaponMgr::GetInst()->MuzzleFlash(vPos, vRot);
	
	WeaponMgr::GetInst()->Play(GUN_ANIMATION_TYPE::FIRE, false);
	CGameObject* pGun = WeaponMgr::GetInst()->GetCurWeapon();
	int weaponIdx = WeaponMgr::GetInst()->GetCurWeaponPlayerAnim(PLAYER_ANIMATION_TYPE::FIRE);

	if(weaponIdx == (int)PLAYER_ANIMATION_TYPE::SNIPER_FIRE)
	{
		tState = PlayerMgr::PLAYER_STATE::FIRE;
		pPlayer->Animator3D()->Play(weaponIdx, false);
		AnimationMgr::GetInst()->AnimationSync(pGun->Animator3D(), pPlayer->Animator3D());
	}
	
}

void CPlayerScript::ShootMissile()
{
	CCamera* MainCam = CRenderMgr::GetInst()->GetMainCam();
	if (nullptr == MainCam)
		MainCam = CameraMgr::GetInst()->GetCamObj(L"MainCamera")->Camera();

	tRay Ray = MainCam->GetRay();
	Vec3 vStart = Ray.vStart;

	CGameObject* pBullet = new CGameObject;
	pBullet->SetName(L"Player Missile");
	pBullet->AddComponent(new CTransform);
	pBullet->AddComponent(new CRigidBody(RIGID_BODY_SHAPE_TYPE::BOX));
	pBullet->AddComponent(new CCollider3D(false));
	pBullet->AddComponent(new CBulletScript);

	SpawnGameObject(pBullet, vStart, LAYER_TYPE::PlayerBullet);
}

void CPlayerScript::Reload()
{
	if (tState == PlayerMgr::PLAYER_STATE::RELOAD || tState == PlayerMgr::PLAYER_STATE::DRAW)
		return;

	int mReloadType = WeaponMgr::GetInst()->GetCurWeaponPlayerAnim(PLAYER_ANIMATION_TYPE::RELOAD);

	CGameObject* pPlayer = GetOwner();
	CGameObject* pGun = WeaponMgr::GetInst()->GetCurWeapon();

	pPlayer->Animator3D()->Play(mReloadType, false);
	WeaponMgr::GetInst()->Play(GUN_ANIMATION_TYPE::RELOAD, false);

	AnimationMgr::GetInst()->AnimationSync(pGun->Animator3D(), pPlayer->Animator3D());

	tState = PlayerMgr::PLAYER_STATE::RELOAD;
}

void CPlayerScript::Draw(int _Idx)
{
	CGameObject* pPlayer = GetOwner();

	if(WeaponMgr::GetInst()->ChangeWeapon(_Idx))
	{
		pPlayer->Animator3D()->Play(WeaponMgr::GetInst()->GetCurWeaponPlayerAnim(PLAYER_ANIMATION_TYPE::DRAW), false);
		tState = PlayerMgr::PLAYER_STATE::DRAW;
	}
}

void CPlayerScript::Movement()
{
	fRateOfFireAcc += DT;

	CGameObject* pCamObj = GetOwner()->GetFollowObj();
	if (nullptr == pCamObj)
		pCamObj = CameraMgr::GetInst()->GetCamObj(L"MainCamera");

	CGameObject* pPlayerFbx = GetOwner();
	//CGameObject* pPlayerBody = PlayerMgr::GetInst()->GetPlayerBody();

	Vec3 vPlayerPos = PlayerMgr::GetInst()->GetPlayerCameraPos();
	Vec3 vCamRot	= pCamObj->Transform()->GetRelativeRot();

	Vec3 vPlayerBodyFront	= pPlayerFbx->Transform()->GetRelativeDir(DIR_TYPE::FRONT);
	Vec3 vPlayerBodyUp		= pPlayerFbx->Transform()->GetRelativeDir(DIR_TYPE::UP);
	Vec3 vPlayerBodyRight	= pPlayerFbx->Transform()->GetRelativeDir(DIR_TYPE::RIGHT);
	Vec3 vPlayerBodyPos		= pPlayerFbx->Transform()->GetRelativePos();
	CRigidBody* pPlayerRB = pPlayerFbx->RigidBody();

	Vec2 vMouseDir		= CKeyMgr::GetInst()->GetMouseDir();
	Vec3 vPrevCamRot	= vCamRot;
	vCamRot.y += (DT * vMouseDir.x * 0.2f);
	vCamRot.x -= (DT * vMouseDir.y * 0.2f);
	vCamRot.z = 0;

	//vCamRot = XMVectorLerpV(vPrevCamRot, vCamRot, Vec3(DT, DT, DT));

	//Vec3 vSmoothRot = Vec3::SmoothStep(vCamRot, vPrevCamRot, 0.5f);
	pPlayerFbx->Transform()->SetRelativeRot(vCamRot);
	pPlayerFbx->Transform()->SetMt(mt);
	pCamObj->Transform()->SetRelativeRot(vCamRot);
	//pPlayerFbx->Transform()->SetRelativeRot(vCamRot);

	Vec3 vPrevCamPos	= pCamObj->Transform()->GetRelativePos();
	//vPlayerPos = XMVectorLerpV(vPlayerPos, vPlayerBodyPos, Vec3(0.1f, 0.1f, 0.1f));
	//Vec3 vSmoothPos		= Vec3::SmoothStep(vPrevCamPos, vPlayerPos, 0.1f);

	//pPlayerFbx->Transform()->SetRelativePos(vPlayerBodyPos);
	pCamObj->Transform()->SetRelativePos(vPlayerPos);

	float _fSpeed		= fSpeed;
	Vec3 final_velocity = Vec3(0.f, 0.f, 0.f);

	UINT uiFront	= (1 << 0);
	UINT uiBack		= (1 << 0);
	UINT uiRight	= (1 << 1);
	UINT uiLeft		= (1 << 1);
	UINT uiIdle		= (1 << 2);
	UINT flag = uiIdle;

	if (KEY_PRESSED(KEY::_1))
	{
		Draw(SMG_IDX);
	}
	if (KEY_PRESSED(KEY::_2))
	{
		Draw(SNIPER_IDX);
	}
	if (KEY_PRESSED(KEY::_3))
	{
		Draw(PISTOL_IDX);
	}

	if (KEY_PRESSED(KEY::R))
	{
		Reload();
	}

	if (KEY_PRESSED(KEY::LSHIFT))
	{
		_fSpeed *= 1.3f;
	}

	if (KEY_PRESSED(KEY::W))
	{
		flag |= uiFront;
		final_velocity += vPlayerBodyFront * DT * _fSpeed;
	}
	if (KEY_PRESSED(KEY::S))
	{
		flag |= uiBack;
		final_velocity += vPlayerBodyFront * DT * -_fSpeed;
	}
	if (KEY_PRESSED(KEY::A))
	{
		flag |= uiLeft;
		final_velocity += vPlayerBodyRight * DT * -_fSpeed;
	}

	if (KEY_PRESSED(KEY::D))
	{
		flag |= uiRight;
		final_velocity += vPlayerBodyRight * DT * _fSpeed;
	}
	if (KEY_PRESSED(KEY::SPACE))
	{
		final_velocity += vPlayerBodyUp * DT * fJump;
	}
	if (KEY_PRESSED(KEY::V))
	{
		if (fRateOfFireAcc >= fRateOfFire)
		{
			fRateOfFireAcc = 0.0f;
			ShootBullet();
		}
	}
	if (KEY_PRESSED(KEY::F))
	{
		final_velocity = Vec3(0.f, 0.f, 0.f);
		pPlayerRB->SetLinearVelocityZero();
	}

	pPlayerRB->SetLinearVelocity(final_velocity * _fSpeed);

	if (tState != PlayerMgr::PLAYER_STATE::RELOAD && tState != PlayerMgr::PLAYER_STATE::DRAW && tState != PlayerMgr::PLAYER_STATE::FIRE)
	{
		if (flag & uiFront)
		{
			pPlayerFbx->Animator3D()->Play(WeaponMgr::GetInst()->GetCurWeaponPlayerAnim(PLAYER_ANIMATION_TYPE::WALK_FORWARD), true);
		}
		//else if (flag & uiBack)
		//{
		//	pPlayerObj->Animator3D()->Play(WeaponMgr::GetInst()->GetCurWeaponPlayerAnim(PLAYER_ANIMATION_TYPE::WALK_BACK), true);
		//}
		else if (flag & uiRight)
		{
			pPlayerFbx->Animator3D()->Play(WeaponMgr::GetInst()->GetCurWeaponPlayerAnim(PLAYER_ANIMATION_TYPE::WALK_RIGHT), true);
		}
		else if (flag & uiLeft)
		{
			pPlayerFbx->Animator3D()->Play(WeaponMgr::GetInst()->GetCurWeaponPlayerAnim(PLAYER_ANIMATION_TYPE::WALK_LEFT), true);
		}
		else if (flag & uiIdle)
		{
			pPlayerFbx->Animator3D()->Play(WeaponMgr::GetInst()->GetCurWeaponPlayerAnim(PLAYER_ANIMATION_TYPE::IDLE), true);
		}
	}
}

void CPlayerScript::Attacked(float _Damage)
{
	if (iPlayerHp < _Damage)
		iPlayerHp = 0;
	else
		iPlayerHp -= _Damage;
}

void CPlayerScript::CatchRaycast()
{
	CGameObject* pPlayer = GetOwner();
	CCamera* MainCam = CRenderMgr::GetInst()->GetMainCam();
	if (nullptr == MainCam)
		MainCam = CameraMgr::GetInst()->GetCamObj(L"MainCamera")->Camera();

	vector<tRayInfo> monsRayInfo = RaycastMgr::GetInst()->GetRayInfo();
	for (int i = 0; i < monsRayInfo.size(); ++i)
	{
		tRayInfo rayInfo = monsRayInfo[i];

		// ================================================================================
		Vec3 vFront = rayInfo.vDir;
		Vec3 vStart = rayInfo.vStart;
		PxVec3 vDir = PxVec3(vFront.x, vFront.y, vFront.z);
		PxVec3 origin = PxVec3(vStart.x, vStart.y, vStart.z);
		// ================================================================================
		PxShape* shape = pPlayer->RigidBody()->GetShape();
		// ================================================================================
		PxGeomRaycastHit hitInfo;
		const PxU32 maxHits = 1;
		const PxHitFlags hitFlags = PxHitFlag::eANY_HIT | PxHitFlag::eMESH_MULTIPLE;
		const PxU32 stride = sizeof(PxGeomRaycastHit);
		const PxGeometryQueryFlags queryFlags = PxGeometryQueryFlag::eDEFAULT;
		PxRaycastThreadContext* threadContext = NULL;
		// ================================================================================
		vDir.normalize();

		PxTransform pose;
		// ================================================================================
		if (pPlayer->RigidBody()->GetRigidType() == RIGID_BODY_TYPE::DYNAMIC)
			pose = PxShapeExt::getGlobalPose(*shape, *pPlayer->RigidBody()->GetDynamicBody());
		else
			pose = PxShapeExt::getGlobalPose(*shape, *pPlayer->RigidBody()->GetStaticBody());
		// ================================================================================
		PxU32 hitCount = PxGeometryQuery::raycast(origin, vDir,
			shape->getGeometry(), pose,
			1e6,
			hitFlags,
			maxHits, &hitInfo,
			stride, queryFlags, threadContext);
		if (hitCount != 0)
		{
			if(rayInfo.tRayType == (UINT)RAYCAST_TYPE::SHOOT)
			{
				Attacked(rayInfo.fDamage);
				RaycastMgr::GetInst()->AddRaycastDraw(vFront, vStart, rayInfo.matWorld, Vec4(1.f, 1.f, 0.f, 1.f));
			}else
			{
				tRayInfo rayReply{};
				rayReply.fDamage = 0.0f;
				rayReply.iLayerIdx = pPlayer->GetLayerIndex();
				rayReply.vDir = MainCam->Transform()->GetRelativeDir(DIR_TYPE::FRONT);
				rayReply.vStart = MainCam->Transform()->GetRelativePos();
				rayReply.matWorld = MainCam->Transform()->GetDrawRayMat();
				rayReply.tRayType = (UINT)RAYCAST_TYPE::REPLY;
				rayReply.iID = rayInfo.iID;
				RaycastMgr::GetInst()->SetPlayerRayInfo(rayReply);
			}
			//RaycastMgr::GetInst()->UseRayInfo();
			//Raycast(RaycastMgr::GetInst()->GetRaycastInfo());
		}
	}

	RaycastMgr::GetInst()->ClearRayInfo();
}

void CPlayerScript::BeginOverlap(CCollider3D* _Other)
{

}

void CPlayerScript::Raycast(tRayInfo _RaycastInfo)
{
	if (iPlayerHp < _RaycastInfo.fDamage)
		iPlayerHp = 0;
	else
		iPlayerHp -= _RaycastInfo.fDamage;
}

void CPlayerScript::SaveToLevelFile(FILE* _File)
{
	fwrite(&fSpeed, sizeof(float), 1, _File);
}

void CPlayerScript::LoadFromLevelFile(FILE* _File)
{
	fread(&fSpeed, sizeof(float), 1, _File);
}