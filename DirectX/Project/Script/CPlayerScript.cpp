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

#include "CBulletScript.h"
#include "CMissileScript.h"


CPlayerScript::CPlayerScript()
	: CScript((UINT)SCRIPT_TYPE::PLAYERSCRIPT)
	, fSpeed(100.f)
	, fJump(200.f)
	, fRateOfFire(0.2f)
	, fRateOfFireAcc(0.0f)
	, fMouseAcces(1.f)
	, iPlayerHp(1000.f)
	, bReloading(false)
	, tState(PlayerMgr::PLAYER_STATE::IDLE)
{
	AddScriptParam(SCRIPT_PARAM::FLOAT, &fSpeed, "Player Speed");
	AddScriptParam(SCRIPT_PARAM::FLOAT, &fMouseAcces, "Mouse Speed");
}

CPlayerScript::~CPlayerScript()
{

}

void CPlayerScript::begin()
{
	// 동적 재질 생성
	//MeshRender()->GetDynamicMaterial(0);
	CGameObject* pPlayer = GetOwner();
	pPlayer->Animator3D()->EndEvent((UINT)ANIMATION_TYPE::RELOAD)
	= std::make_shared<std::function<void()>>([=]()
		{
			tState = PlayerMgr::PLAYER_STATE::IDLE;
		});
}

void CPlayerScript::tick()
{
	Look();
	Movement();
	CatchRaycast();
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
	CCamera* MainCam = CRenderMgr::GetInst()->GetMainCam();
	if (nullptr == MainCam)
		MainCam = CameraMgr::GetInst()->GetCamObj(L"MainCamera")->Camera();
	CGameObject* pPlayer = GetOwner();

	tRayInfo rayInfo{};
	rayInfo.fDamage = 10.f;
	rayInfo.iLayerIdx = pPlayer->GetLayerIndex();
	rayInfo.vDir = MainCam->Transform()->GetRelativeDir(DIR_TYPE::FRONT);
	rayInfo.vStart = MainCam->Transform()->GetRelativePos();
	rayInfo.matWorld = MainCam->Transform()->GetDrawRayMat();
	rayInfo.tRayType = (UINT)RAYCAST_TYPE::SHOOT;
	RaycastMgr::GetInst()->SetPlayerRayInfo(rayInfo);

	Vec3 vPos = WeaponMgr::GetInst()->GetCurWeaponMuzzlePos();
	Vec3 vRot = WeaponMgr::GetInst()->GetCurWeapon()->Transform()->GetRelativeRot();
	
	//vPos = PlayerMgr::GetInst()->GetConvertAnimationPos(vPos);
	{
		int randX = RandMgr::GetInst()->GetRandMuzzleX(2);
		int randY = RandMgr::GetInst()->GetRandMuzzleY(2);
		Vec2 muzzleSize = Vec2(512.f / 2.f, 512.f / 2.f);

		CGameObject* Light = new CGameObject;
		Light->SetName(L"Point Light");
		Light->AddComponent(new CMeshRender);
		Light->AddComponent(new CTransform);
		Light->AddComponent(new CLight3D);
		Light->AddComponent(new CAnimator2D);

		Light->Animator2D()->Create(L"muzzle"
			, CResMgr::GetInst()->FindRes<CTexture>(L"texture\\effect\\Tex_Assault_Muzzle_Flash_Front.tga")
			, Vec2(muzzleSize.x * randX, muzzleSize.y * randY)
			, muzzleSize
			, 1
			, 1
			, Vec2::Zero
			, Vec2::Zero
		);
		Light->Animator2D()->Play(L"muzzle", false);

		Light->Light3D()->SetRadius(100.f);
		Light->Light3D()->SetLightType(LIGHT_TYPE::POINT);
		Light->Light3D()->SetLightColor(Vec3(1.f, 1.f, 0.f));
		Light->Light3D()->SetLightAmbient(Vec3(0.15f, 0.15f, 0.15f));
		Light->Light3D()->SetLifeSpan(0.01f);
		Light->Transform()->SetRelativeRot(vRot);
		Light->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
		Light->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Std3DShaderMtrl"), 0);
		//Light->MeshRender()->SetDynamicMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Std3D_DeferredMtrl"), 0);
		//Light->MeshRender()->GetMaterial(0)->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"texture\\effect\\Tex_Assault_Muzzle_Flash_Front.tga"));
		//Light->MeshRender()->GetMaterial(0)->SetTexParam(TEX_1, nullptr);

		//Light->MeshRender()->GetDynamicMaterial(0)->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"texture\\effect\\Tex_Assault_Muzzle_Flash_Front.tga"));
		//Light->MeshRender()->GetDynamicMaterial(0)->SetTexParam(TEX_1, nullptr);

		SpawnGameObject(Light, vPos, 0);
	}

	tState = PlayerMgr::PLAYER_STATE::FIRE;
	WeaponMgr::GetInst()->Play(GUN_ANIMATION_TYPE::FIRE, false);
}

void CPlayerScript::ShootMissile()
{
	CCamera* MainCam = CRenderMgr::GetInst()->GetMainCam();
	if (nullptr == MainCam)
		MainCam = CameraMgr::GetInst()->GetCamObj(L"MainCamera")->Camera();

	tRay Ray = MainCam->GetRay();
	Vec3 vStart = Ray.vStart;

	CGameObject* pBullet = new CGameObject;
	pBullet->SetName(L"Player Bullet");
	pBullet->AddComponent(new CTransform);
	pBullet->AddComponent(new CRigidBody(RIGID_BODY_SHAPE_TYPE::BOX));
	pBullet->AddComponent(new CCollider3D(false));
	pBullet->AddComponent(new CBulletScript);

	SpawnGameObject(pBullet, vStart, L"Bullet");


}

