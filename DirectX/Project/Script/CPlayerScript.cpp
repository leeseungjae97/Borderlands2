#include "pch.h"
#include "CPlayerScript.h"

#include <Engine/CEngine.h>
#include <Engine/PlayerMgr.h>
#include <Engine\CMeshRender.h>
#include <Engine\CMaterial.h>
#include <Engine\CRenderMgr.h>
#include <Engine\CameraMgr.h>
#include <Engine\RaycastMgr.h>
#include <Engine\WeaponMgr.h>
#include <Engine\RandMgr.h>
#include <Engine\AnimationMgr.h>
#include <Engine\ParticleMgr.h>
#include <Engine\CUI.h>
#include <Engine\CFontMgr.h>

#include "CBulletScript.h"
#include "CEnemyScript.h"
#include "CMissileScript.h"
#include "CWarriorScript.h"


CPlayerScript::CPlayerScript()
	: CScript((UINT)SCRIPT_TYPE::PLAYER_SCRIPT)
	, fSpeed(300.f)
	, fJump(1000.f)
	, fRateOfFire(0.05f)
	, fRateOfFireAcc(0.0f)
	, fMouseAcces(1.f)
	, iPlayerHp(5000.f)
	, iAmmo(30)
	, iAmmoCapa(30)
	, fBurnTime(0.0f)
	, bJump(false)
	, tState(PlayerMgr::PLAYER_STATE::IDLE)
{
	AddScriptParam(SCRIPT_PARAM::FLOAT, &fSpeed, "Player Speed");
	AddScriptParam(SCRIPT_PARAM::FLOAT, &fJump, "Player Jump");
	AddScriptParam(SCRIPT_PARAM::FLOAT, &fMouseAcces, "Mouse Speed");
}

CPlayerScript::~CPlayerScript()
{
}

void CPlayerScript::begin()
{
	CGameObject* pPlayer = GetOwner();

	CreateUI();
	//for (int i = 0; i < 3; ++i)
	//{
	//	pPlayer->Animator3D()->EndEvent((UINT)PLAYER_ANIMATION_TYPE::RELOAD + i)
	//		= std::make_shared<std::function<void()>>([=]()
	//			{
	//				tState = PlayerMgr::PLAYER_STATE::IDLE;
	//				iAmmo = 30;
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

	//makeCollider();

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
	Look();
	//CatchRaycast();
	Movement();
	Burn();

	if (m_pAmmoText)
		m_pAmmoText->SetText(std::to_wstring(iAmmo) + L"/ 30");
	if (m_pHPText)
		m_pHPText->SetText(std::to_wstring(iPlayerHp));

	//float ratio = (1.f - (float)iAmmo / (float)iAmmoCapa);
	//m_pUI_EnemyHp->MeshRender()->GetMaterial(0)->SetScalarParam(FLOAT_0, &ratio);
}

void CPlayerScript::finaltick()
{
	static bool init = false;
	if(!init)
	{
		//CreateUI();
		init = true;
	}
	//Movement();

	//if (pHeadCollider)
	//{
	//	CGameObject* pPlayer = GetOwner();
	//	int headIdx = pPlayer->Animator3D()->GetHeadIdx();
	//	Vec3 vPos = AnimationMgr::GetInst()->BonePos(headIdx, pPlayer);
	//	Vec3 vRot = AnimationMgr::GetInst()->BoneRot(headIdx, pPlayer);

	//	pHeadCollider->Collider3D()->SetColliderPos(vPos);
	//	pHeadCollider->Collider3D()->SetColliderRot(vRot);
	//}
}

