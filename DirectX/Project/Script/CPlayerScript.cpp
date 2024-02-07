#include "pch.h"
#include "CPlayerScript.h"

#include <Engine/PlayerMgr.h>
#include <Engine\CMeshRender.h>
#include <Engine\CMaterial.h>
#include <Engine\CRenderMgr.h>
#include <Engine\CameraMgr.h>
#include <Engine\RaycastMgr.h>
#include <Engine\WeaponMgr.h>

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
}

void CPlayerScript::tick()
{
	Look();
	Move();
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
	rayInfo.vDir = MainCam->Transform()->GetRelativeDir(DIR_TYPE::FRONT);;
	rayInfo.vStart = MainCam->Transform()->GetRelativePos();
	rayInfo.matWorld = MainCam->Transform()->GetDrawRayMat();
	rayInfo.tRayType = (UINT)RAYCAST_TYPE::SHOOT;
	RaycastMgr::GetInst()->SetPlayerRayInfo(rayInfo);

	Vec3 vPos = WeaponMgr::GetInst()->GetCurWeaponMuzzlePos();

	{
		CGameObject* Light = new CGameObject;
		Light->SetName(L"Point Light");
		Light->AddComponent(new CTransform);
		Light->AddComponent(new CLight3D);

		//Light->Transform()->SetRelativeRot(Vec3(0.f, 0.f, 0.f));
		Light->Light3D()->SetRadius(100.f);
		//Light->Light3D()->SetShadow(true);
		//Light->Light3D()->SetFloatConstant(0, 5);
		//Light->Light3D()->SetFloatConstant(1, 0.0004);
		Light->Light3D()->SetLightType(LIGHT_TYPE::POINT);
		Light->Light3D()->SetLightColor(Vec3(1.f, 1.f, 0.f));
		Light->Light3D()->SetLightAmbient(Vec3(0.15f, 0.15f, 0.0f));

		SpawnGameObject(Light, vPos, 1);
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
	pBullet->SetName(L"Player Bullet");
	pBullet->AddComponent(new CTransform);
	pBullet->AddComponent(new CRigidBody(RIGID_BODY_SHAPE_TYPE::BOX));
	pBullet->AddComponent(new CCollider3D(false));
	pBullet->AddComponent(new CBulletScript);

	SpawnGameObject(pBullet, vStart, L"Bullet");
}

void CPlayerScript::Move()
{
	fRateOfFireAcc += DT;

	CGameObject* pCamObj = GetOwner()->GetFollowObj();
	if (nullptr == pCamObj)
		pCamObj = CameraMgr::GetInst()->GetCamObj(L"MainCamera");

	CGameObject* pPlayerObj = GetOwner();

	//Vec3 vPlayerPos = pPlayerObj->Transform()->GetRelativePos();
	Vec3 vPlayerPos = PlayerMgr::GetInst()->GetPlayerCameraPos();
	Vec3 vCamRot = pCamObj->Transform()->GetRelativeRot();

	Vec3 vPlayerFront = pPlayerObj->Transform()->GetRelativeDir(DIR_TYPE::FRONT);
	Vec3 vPlayerUp = pPlayerObj->Transform()->GetRelativeDir(DIR_TYPE::UP);
	Vec3 vPlayerRight = pPlayerObj->Transform()->GetRelativeDir(DIR_TYPE::RIGHT);

	CRigidBody* pPlayerRB = pPlayerObj->RigidBody();

	Vec2 vMouseDir = CKeyMgr::GetInst()->GetMouseDir();

	vCamRot.y += (DT * vMouseDir.x * 1.f);
	vCamRot.x -= (DT * vMouseDir.y * 1.f);
	vCamRot.z = 0;

	pPlayerObj->Transform()->SetRelativeRot(Vec3(0.f, vCamRot.y, 0.f));
	pCamObj->Transform()->SetRelativeRot(vCamRot);

	//vPlayerPos += pCamObj->Transform()->GetFollowOffset();
	pCamObj->Transform()->SetRelativePos(vPlayerPos);

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
	// 1 2
	if (flag & uiFront)
		pPlayerObj->Animator3D()->Play(ANIMATION_TYPE::WALK_FORWARD, true);
	else if (flag & uiBack)
		pPlayerObj->Animator3D()->Play(ANIMATION_TYPE::WALK_BACK, true);
	else if (flag & uiRight)
		pPlayerObj->Animator3D()->Play(ANIMATION_TYPE::WALK_RIGHT, true);
	else if (flag & uiLeft)
		pPlayerObj->Animator3D()->Play(ANIMATION_TYPE::WALK_LEFT, true);
	else if (flag & uiIdle)
	{
		pPlayerObj->Animator3D()->Play(ANIMATION_TYPE::IDLE, true);
	}

	if (KEY_PRESSED(KEY::SPACE))
	{
		pPlayerObj->Animator3D()->Play(ANIMATION_TYPE::JUMP, false);
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