void CPlayerScript::Reload()
{
	if (tState == PlayerMgr::PLAYER_STATE::RELOAD)
		return;

	CGameObject* pPlayer = GetOwner();
	pPlayer->Animator3D()->Play((UINT)ANIMATION_TYPE::RELOAD, false);
	WeaponMgr::GetInst()->Play(GUN_ANIMATION_TYPE::RELOAD, false);

	tState = PlayerMgr::PLAYER_STATE::RELOAD;
}

void CPlayerScript::Movement()
{
	fRateOfFireAcc += DT;

	CGameObject* pCamObj = GetOwner()->GetFollowObj();
	if (nullptr == pCamObj)
		pCamObj = CameraMgr::GetInst()->GetCamObj(L"MainCamera");

	CGameObject* pPlayerObj = GetOwner();

	Vec3 vPlayerPos = PlayerMgr::GetInst()->GetPlayerCameraPos();
	Vec3 vCamRot = pCamObj->Transform()->GetRelativeRot();

	Vec3 vPlayerFront = pPlayerObj->Transform()->GetRelativeDir(DIR_TYPE::FRONT);
	Vec3 vPlayerUp = pPlayerObj->Transform()->GetRelativeDir(DIR_TYPE::UP);
	Vec3 vPlayerRight = pPlayerObj->Transform()->GetRelativeDir(DIR_TYPE::RIGHT);

	CRigidBody* pPlayerRB = pPlayerObj->RigidBody();

	Vec2 vMouseDir = CKeyMgr::GetInst()->GetMouseDir();
	Vec3 vPrevCamRot = vCamRot;
	vCamRot.y += (DT * vMouseDir.x * 1.f);
	vCamRot.x -= (DT * vMouseDir.y * 1.f);
	vCamRot.z = 0;

	//pPlayerObj->Transform()->SetRelativeRot(Vec3(0.f, vCamRot.y, 0.f));
	Vec3 vSmoothRot = Vec3::SmoothStep(vCamRot, vPrevCamRot, 0.5f);
	pPlayerObj->Transform()->SetRelativeRot(vSmoothRot);
	pCamObj->Transform()->SetRelativeRot(vSmoothRot);

	//vPlayerPos += pCamObj->Transform()->GetFollowOffset();
	Vec3 vPrevCamPos = pCamObj->Transform()->GetRelativePos();
	Vec3 vSmoothPos = Vec3::SmoothStep(vPrevCamPos, vPlayerPos, 0.5f);
	
	pCamObj->Transform()->SetRelativePos(vSmoothPos);

	float _fSpeed = fSpeed;
	//bool bKeyPressed = false;
	Vec3 final_velocity = Vec3(0.f, 0.f, 0.f);


	if (KEY_PRESSED(KEY::LSHIFT))
	{
		_fSpeed *= 2.f;
	}

	// 0001
	UINT uiFront = (1 << 0);
	UINT uiBack = (1 << 0);

	// 0010
	UINT uiRight = (1 << 1);
	UINT uiLeft = (1 << 1);

	// 0100
	UINT uiIdle = (1 << 2);


	UINT flag = uiIdle;

	if (KEY_PRESSED(KEY::R))
	{
		Reload();
	}

	if (KEY_PRESSED(KEY::W))
	{
		flag |= uiFront;
		final_velocity += vPlayerFront * DT * _fSpeed;
		//bKeyPressed = true;
	}
	if (KEY_PRESSED(KEY::S))
	{
		flag |= uiBack;
		final_velocity += vPlayerFront * DT * -_fSpeed;
		//bKeyPressed = true;
	}
	if (KEY_PRESSED(KEY::A))
	{
		flag |= uiLeft;
		final_velocity += vPlayerRight * DT * -_fSpeed;
		//bKeyPressed = true;
	}

	if (KEY_PRESSED(KEY::D))
	{
		flag |= uiRight;

		final_velocity += vPlayerRight * DT * _fSpeed;
		//bKeyPressed = true;
	}
	
	if (tState != PlayerMgr::PLAYER_STATE::RELOAD)
	{
		if (flag & uiFront)
			pPlayerObj->Animator3D()->Play((UINT)ANIMATION_TYPE::WALK_FORWARD, true);
		else if (flag & uiBack)
			pPlayerObj->Animator3D()->Play((UINT)ANIMATION_TYPE::WALK_BACK, true);
		else if (flag & uiRight)
			pPlayerObj->Animator3D()->Play((UINT)ANIMATION_TYPE::WALK_RIGHT, true);
		else if (flag & uiLeft)
			pPlayerObj->Animator3D()->Play((UINT)ANIMATION_TYPE::WALK_LEFT, true);
		else if (flag & uiIdle)
		{
			pPlayerObj->Animator3D()->Play((UINT)ANIMATION_TYPE::IDLE, true);
		}
	}



	if (KEY_PRESSED(KEY::SPACE))
	{
		pPlayerObj->Animator3D()->Play((UINT)ANIMATION_TYPE::JUMP, false);
		final_velocity += vPlayerUp * DT * fJump;
		//bKeyPressed = true;
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
}

void CPlayerScript::CatchRaycast()
{
	CGameObject* pPlayer = GetOwner();

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
		const PxHitFlags hitFlags = PxHitFlag::eDEFAULT;
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
			//RaycastMgr::GetInst()->UseRayInfo();
			RaycastMgr::GetInst()->AddRaycastDraw(vFront, vStart, rayInfo.matWorld, Vec4(1.f, 1.f, 0.f, 1.f));
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