void CPlayerScript::CreateUI()
{
	Ptr<CGraphicsShader> UIShader = (CGraphicsShader*)CResMgr::GetInst()->FindRes<CGraphicsShader>(L"UI2DShader").Get();
	Ptr<CGraphicsShader> adjustUIShader = (CGraphicsShader*)CResMgr::GetInst()->FindRes<CGraphicsShader>(L"AdjustUI2DShader").Get();
	Ptr<CMaterial> pMtrl = nullptr;
	Ptr<CTexture> pTex = nullptr;
	Vec2 vResol = CEngine::GetInst()->GetWindowResolution();
	Vec2 vHalfR = vResol / 2.f;
	float backWidth = 0;
	float backHeight = 0;

	m_pAmmoText = new CUI();
	m_pAmmoText->SetName(L"Ammo Text");
	m_pAmmoText->AddComponent(new CTransform);
	m_pAmmoText->AddComponent(new CMeshRender);
	m_pAmmoText->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	m_pAmmoText->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"AdjustUI2DShaderMtrl"), 0);
	m_pAmmoText->Transform()->SetRelativeScale(Vec3(100.f, 100.f, 1.f));
	m_pAmmoText->SetTextSize(20.f);
	m_pAmmoText->SetTextNormalColor(Vec4(1.f, 1.f, 1.f, 1.f));
	m_pAmmoText->SetText(std::to_wstring(iAmmo) + L"/ 30");

	SpawnGameObject(m_pAmmoText, Vec3(1000.f, vResol.y - 50.f, 1.f), LAYER_TYPE::ViewPortUI);

	m_pHPText = new CUI();
	m_pHPText->SetName(L"HP Text");
	m_pHPText->AddComponent(new CTransform);
	m_pHPText->AddComponent(new CMeshRender);
	m_pHPText->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	m_pHPText->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"AdjustUI2DShaderMtrl"), 0);
	m_pHPText->Transform()->SetRelativeScale(Vec3(100.f, 100.f, 1.f));

	m_pHPText->SetTextSize(20.f);
	m_pHPText->SetTextNormalColor(Vec4(1.f, 1.f, 1.f, 1.f));
	m_pHPText->SetText(std::to_wstring(iPlayerHp));

	SpawnGameObject(m_pHPText, Vec3(50.f, vResol.y - 50.f, 1.f), LAYER_TYPE::ViewPortUI);

	{
		pMtrl = new CMaterial(true);
		pMtrl->SetShader(UIShader);
		CResMgr::GetInst()->AddRes(L"UIHPBackMtrl", pMtrl);

		CGameObject* HP_UI_BACK = new CGameObject;

		pTex = CResMgr::GetInst()->FindRes<CTexture>(L"texture\\UI\\player_hp_back.png");
		backWidth = pTex->Width();
		backHeight = pTex->Height();

		HP_UI_BACK->SetName(L"UI HP BACK");
		HP_UI_BACK->AddComponent(new CTransform);
		HP_UI_BACK->AddComponent(new CMeshRender);

		HP_UI_BACK->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
		HP_UI_BACK->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"UIHPBackMtrl"), 0);
		HP_UI_BACK->MeshRender()->GetMaterial(0)->SetTexParam(TEX_0, pTex);
		HP_UI_BACK->Transform()->SetRelativeScale(Vec3(238.f, 38.f, 1.f));
		HP_UI_BACK->Transform()->SetRelativeRot(Vec3(0.f, 0.f, 3.f));

		SpawnGameObject(HP_UI_BACK, Vec3(238.f, -331.f, 0.f), LAYER_TYPE::ViewPortUI);
	}
	{
		pMtrl = new CMaterial(true);
		pMtrl->SetShader(UIShader);
		CResMgr::GetInst()->AddRes(L"UIHPMtrl", pMtrl);

		pTex = CResMgr::GetInst()->FindRes<CTexture>(L"texture\\UI\\player_hp_front.png");

		m_pUI_HP = new CGameObject;

		m_pUI_HP->SetName(L"UI HP");
		m_pUI_HP->AddComponent(new CTransform);
		m_pUI_HP->AddComponent(new CMeshRender);

		m_pUI_HP->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
		m_pUI_HP->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"UIHPMtrl"), 0);
		m_pUI_HP->MeshRender()->GetMaterial(0)->SetTexParam(TEX_0, pTex);
		m_pUI_HP->Transform()->SetRelativeScale(Vec3(204.f, 25.f, 1.f));
		m_pUI_HP->Transform()->SetRelativeRot(Vec3(0.f, 0.f, 3.f));

		SpawnGameObject(m_pUI_HP, Vec3(-475.f, -323.f, 0.f), LAYER_TYPE::ViewPortUI);
	}
	{
		pMtrl = new CMaterial(true);
		pMtrl->SetShader(UIShader);
		CResMgr::GetInst()->AddRes(L"UIAMMOBackMtrl", pMtrl);

		pTex = CResMgr::GetInst()->FindRes<CTexture>(L"texture\\UI\\player_ammo_back.png");

		CGameObject* AMMO_BACK_UI = new CGameObject;

		AMMO_BACK_UI->SetName(L"UI AMMO BACK");
		AMMO_BACK_UI->AddComponent(new CTransform);
		AMMO_BACK_UI->AddComponent(new CMeshRender);

		AMMO_BACK_UI->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
		AMMO_BACK_UI->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"UIAMMOBackMtrl"), 0);
		AMMO_BACK_UI->MeshRender()->GetMaterial(0)->SetTexParam(TEX_0, pTex);
		AMMO_BACK_UI->Transform()->SetRelativeScale(Vec3(220.f, 38.f, 1.f));
		AMMO_BACK_UI->Transform()->SetRelativeRot(Vec3(0.f, 0.f, -3.f));

		SpawnGameObject(AMMO_BACK_UI, Vec3(479.f, -327.f, 0.f), LAYER_TYPE::ViewPortUI);
	}
	{
		pMtrl = new CMaterial(true);
		pMtrl->SetShader(UIShader);
		CResMgr::GetInst()->AddRes(L"UIAMMOIconMtrl", pMtrl);

		pTex = CResMgr::GetInst()->FindRes<CTexture>(L"texture\\UI\\smg_ammo.png");
		backWidth = pTex->Width();
		backHeight = pTex->Height();

		m_pAmmoIcon = new CGameObject;

		m_pAmmoIcon->SetName(L"UI AMMO ICON");
		m_pAmmoIcon->AddComponent(new CTransform);
		m_pAmmoIcon->AddComponent(new CMeshRender);

		m_pAmmoIcon->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
		m_pAmmoIcon->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"UIAMMOIconMtrl"), 0);
		m_pAmmoIcon->MeshRender()->GetMaterial(0)->SetTexParam(TEX_0, pTex);
		m_pAmmoIcon->Transform()->SetRelativeScale(Vec3(32.f, 49.f, 1.f));
		m_pAmmoIcon->Transform()->SetRelativeRot(Vec3(0.f, 0.f, -3.f));
		SpawnGameObject(m_pAmmoIcon, Vec3(598.f, -333.f, 0.f), LAYER_TYPE::ViewPortUI);
	}
	{
		pMtrl = new CMaterial(true);
		pMtrl->SetShader(UIShader);
		CResMgr::GetInst()->AddRes(L"UIAMMOMtrl", pMtrl);

		pTex = CResMgr::GetInst()->FindRes<CTexture>(L"texture\\UI\\player_ammo_front.png");

		m_pUI_AMMO = new CGameObject;

		m_pUI_AMMO->SetName(L"UI AMMO");
		m_pUI_AMMO->AddComponent(new CTransform);
		m_pUI_AMMO->AddComponent(new CMeshRender);

		m_pUI_AMMO->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
		m_pUI_AMMO->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"UIAMMOMtrl"), 0);
		m_pUI_AMMO->MeshRender()->GetMaterial(0)->SetTexParam(TEX_0, pTex);
		m_pUI_AMMO->Transform()->SetRelativeScale(Vec3(207.f, 22.f, 1.f));
		m_pUI_AMMO->Transform()->SetRelativeRot(Vec3(0.f, 0.f, -3.f));
		SpawnGameObject(m_pUI_AMMO, Vec3(466.f, -319.f, 0.f), LAYER_TYPE::ViewPortUI);
	}

	{
		pMtrl = new CMaterial(true);
		pMtrl->SetShader(UIShader);
		CResMgr::GetInst()->AddRes(L"CrossHairMtrl", pMtrl);

		CGameObject* pCrossHair = new CGameObject;

		pCrossHair->SetName(L"UI Cross Hair");
		pCrossHair->AddComponent(new CTransform);
		pCrossHair->AddComponent(new CMeshRender);

		pCrossHair->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
		pCrossHair->MeshRender()->SetMaterial(pMtrl, 0);
		pCrossHair->MeshRender()->GetMaterial(0)->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"texture\\UI\\Crosshair.png"));
		pCrossHair->Transform()->SetRelativeScale(Vec3(80.f, 80.f, 1.f));
		SpawnGameObject(pCrossHair, Vec3(0.f, 0.f, 0.f), LAYER_TYPE::ViewPortUI);
	}
	{
		pMtrl = new CMaterial(true);
		pMtrl->SetShader(adjustUIShader);
		CResMgr::GetInst()->AddRes(L"EnemyHpMtrl", pMtrl);

		Ptr<CTexture> pTex = CResMgr::GetInst()->FindRes<CTexture>(L"texture\\UI\\enemy_hp_front.png");
		m_pUI_EnemyHp = new CGameObject;

		m_pUI_EnemyHp->SetName(L"UI Enemy Hp");
		m_pUI_EnemyHp->AddComponent(new CTransform);
		m_pUI_EnemyHp->AddComponent(new CMeshRender);

		m_pUI_EnemyHp->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
		m_pUI_EnemyHp->MeshRender()->SetMaterial(pMtrl, 0);
		m_pUI_EnemyHp->MeshRender()->GetMaterial(0)->SetTexParam(TEX_0, pTex);
		m_pUI_EnemyHp->Transform()->SetRelativeScale(Vec3(pTex->Width(), pTex->Height(), 1.f));
		SpawnGameObject(m_pUI_EnemyHp, Vec3(0.f, 50.f, 0.f), LAYER_TYPE::ViewPortUI);

		m_pUI_EnemyHp->SetObjectState(CGameObject::OBJECT_STATE::VISIBLE);
	}
	{
		pMtrl = new CMaterial(true);
		pMtrl->SetShader(UIShader);
		CResMgr::GetInst()->AddRes(L"EnemyHpBack", pMtrl);

		Ptr<CTexture> pTex = CResMgr::GetInst()->FindRes<CTexture>(L"texture\\UI\\enemy_hp_back.png");
		m_pUI_EnemyHp_Back = new CGameObject;

		m_pUI_EnemyHp_Back->SetName(L"UI Enemy Hp Back");
		m_pUI_EnemyHp_Back->AddComponent(new CTransform);
		m_pUI_EnemyHp_Back->AddComponent(new CMeshRender);

		m_pUI_EnemyHp_Back->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
		m_pUI_EnemyHp_Back->MeshRender()->SetMaterial(pMtrl, 0);
		m_pUI_EnemyHp_Back->MeshRender()->GetMaterial(0)->SetTexParam(TEX_0, pTex);
		m_pUI_EnemyHp_Back->Transform()->SetRelativeScale(Vec3(pTex->Width(), pTex->Height(), 1.f));
		SpawnGameObject(m_pUI_EnemyHp_Back, Vec3(0.f, 50.f, 0.f), LAYER_TYPE::ViewPortUI);

		m_pUI_EnemyHp_Back->SetObjectState(CGameObject::OBJECT_STATE::VISIBLE);
	}

}

void CPlayerScript::Look()
{
	CCamera* MainCam = CRenderMgr::GetInst()->GetMainCam();
	if (nullptr == MainCam)
		MainCam = CameraMgr::GetInst()->GetCamObj(L"MainCamera")->Camera();

	tRayInfo rayInfo{};
	rayInfo.fDamage = 0.f;
	rayInfo.iLayerIdx = GetOwner()->GetLayerIndex();
	rayInfo.vDir = MainCam->Transform()->GetRelativeDir(DIR_TYPE::FRONT);
	rayInfo.vStart = MainCam->Transform()->GetRelativePos();
	rayInfo.matWorld = MainCam->Transform()->GetDrawRayMat();
	rayInfo.tRayType = (UINT)RAYCAST_TYPE::LOOK;
	//RaycastMgr::GetInst()->SetPlayerRayInfo(rayInfo);

	CGameObject* hoverEnemy = nullptr;
	RaycastMgr::GetInst()->DoOneHitRaycast(rayInfo, &hoverEnemy, RAYCAST_GROUP_TYPE::Player);
	if (hoverEnemy)
	{
		if (hoverEnemy->GetScript<CEnemyScript>())
		{
			bHitEnemy = true;
			m_pUI_EnemyHp->SetObjectState(CGameObject::OBJECT_STATE::VISIBLE);
			m_pUI_EnemyHp_Back->SetObjectState(CGameObject::OBJECT_STATE::VISIBLE);
			hoverEnemy->GetName();

			float hpRatio = 1.f - hoverEnemy->GetScript<CEnemyScript>()->GetHpRatio();
			m_pUI_EnemyHp->MeshRender()->GetMaterial(0)->SetScalarParam(FLOAT_0, &hpRatio);

		}
		else if (hoverEnemy->GetScript<CWarriorScript>())
		{
			m_pUI_EnemyHp->SetObjectState(CGameObject::OBJECT_STATE::VISIBLE);
			m_pUI_EnemyHp_Back->SetObjectState(CGameObject::OBJECT_STATE::VISIBLE);

			float hpRatio = 1.f - hoverEnemy->GetScript<CWarriorScript>()->GetHpRatio();
			m_pUI_EnemyHp->MeshRender()->GetMaterial(0)->SetScalarParam(FLOAT_0, &hpRatio);
			bHitEnemy = true;
		}
		else
		{
			m_pUI_EnemyHp->SetObjectState(CGameObject::OBJECT_STATE::INVISIBLE);
			m_pUI_EnemyHp_Back->SetObjectState(CGameObject::OBJECT_STATE::INVISIBLE);
			bHitEnemy = false;
		}
	}
	else
	{
		m_pUI_EnemyHp->SetObjectState(CGameObject::OBJECT_STATE::INVISIBLE);
		m_pUI_EnemyHp_Back->SetObjectState(CGameObject::OBJECT_STATE::INVISIBLE);
		bHitEnemy = false;
	}
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

	--iAmmo;

	tRayInfo rayInfo{};
	rayInfo.fDamage = 10.f;
	rayInfo.iLayerIdx = pPlayer->GetLayerIndex();
	rayInfo.vDir = MainCam->Transform()->GetRelativeDir(DIR_TYPE::FRONT);
	rayInfo.vStart = MainCam->Transform()->GetRelativePos();
	rayInfo.matWorld = MainCam->Transform()->GetDrawRayMat();
	rayInfo.tRayType = (UINT)RAYCAST_TYPE::SHOOT;
	//RaycastMgr::GetInst()->SetPlayerRayInfo(rayInfo);
	Vec3 vPosition;
	RaycastMgr::GetInst()->DoOneHitRaycast(rayInfo, &vPosition, RAYCAST_GROUP_TYPE::Player);
	Vec3 vPos = WeaponMgr::GetInst()->GetCurWeaponMuzzlePos();
	Vec3 vRot = WeaponMgr::GetInst()->GetCurWeapon()->Transform()->GetRelativeRot();

	WeaponMgr::GetInst()->MuzzleFlash(vPos, vRot);

	WeaponMgr::GetInst()->Play(GUN_ANIMATION_TYPE::FIRE, false);

	Recoil();

	if (!bHitEnemy)
		ParticleMgr::GetInst()->DoParticle(ParticleMgr::PARTICLE_SETTING_TYPE::BULLET_IMPACT, rayInfo.vStart + (rayInfo.vDir * vPosition.z));

	CGameObject* pGun = WeaponMgr::GetInst()->GetCurWeapon();
	int weaponIdx = WeaponMgr::GetInst()->GetCurWeaponPlayerAnim(PLAYER_ANIMATION_TYPE::FIRE);

	//if (weaponIdx == (int)PLAYER_ANIMATION_TYPE::SNIPER_FIRE)
	//{
	//	tState = PlayerMgr::PLAYER_STATE::FIRE;
	//	pPlayer->Animator3D()->Play(weaponIdx, false);
	//	AnimationMgr::GetInst()->AnimationSync(pGun->Animator3D(), pPlayer->Animator3D());
	//}

}

void CPlayerScript::ShootMissile()
{
	//CCamera* MainCam = CRenderMgr::GetInst()->GetMainCam();
	//if (nullptr == MainCam)
	//	MainCam = CameraMgr::GetInst()->GetCamObj(L"MainCamera")->Camera();

	//tRay Ray = MainCam->GetRay();
	//Vec3 vStart = Ray.vStart;

	//CGameObject* pBullet = new CGameObject;
	//pBullet->SetName(L"Player Missile");
	//pBullet->AddComponent(new CTransform);
	//pBullet->AddComponent(new CRigidBody(RIGID_BODY_SHAPE_TYPE::BOX));
	//pBullet->AddComponent(new CCollider3D(false));
	//pBullet->AddComponent(new CBulletScript);

	//SpawnGameObject(pBullet, vStart, LAYER_TYPE::PlayerBullet);
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

	if (WeaponMgr::GetInst()->ChangeWeapon(_Idx))
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

	CGameObject* pPlayer = GetOwner();

	Vec3 vPlayerPos = pPlayer->Transform()->GetRelativePos();
	Vec3 vPlayerCamPos = PlayerMgr::GetInst()->GetPlayerCameraPos();
	Vec3 vCamRot = pCamObj->Transform()->GetRelativeRot();

	Vec3 vPlayerFront = pPlayer->Transform()->GetRelativeDir(DIR_TYPE::FRONT);
	Vec3 vPlayerUp = pPlayer->Transform()->GetRelativeDir(DIR_TYPE::UP);
	Vec3 vPlayerRight = pPlayer->Transform()->GetRelativeDir(DIR_TYPE::RIGHT);

	CRigidBody* pPlayerRB = pPlayer->RigidBody();

	Vec2 vMouseDir = CKeyMgr::GetInst()->GetMouseDir();

	vCamRot.y += (DT * vMouseDir.x * 0.2f);
	vCamRot.x -= (DT * vMouseDir.y * 0.2f);
	vCamRot.z = 0;

	pPlayer->Transform()->SetRelativeRot(vCamRot);
	pCamObj->Transform()->SetRelativeRot(vCamRot);

	pCamObj->Transform()->SetRelativePos(vPlayerCamPos);

	float _fSpeed = fSpeed;
	Vec3 final_velocity = Vec3(0.0f, 0.0f, 0.0f);

	UINT uiFront = (1 << 0);
	UINT uiBack = (1 << 0);
	UINT uiRight = (1 << 1);
	UINT uiLeft = (1 << 1);
	UINT uiIdle = (1 << 2);
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
		final_velocity += vPlayerFront * _fSpeed;
	}
	if (KEY_PRESSED(KEY::S))
	{
		flag |= uiBack;
		final_velocity += vPlayerFront * -_fSpeed;
	}
	if (KEY_PRESSED(KEY::A))
	{
		flag |= uiLeft;
		final_velocity += vPlayerRight * -_fSpeed;
	}

	if (KEY_PRESSED(KEY::D))
	{
		flag |= uiRight;
		final_velocity += vPlayerRight * _fSpeed;
	}
	if (KEY_PRESSED(KEY::SPACE))
	{
		if (!bJump)
		{
			bJump = true;
			final_velocity += vPlayerUp * fJump;
		}

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
	//PxTransform rbPos = pPlayerRB->GetRigidBodyPos();
	//rbPos.p = PxVec3(vPlayerPos.x, vPlayerPos.y, vPlayerPos.z);
	//pPlayerRB->GetDynamicBody()->setKinematicTarget(rbPos);

	pPlayerRB->SetLinearVelocity(final_velocity);

	if (tState != PlayerMgr::PLAYER_STATE::RELOAD && tState != PlayerMgr::PLAYER_STATE::DRAW && tState != PlayerMgr::PLAYER_STATE::FIRE)
	{
		if (flag & uiFront)
		{
			pPlayer->Animator3D()->Play(WeaponMgr::GetInst()->GetCurWeaponPlayerAnim(PLAYER_ANIMATION_TYPE::WALK_FORWARD), true);
		}
		//else if (flag & uiBack)
		//{
		//	pPlayerObj->Animator3D()->Play(WeaponMgr::GetInst()->GetCurWeaponPlayerAnim(PLAYER_ANIMATION_TYPE::WALK_BACK), true);
		//}
		else if (flag & uiRight)
		{
			pPlayer->Animator3D()->Play(WeaponMgr::GetInst()->GetCurWeaponPlayerAnim(PLAYER_ANIMATION_TYPE::WALK_RIGHT), true);
		}
		else if (flag & uiLeft)
		{
			pPlayer->Animator3D()->Play(WeaponMgr::GetInst()->GetCurWeaponPlayerAnim(PLAYER_ANIMATION_TYPE::WALK_LEFT), true);
		}
		else if (flag & uiIdle)
		{
			pPlayer->Animator3D()->Play(WeaponMgr::GetInst()->GetCurWeaponPlayerAnim(PLAYER_ANIMATION_TYPE::IDLE), true);
		}
	}
}

void CPlayerScript::Recoil()
{
	CGameObject* pCamObj = GetOwner()->GetFollowObj();
	if (nullptr == pCamObj)
		pCamObj = CameraMgr::GetInst()->GetCamObj(L"MainCamera");

	CGameObject* pPlayer = GetOwner();
	Vec3 vRot = pPlayer->Transform()->GetRelativeRot();
	vRot.x -= 0.01f;

	pPlayer->Transform()->SetRelativeRot(vRot);
	pCamObj->Transform()->SetRelativeRot(vRot);
}

void CPlayerScript::Attacked(int _Damage)
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
			if (rayInfo.tRayType == (UINT)RAYCAST_TYPE::SHOOT)
			{
				Attacked(rayInfo.fDamage);
				RaycastMgr::GetInst()->AddRaycastDraw(vFront, vStart, rayInfo.matWorld, Vec4(1.f, 1.f, 0.f, 1.f));
			}
			else
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

void CPlayerScript::Burn()
{
	if (bBurn)
	{
		fBurnTickAcc += DT;
		fBurnAcc += DT;
		if (fBurnTickAcc > 1.f)
		{
			Attacked(10.f);
			fBurnTickAcc = 0.f;
		}
		if (fBurnAcc >= fBurnTime)
		{
			fBurnTickAcc = 0.0f;
			fBurnAcc = 0.0f;
			fBurnTime = 0.0f;
			bBurn = false;
		}
	}
}

void CPlayerScript::makeCollider()
{
	{
		CGameObject* pObj = new CGameObject;
		pObj->AddComponent(new CCollider3D(false, true));
		pObj->Collider3D()->SetScale(Vec3(200.f, 200.f, 200.f));

		pObj->SetName(L"fbx player head collider");

		SpawnGameObject(pObj, Vec3(0.f, 100.f, 0.f), LAYER_TYPE::NoRaycastingCollider, true);
		pHeadCollider = pObj;
	}
}

void CPlayerScript::BeginOverlap(CCollider3D* _Other)
{
	OutputDebugStringW(L"CPlayerScript BeginOverlap\n");
	bJump = false;
}

void CPlayerScript::OnOverlap(CCollider3D* _Other)
{
	OutputDebugStringW(L"CPlayerScript OnOverlap\n");
	bJump = false;
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