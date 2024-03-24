#include "pch.h"
#include "CPlayerScript.h"

#include <Engine/CEngine.h>
#include <Engine/PlayerMgr.h>
#include <Engine/TextMgr.h>
#include <Engine\CMeshRender.h>
#include <Engine\CMaterial.h>
#include <Engine\CRenderMgr.h>
#include <Engine\CameraMgr.h>
#include <Engine\RaycastMgr.h>
#include <Engine\WeaponMgr.h>
#include <Engine\AnimationMgr.h>
#include <Engine\ParticleMgr.h>
#include <Engine\CUI.h>
#include <Engine\CSimpleTextMgr.h>
#include <Engine\FieldUIMgr.h>

#include "CBulletScript.h"
#include "CEnemyScript.h"
#include "CMissileScript.h"
#include "CWarriorScript.h"


CPlayerScript::CPlayerScript()
	: CScript((UINT)SCRIPT_TYPE::PLAYER_SCRIPT)
	, fSpeed(500.f)
	, fJump(300.f)
	, fRateOfFire(0.05f)
	, fRateOfFireAcc(0.0f)
	, fMouseAcces(1.f)
	, iPlayerHp(50000.f)
	, iPlayerHpCapa(50000.f)
	, iAmmo(30)
	, iExp(0)
	, iExpMax(10)
	, iLevel(1)
	, iAmmoCapa(30)
	, iAmmoRemain(9999)
	, fBurnTime(0.0f)
	, bJump(false)
	, fHpFrontAcc(0.f)
	, fHpBackAcc(0.f)
	, fHpLeftAcc(0.f)
	, fHpRightAcc(0.f)
	, playerWalkIdx(0)
	, fWalkSoundAcc(0)
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

	pPlayer->Animator3D()->SetAnimClipEventIdx(
		(UINT)PLAYER_ANIMATION_TYPE::SNIPER_RELOAD, -1, -1, 103, 130);


	for (int i = 0; i < 3; ++i)
	{
		pPlayer->Animator3D()->EndEvent((UINT)PLAYER_ANIMATION_TYPE::RELOAD + i)
			= std::make_shared<std::function<void()>>([=]()
				{
					tState = PlayerMgr::PLAYER_STATE::IDLE;
					if (iAmmoRemain < iAmmoCapa)
					{
						iAmmo = iAmmoRemain;
						iAmmoRemain = 0;
					}
					else
					{
						iAmmoRemain -= (iAmmoCapa - iAmmo);
						iAmmo = iAmmoCapa;
					}
				});
		pPlayer->Animator3D()->StartEvent((UINT)PLAYER_ANIMATION_TYPE::DRAW + i)
			= std::make_shared<std::function<void()>>([=]()
				{
					if ((UINT)PLAYER_ANIMATION_TYPE::DRAW + i == (UINT)PLAYER_ANIMATION_TYPE::DRAW)
					{
						SoundMgr::GetInst()->Play(L"sound\\weapon\\smg_draw.ogg", pPlayer->Transform()->GetRelativePos(), 0, 10.f, SoundMgr::SOUND_TYPE::SFX);
					}
					if ((UINT)PLAYER_ANIMATION_TYPE::DRAW + i == (UINT)PLAYER_ANIMATION_TYPE::PISTOL_DRAW)
					{
						SoundMgr::GetInst()->Play(L"sound\\weapon\\pistol_draw.ogg", pPlayer->Transform()->GetRelativePos(), 0, 10.f, SoundMgr::SOUND_TYPE::SFX);
					}
					if ((UINT)PLAYER_ANIMATION_TYPE::DRAW + i == (UINT)PLAYER_ANIMATION_TYPE::SNIPER_DRAW)
					{
						SoundMgr::GetInst()->Play(L"sound\\weapon\\sniper_draw.ogg", pPlayer->Transform()->GetRelativePos(), 0, 10.f, SoundMgr::SOUND_TYPE::SFX);
					}

					tState = PlayerMgr::PLAYER_STATE::DRAW;
				});
		pPlayer->Animator3D()->EndEvent((UINT)PLAYER_ANIMATION_TYPE::DRAW + i)
			= std::make_shared<std::function<void()>>([=]()
				{
					tState = PlayerMgr::PLAYER_STATE::IDLE;
				});
	}
	//pPlayer->Animator3D()->EndEvent((UINT)PLAYER_ANIMATION_TYPE::SNIPER_FIRE)
	//	= std::make_shared<std::function<void()>>([=]()
	//		{
	//			tState = PlayerMgr::PLAYER_STATE::IDLE;
	//		});

	//makeCollider();

	// Test code
	//pPlayer->Animator3D()->EndEvent((UINT)PLAYER_ANIMATION_TYPE::RELOAD)
	//	= std::make_shared<std::function<void()>>([=]()
	//		{
	//			tState = PlayerMgr::PLAYER_STATE::IDLE;
	//			if(iAmmoRemain < iAmmoCapa)
	//			{
	//				iAmmo = iAmmoRemain;
	//				iAmmoRemain = 0;
	//			}else
	//			{
	//				iAmmoRemain -= (iAmmoCapa - iAmmo);
	//				iAmmo = iAmmoCapa;
	//			}
	//		});
	//pPlayer->Animator3D()->EndEvent((UINT)PLAYER_ANIMATION_TYPE::DRAW)
	//	= std::make_shared<std::function<void()>>([=]()
	//		{
	//			tState = PlayerMgr::PLAYER_STATE::IDLE;
	//		});
}

void CPlayerScript::tick()
{
	Look();
	Movement();
	Burn();

	if (m_pAmmoText)
		m_pAmmoText->SetText(std::to_wstring(iAmmo) + L" / " + std::to_wstring(iAmmoRemain));

	if (m_pHPText)
		m_pHPText->SetText(std::to_wstring(iPlayerHp));

	if (m_pLevelText)
		m_pLevelText->SetText(L"LV " + std::to_wstring(iLevel) + L" Soldier");

	bool reverse = true;
	float ratio = 1.f - (float)iAmmo / (float)iAmmoCapa;
	m_pUI_AMMO->MeshRender()->GetMaterial(0)->SetScalarParam(FLOAT_0, &ratio);
	m_pUI_AMMO->MeshRender()->GetMaterial(0)->SetScalarParam(INT_0, &reverse);

	//reverse = false;
	ratio = 1.f - (float)iPlayerHp / (float)iPlayerHpCapa;
	m_pUI_HP->MeshRender()->GetMaterial(0)->SetScalarParam(FLOAT_0, &ratio);
	//m_pUI_HP->MeshRender()->GetMaterial(0)->SetScalarParam(INT_0, &reverse);

	if (iExp >= iExpMax)
		iExp = 0;

	ratio = 1.f - (float)iExp / (float)iExpMax;
	m_pUI_EXP->MeshRender()->GetMaterial(0)->SetScalarParam(FLOAT_0, &ratio);

	if (m_pUI_HPFrontHit->GetObjectState() == CGameObject::OBJECT_STATE::VISIBLE)
	{
		fHpFrontAcc += DT;

		float time = 0.5f;
		bool alpha = true;

		m_pUI_HPFrontHit->MeshRender()->GetMaterial(0)->SetScalarParam(INT_1, &alpha);
		m_pUI_HPFrontHit->MeshRender()->GetMaterial(0)->SetScalarParam(FLOAT_1, &time);

		if (fHpFrontAcc > 3.f && fHpFrontAcc - 3.f < 2.f)
		{
			time = 0.5f - (((fHpFrontAcc - 3.f) / 2.f) * 0.5);
			m_pUI_HPFrontHit->MeshRender()->GetMaterial(0)->SetScalarParam(INT_1, &alpha);
			m_pUI_HPFrontHit->MeshRender()->GetMaterial(0)->SetScalarParam(FLOAT_1, &time);
		}
		if (fHpFrontAcc > 5.f)
		{
			fHpFrontAcc = 0.0f;
			m_pUI_HPFrontHit->SetObjectState(CGameObject::OBJECT_STATE::INVISIBLE);
		}
	}
	if (m_pUI_HPBackHit->GetObjectState() == CGameObject::OBJECT_STATE::VISIBLE)
	{
		fHpBackAcc += DT;

		float time = 0.5f;
		bool alpha = true;

		m_pUI_HPBackHit->MeshRender()->GetMaterial(0)->SetScalarParam(INT_1, &alpha);
		m_pUI_HPBackHit->MeshRender()->GetMaterial(0)->SetScalarParam(FLOAT_1, &time);

		if (fHpBackAcc > 3.f && fHpBackAcc - 3.f < 2.f)
		{
			time = 0.5f - (((fHpBackAcc - 3.f) / 2.f) * 0.5);
			m_pUI_HPBackHit->MeshRender()->GetMaterial(0)->SetScalarParam(INT_1, &alpha);
			m_pUI_HPBackHit->MeshRender()->GetMaterial(0)->SetScalarParam(FLOAT_1, &time);
		}
		if (fHpBackAcc > 5.f)
		{
			fHpBackAcc = 0.0f;
			m_pUI_HPBackHit->SetObjectState(CGameObject::OBJECT_STATE::INVISIBLE);
		}
	}
	if (m_pUI_HPRightHit->GetObjectState() == CGameObject::OBJECT_STATE::VISIBLE)
	{
		fHpRightAcc += DT;

		float time = 0.5f;
		bool alpha = true;

		m_pUI_HPRightHit->MeshRender()->GetMaterial(0)->SetScalarParam(INT_1, &alpha);
		m_pUI_HPRightHit->MeshRender()->GetMaterial(0)->SetScalarParam(FLOAT_1, &time);

		if (fHpRightAcc > 3.f && fHpRightAcc - 3.f < 2.f)
		{
			time = 0.5f - (((fHpRightAcc - 3.f) / 2.f) * 0.5);
			m_pUI_HPRightHit->MeshRender()->GetMaterial(0)->SetScalarParam(INT_1, &alpha);
			m_pUI_HPRightHit->MeshRender()->GetMaterial(0)->SetScalarParam(FLOAT_1, &time);
		}
		if (fHpRightAcc > 5.f)
		{
			fHpRightAcc = 0.0f;
			m_pUI_HPRightHit->SetObjectState(CGameObject::OBJECT_STATE::INVISIBLE);
		}
	}
	if (m_pUI_HPLeftHit->GetObjectState() == CGameObject::OBJECT_STATE::VISIBLE)
	{
		fHpLeftAcc += DT;

		float time = 0.5f;
		bool alpha = true;

		m_pUI_HPLeftHit->MeshRender()->GetMaterial(0)->SetScalarParam(INT_1, &alpha);
		m_pUI_HPLeftHit->MeshRender()->GetMaterial(0)->SetScalarParam(FLOAT_1, &time);

		if (fHpLeftAcc > 3.f && fHpLeftAcc - 3.f < 2.f)
		{
			time = 0.5f - (((fHpLeftAcc - 3.f) / 2.f) * 0.5);
			m_pUI_HPLeftHit->MeshRender()->GetMaterial(0)->SetScalarParam(INT_1, &alpha);
			m_pUI_HPLeftHit->MeshRender()->GetMaterial(0)->SetScalarParam(FLOAT_1, &time);
		}
		if (fHpLeftAcc > 5.f)
		{
			fHpLeftAcc = 0.0f;
			m_pUI_HPLeftHit->SetObjectState(CGameObject::OBJECT_STATE::INVISIBLE);
		}
	}

	CustomReloadSound();
}

void CPlayerScript::finaltick()
{
	static bool init = false;
	if (!init)
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

	{
		pMtrl = new CMaterial(true);
		pMtrl->SetShader(CResMgr::GetInst()->FindRes<CGraphicsShader>(L"UI2DShader"));
		CResMgr::GetInst()->AddRes(L"Character Damage Fire Mtrl", pMtrl);

		bool flatFire = true;
		pMtrl->SetScalarParam(INT_0, &flatFire);

		m_pUI_FireHitEffect = new CGameObject;

		m_pUI_FireHitEffect->SetName(L"UI Damage Fire");
		m_pUI_FireHitEffect->AddComponent(new CTransform);
		m_pUI_FireHitEffect->AddComponent(new CMeshRender);

		m_pUI_FireHitEffect->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
		m_pUI_FireHitEffect->MeshRender()->SetMaterial(pMtrl, 0);

		m_pUI_FireHitEffect->Transform()->SetRelativeScale(Vec3(1280.f, 1400.f, 1.f));
		//m_pUI_FireHitEffect->Transform()->SetRelativeRot(Vec3(0.f, 0.f, 90.f * DegToRad()));
		SpawnGameObject(m_pUI_FireHitEffect, Vec3(0.f, 0.f, 0.f), LAYER_TYPE::ViewPortUI);

		//m_pUI_FireHitEffect->SetObjectState(CGameObject::OBJECT_STATE::INVISIBLE);
	}
	{
		pMtrl = new CMaterial(true);
		pMtrl->SetShader(UIShader);
		CResMgr::GetInst()->AddRes(L"UIHPBackMtrl", pMtrl);

		pTex = CResMgr::GetInst()->FindRes<CTexture>(L"texture\\UI\\player_hp_back.png");

		m_pUI_EnemyHp_Back = new CGameObject;
		m_pUI_EnemyHp_Back->SetName(L"UI HP BACK");
		m_pUI_EnemyHp_Back->AddComponent(new CTransform);
		m_pUI_EnemyHp_Back->AddComponent(new CMeshRender);

		m_pUI_EnemyHp_Back->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
		m_pUI_EnemyHp_Back->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"UIHPBackMtrl"), 0);
		m_pUI_EnemyHp_Back->MeshRender()->GetMaterial(0)->SetTexParam(TEX_0, pTex);
		m_pUI_EnemyHp_Back->Transform()->SetRelativeScale(Vec3(238.f, 38.f, 1.f));
		m_pUI_EnemyHp_Back->Transform()->SetRelativeRot(Vec3(0.f, 0.f, 3.f * DegToRad()));

		SpawnGameObject(m_pUI_EnemyHp_Back, Vec3(-497.f, -331.f, 0.f), LAYER_TYPE::ViewPortUI);
	}
	{
		pMtrl = new CMaterial(true);
		pMtrl->SetShader(adjustUIShader);
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
		m_pUI_HP->Transform()->SetRelativeRot(Vec3(0.f, 0.f, 3.f * DegToRad()));

		SpawnGameObject(m_pUI_HP, Vec3(-475.f, -323.f, 0.f), LAYER_TYPE::ViewPortUI);
	}
	{
		pMtrl = new CMaterial(true);
		pMtrl->SetShader(UIShader);
		CResMgr::GetInst()->AddRes(L"UIAMMOBackMtrl", pMtrl);

		pTex = CResMgr::GetInst()->FindRes<CTexture>(L"texture\\UI\\player_ammo_back.png");
		m_pUI_AMMO_Back = new CGameObject;

		m_pUI_AMMO_Back->SetName(L"UI AMMO BACK");
		m_pUI_AMMO_Back->AddComponent(new CTransform);
		m_pUI_AMMO_Back->AddComponent(new CMeshRender);

		m_pUI_AMMO_Back->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
		m_pUI_AMMO_Back->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"UIAMMOBackMtrl"), 0);
		m_pUI_AMMO_Back->MeshRender()->GetMaterial(0)->SetTexParam(TEX_0, pTex);
		m_pUI_AMMO_Back->Transform()->SetRelativeScale(Vec3(220.f, 38.f, 1.f));
		m_pUI_AMMO_Back->Transform()->SetRelativeRot(Vec3(0.f, 0.f, -3.f * DegToRad()));

		SpawnGameObject(m_pUI_AMMO_Back, Vec3(479.f, -327.f, 0.f), LAYER_TYPE::ViewPortUI);
	}
	{
		pMtrl = new CMaterial(true);
		pMtrl->SetShader(UIShader);
		CResMgr::GetInst()->AddRes(L"UIAMMOIconMtrl", pMtrl);

		pTex = CResMgr::GetInst()->FindRes<CTexture>(L"texture\\UI\\smg_ammo.png");

		m_pUI_AmmoIcon = new CGameObject;

		m_pUI_AmmoIcon->SetName(L"UI AMMO ICON");
		m_pUI_AmmoIcon->AddComponent(new CTransform);
		m_pUI_AmmoIcon->AddComponent(new CMeshRender);

		m_pUI_AmmoIcon->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
		m_pUI_AmmoIcon->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"UIAMMOIconMtrl"), 0);
		m_pUI_AmmoIcon->MeshRender()->GetMaterial(0)->SetTexParam(TEX_0, pTex);
		m_pUI_AmmoIcon->Transform()->SetRelativeScale(Vec3(32.f, 49.f, 1.f));
		m_pUI_AmmoIcon->Transform()->SetRelativeRot(Vec3(0.f, 0.f, -3.f * DegToRad()));
		SpawnGameObject(m_pUI_AmmoIcon, Vec3(598.f, -333.f, 0.f), LAYER_TYPE::ViewPortUI);
	}
	{
		pMtrl = new CMaterial(true);
		pMtrl->SetShader(adjustUIShader);
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
		m_pUI_AMMO->Transform()->SetRelativeRot(Vec3(0.f, 0.f, -3.f * DegToRad()));
		SpawnGameObject(m_pUI_AMMO, Vec3(466.f, -319.f, 0.f), LAYER_TYPE::ViewPortUI);
	}

	{
		pMtrl = new CMaterial(true);
		pMtrl->SetShader(UIShader);
		CResMgr::GetInst()->AddRes(L"CrossHairMtrl", pMtrl);
		m_pUI_CrossHair = new CGameObject;
		m_pUI_CrossHair->SetName(L"UI Cross Hair");
		m_pUI_CrossHair->AddComponent(new CTransform);
		m_pUI_CrossHair->AddComponent(new CMeshRender);

		m_pUI_CrossHair->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
		m_pUI_CrossHair->MeshRender()->SetMaterial(pMtrl, 0);
		m_pUI_CrossHair->MeshRender()->GetMaterial(0)->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"texture\\UI\\Crosshair.png"));
		m_pUI_CrossHair->Transform()->SetRelativeScale(Vec3(80.f, 80.f, 1.f));
		SpawnGameObject(m_pUI_CrossHair, Vec3(0.f, 0.f, 0.f), LAYER_TYPE::ViewPortUI);
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
	{
		pMtrl = new CMaterial(true);
		pMtrl->SetShader(UIShader);
		CResMgr::GetInst()->AddRes(L"ExpBack", pMtrl);

		Ptr<CTexture> pTex = CResMgr::GetInst()->FindRes<CTexture>(L"texture\\UI\\exp_bar_back.png");
		m_pUI_ExpBar_Back = new CGameObject;

		m_pUI_ExpBar_Back->SetName(L"UI Exp Back");
		m_pUI_ExpBar_Back->AddComponent(new CTransform);
		m_pUI_ExpBar_Back->AddComponent(new CMeshRender);

		m_pUI_ExpBar_Back->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
		m_pUI_ExpBar_Back->MeshRender()->SetMaterial(pMtrl, 0);
		m_pUI_ExpBar_Back->MeshRender()->GetMaterial(0)->SetTexParam(TEX_0, pTex);
		m_pUI_ExpBar_Back->Transform()->SetRelativeScale(Vec3(pTex->Width(), pTex->Height(), 1.f));
		SpawnGameObject(m_pUI_ExpBar_Back, Vec3(0.f, -324.f, 0.f), LAYER_TYPE::ViewPortUI);
	}
	{
		pMtrl = new CMaterial(true);
		pMtrl->SetShader(adjustUIShader);
		CResMgr::GetInst()->AddRes(L"ExpFront", pMtrl);

		Ptr<CTexture> pTex = CResMgr::GetInst()->FindRes<CTexture>(L"texture\\UI\\exp_bar_front.png");
		m_pUI_EXP = new CGameObject;

		m_pUI_EXP->SetName(L"UI Exp Front");
		m_pUI_EXP->AddComponent(new CTransform);
		m_pUI_EXP->AddComponent(new CMeshRender);

		m_pUI_EXP->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
		m_pUI_EXP->MeshRender()->SetMaterial(pMtrl, 0);
		m_pUI_EXP->MeshRender()->GetMaterial(0)->SetTexParam(TEX_0, pTex);
		m_pUI_EXP->Transform()->SetRelativeScale(Vec3(pTex->Width(), pTex->Height(), 1.f));
		SpawnGameObject(m_pUI_EXP, Vec3(0.f, -317.f, 0.f), LAYER_TYPE::ViewPortUI);
	}
	{

		pMtrl = new CMaterial(true);
		pMtrl->SetShader(adjustUIShader);
		CResMgr::GetInst()->AddRes(L"DamageHp", pMtrl);

		Ptr<CTexture> pTex = CResMgr::GetInst()->FindRes<CTexture>(L"texture\\UI\\damaged_hp.png");
		m_pUI_HPBackHit = new CGameObject;

		m_pUI_HPBackHit->SetName(L"UI Damage HP");
		m_pUI_HPBackHit->AddComponent(new CTransform);
		m_pUI_HPBackHit->AddComponent(new CMeshRender);

		m_pUI_HPBackHit->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
		m_pUI_HPBackHit->MeshRender()->SetMaterial(pMtrl, 0);
		m_pUI_HPBackHit->MeshRender()->GetMaterial(0)->SetTexParam(TEX_0, pTex);
		m_pUI_HPBackHit->Transform()->SetRelativeScale(Vec3(pTex->Width(), pTex->Height(), 1.f));
		SpawnGameObject(m_pUI_HPBackHit, Vec3(0.f, -317.f, 0.f), LAYER_TYPE::ViewPortUI);

		m_pUI_HPBackHit->SetObjectState(CGameObject::OBJECT_STATE::INVISIBLE);
	}
	{

		pMtrl = new CMaterial(true);
		pMtrl->SetShader(adjustUIShader);
		CResMgr::GetInst()->AddRes(L"FrontDamageHp", pMtrl);

		Ptr<CTexture> pTex = CResMgr::GetInst()->FindRes<CTexture>(L"texture\\UI\\damaged_hp.png");
		m_pUI_HPBackHit = new CGameObject;

		m_pUI_HPBackHit->SetName(L"UI Back Damage HP");
		m_pUI_HPBackHit->AddComponent(new CTransform);
		m_pUI_HPBackHit->AddComponent(new CMeshRender);

		m_pUI_HPBackHit->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
		m_pUI_HPBackHit->MeshRender()->SetMaterial(pMtrl, 0);
		m_pUI_HPBackHit->MeshRender()->GetMaterial(0)->SetTexParam(TEX_0, pTex);
		m_pUI_HPBackHit->Transform()->SetRelativeScale(Vec3(pTex->Width(), pTex->Height(), 1.f));
		SpawnGameObject(m_pUI_HPBackHit, Vec3(0.f, -317.f, 0.f), LAYER_TYPE::ViewPortUI);

		m_pUI_HPBackHit->SetObjectState(CGameObject::OBJECT_STATE::INVISIBLE);
	}
	{

		pMtrl = new CMaterial(true);
		pMtrl->SetShader(adjustUIShader);
		CResMgr::GetInst()->AddRes(L"BackDamageHp", pMtrl);

		Ptr<CTexture> pTex = CResMgr::GetInst()->FindRes<CTexture>(L"texture\\UI\\damaged_hp.png");
		m_pUI_HPFrontHit = new CGameObject;

		m_pUI_HPFrontHit->SetName(L"UI Back Damage HP");
		m_pUI_HPFrontHit->AddComponent(new CTransform);
		m_pUI_HPFrontHit->AddComponent(new CMeshRender);

		m_pUI_HPFrontHit->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
		m_pUI_HPFrontHit->MeshRender()->SetMaterial(pMtrl, 0);
		m_pUI_HPFrontHit->MeshRender()->GetMaterial(0)->SetTexParam(TEX_0, pTex);
		m_pUI_HPFrontHit->Transform()->SetRelativeScale(Vec3(pTex->Width(), pTex->Height(), 1.f));
		m_pUI_HPFrontHit->Transform()->SetRelativeRot(Vec3(0.f, 0.f, 180.f * DegToRad()));
		SpawnGameObject(m_pUI_HPFrontHit, Vec3(0.f, 317.f, 0.f), LAYER_TYPE::ViewPortUI);

		m_pUI_HPFrontHit->SetObjectState(CGameObject::OBJECT_STATE::INVISIBLE);
	}
	{
		pMtrl = new CMaterial(true);
		pMtrl->SetShader(adjustUIShader);
		CResMgr::GetInst()->AddRes(L"LeftDamageHp", pMtrl);

		Ptr<CTexture> pTex = CResMgr::GetInst()->FindRes<CTexture>(L"texture\\UI\\damaged_hp.png");
		m_pUI_HPLeftHit = new CGameObject;

		m_pUI_HPLeftHit->SetName(L"UI Left Damage HP");
		m_pUI_HPLeftHit->AddComponent(new CTransform);
		m_pUI_HPLeftHit->AddComponent(new CMeshRender);

		m_pUI_HPLeftHit->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
		m_pUI_HPLeftHit->MeshRender()->SetMaterial(pMtrl, 0);
		m_pUI_HPLeftHit->MeshRender()->GetMaterial(0)->SetTexParam(TEX_0, pTex);
		m_pUI_HPLeftHit->Transform()->SetRelativeScale(Vec3(pTex->Width(), pTex->Height(), 1.f));
		m_pUI_HPLeftHit->Transform()->SetRelativeRot(Vec3(0.f, 0.f, -90.f * DegToRad()));
		SpawnGameObject(m_pUI_HPLeftHit, Vec3(-580.f, 0.f, 0.f), LAYER_TYPE::ViewPortUI);

		m_pUI_HPLeftHit->SetObjectState(CGameObject::OBJECT_STATE::INVISIBLE);
	}
	{
		pMtrl = new CMaterial(true);
		pMtrl->SetShader(adjustUIShader);
		CResMgr::GetInst()->AddRes(L"LeftDamageHp", pMtrl);

		Ptr<CTexture> pTex = CResMgr::GetInst()->FindRes<CTexture>(L"texture\\UI\\damaged_hp.png");
		m_pUI_HPRightHit = new CGameObject;

		m_pUI_HPRightHit->SetName(L"UI Left Damage HP");
		m_pUI_HPRightHit->AddComponent(new CTransform);
		m_pUI_HPRightHit->AddComponent(new CMeshRender);

		m_pUI_HPRightHit->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
		m_pUI_HPRightHit->MeshRender()->SetMaterial(pMtrl, 0);
		m_pUI_HPRightHit->MeshRender()->GetMaterial(0)->SetTexParam(TEX_0, pTex);
		m_pUI_HPRightHit->Transform()->SetRelativeScale(Vec3(pTex->Width(), pTex->Height(), 1.f));
		m_pUI_HPRightHit->Transform()->SetRelativeRot(Vec3(0.f, 0.f, 90.f * DegToRad()));
		SpawnGameObject(m_pUI_HPRightHit, Vec3(580.f, 0.f, 0.f), LAYER_TYPE::ViewPortUI);

		m_pUI_HPRightHit->SetObjectState(CGameObject::OBJECT_STATE::INVISIBLE);
	}


	m_pAmmoText = new CUI();
	m_pAmmoText->SetName(L"Ammo Text");
	m_pAmmoText->AddComponent(new CTransform);
	//m_pAmmoText->AddComponent(new CMeshRender);
	//m_pAmmoText->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	//m_pAmmoText->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"AdjustUI2DShaderMtrl"), 0);
	m_pAmmoText->Transform()->SetRelativeScale(Vec3(100.f, 100.f, 1.f));

	//m_pAmmoText->SetTextSize(Vec2(10.f, 10.f));
	m_pAmmoText->SetTextNormalColor(Vec4(1.f, 1.f, 1.f, 1.f));
	m_pAmmoText->SetText(std::to_wstring(iAmmo) + L" / " + std::to_wstring(iAmmoRemain));
	m_pAmmoText->Transform()->SetRelativeRot(Vec3(0.f, 0.f, 3 * DegToRad()));
	m_pAmmoText->SetTextScale(0.75f);
	m_pAmmoText->SetOutline(true);
	SpawnGameObject(m_pAmmoText, Vec3(544.f, -347.f, 1.f), LAYER_TYPE::ViewPortUI);

	m_pHPText = new CUI();
	m_pHPText->SetName(L"HP Text");
	m_pHPText->AddComponent(new CTransform);
	//m_pHPText->AddComponent(new CMeshRender);
	//m_pHPText->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	//m_pHPText->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"AdjustUI2DShaderMtrl"), 0);
	m_pHPText->Transform()->SetRelativeScale(Vec3(100.f, 100.f, 1.f));

	//m_pHPText->SetTextSize(Vec2(20.f, 20.f));
	m_pHPText->SetTextNormalColor(Vec4(1.f, 1.f, 1.f, 1.f));
	m_pHPText->SetText(std::to_wstring(iPlayerHp));
	m_pHPText->SetTextScale(0.75f);
	m_pHPText->SetOutline(true);
	m_pHPText->Transform()->SetRelativeRot(Vec3(0.f, 0.f, -3 * DegToRad()));
	SpawnGameObject(m_pHPText, Vec3(-501.f, -358.f, 1.f), LAYER_TYPE::ViewPortUI);

	m_pLevelText = new CUI();
	m_pLevelText->SetName(L"Level Text");
	m_pLevelText->AddComponent(new CTransform);
	//m_pLevelText->AddComponent(new CMeshRender);
	//m_pLevelText->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	//m_pLevelText->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"AdjustUI2DShaderMtrl"), 0);
	m_pLevelText->Transform()->SetRelativeScale(Vec3(100.f, 100.f, 1.f));
	m_pLevelText->SetTextScale(0.75f);
	m_pLevelText->SetOutline(true);
	//m_pLevelText->SetTextSize(Vec2(5.f, 5.f));
	m_pLevelText->SetTextNormalColor(Vec4(1.f, 1.f, 1.f, 1.f));
	m_pLevelText->SetText(L"LV " + std::to_wstring(iLevel) + L" Soldier");
	SpawnGameObject(m_pLevelText, Vec3(-111.f, -359.f, 1.f), LAYER_TYPE::ViewPortUI);

	m_pEnemyLevel = new CUI();
	m_pEnemyLevel->SetName(L"Enemy Level Text");
	m_pEnemyLevel->AddComponent(new CTransform);
	//m_pEnemyLevel->AddComponent(new CMeshRender);
	//m_pEnemyLevel->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	//m_pEnemyLevel->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"AdjustUI2DShaderMtrl"), 0);
	m_pEnemyLevel->Transform()->SetRelativeScale(Vec3(100.f, 100.f, 1.f));
	m_pEnemyLevel->SetTextScale(0.6f);
	m_pEnemyLevel->SetOutline(true);
	m_pEnemyLevel->SetTextNormalColor(Vec4(1.f, 1.f, 1.f, 1.f));
	SpawnGameObject(m_pEnemyLevel, Vec3(-56.f, 3.f, 1.f), LAYER_TYPE::ViewPortUI);

	m_pEnemyName = new CUI();
	m_pEnemyName->SetName(L"Enemy Name Text");
	m_pEnemyName->AddComponent(new CTransform);
	//m_pEnemyName->AddComponent(new CMeshRender);
	//m_pEnemyName->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	//m_pEnemyName->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"AdjustUI2DShaderMtrl"), 0);
	m_pEnemyName->Transform()->SetRelativeScale(Vec3(100.f, 100.f, 1.f));
	m_pEnemyName->SetTextScale(0.5f);
	m_pEnemyName->SetOutline(true);
	m_pEnemyName->SetTextNormalColor(Vec4(1.f, 1.f, 1.f, 1.f));
	SpawnGameObject(m_pEnemyName, Vec3(0.f, -10.f, 1.f), LAYER_TYPE::ViewPortUI);
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
			m_pEnemyName->SetText(hoverEnemy->GetScript<CEnemyScript>()->GetEnemyName());
			wstring str = L"01";
			m_pEnemyLevel->SetText(str);
			bHitEnemy = true;
			m_pUI_EnemyHp->SetObjectState(CGameObject::OBJECT_STATE::VISIBLE);
			m_pUI_EnemyHp_Back->SetObjectState(CGameObject::OBJECT_STATE::VISIBLE);
			m_pEnemyLevel->SetObjectState(CGameObject::OBJECT_STATE::VISIBLE);
			m_pEnemyName->SetObjectState(CGameObject::OBJECT_STATE::VISIBLE);

			Vec3 vPos = m_pEnemyName->Transform()->GetRelativePos();
			Vec2 vSize = m_pEnemyName->GetTextSize();
			vPos.x = (vSize.x / 2.f) - 61.f;
			m_pEnemyName->Transform()->SetRelativePos(vPos);
			hoverEnemy->GetName();

			float hpRatio = 1.f - hoverEnemy->GetScript<CEnemyScript>()->GetHpRatio();
			m_pUI_EnemyHp->MeshRender()->GetMaterial(0)->SetScalarParam(FLOAT_0, &hpRatio);



		}
		//else if (hoverEnemy->GetScript<CWarriorScript>())
		//{
		//	m_pUI_EnemyHp->SetObjectState(CGameObject::OBJECT_STATE::VISIBLE);
		//	m_pUI_EnemyHp_Back->SetObjectState(CGameObject::OBJECT_STATE::VISIBLE);

		//	float hpRatio = 1.f - hoverEnemy->GetScript<CWarriorScript>()->GetHpRatio();
		//	m_pUI_EnemyHp->MeshRender()->GetMaterial(0)->SetScalarParam(FLOAT_0, &hpRatio);
		//	bHitEnemy = true;
		//}
		else
		{
			m_pUI_EnemyHp->SetObjectState(CGameObject::OBJECT_STATE::INVISIBLE);
			m_pUI_EnemyHp_Back->SetObjectState(CGameObject::OBJECT_STATE::INVISIBLE);
			m_pEnemyLevel->SetObjectState(CGameObject::OBJECT_STATE::INVISIBLE);
			m_pEnemyName->SetObjectState(CGameObject::OBJECT_STATE::INVISIBLE);
			bHitEnemy = false;
		}
	}
	else
	{
		m_pUI_EnemyHp->SetObjectState(CGameObject::OBJECT_STATE::INVISIBLE);
		m_pUI_EnemyHp_Back->SetObjectState(CGameObject::OBJECT_STATE::INVISIBLE);
		m_pEnemyLevel->SetObjectState(CGameObject::OBJECT_STATE::INVISIBLE);
		m_pEnemyName->SetObjectState(CGameObject::OBJECT_STATE::INVISIBLE);
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

	WeaponMgr::GetInst()->MuzzleFlash(vPos, vRot, GetOwner());

	WeaponMgr::GetInst()->Play(GUN_ANIMATION_TYPE::FIRE, false);

	Recoil();

	if (!bHitEnemy && 0.0f < vPosition.z) // 충돌이 없을 때 distance가 0
		ParticleMgr::GetInst()->DoParticle(ParticleMgr::PARTICLE_SETTING_TYPE::BULLET_IMPACT, rayInfo.vStart + (rayInfo.vDir * vPosition.z));

	//if (bHitEnemy)
	//{
	//	FieldUIMgr::GetInst()->AddDamage(10.f, rayInfo.vStart + (rayInfo.vDir * vPosition.z));
	//}

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
	if (tState == PlayerMgr::PLAYER_STATE::RELOAD || tState == PlayerMgr::PLAYER_STATE::DRAW
		|| iAmmoRemain == 0)
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

	if (pCamObj->Camera()->IsCinematic())
		return;

	CGameObject* pPlayer = GetOwner();

	Vec3 vPlayerPos = pPlayer->Transform()->GetRelativePos();

	//if (nullptr == pPlayer->RigidBody())
	//{
	//	pPlayer->Transform()->SetRelativePos(vPlayerPos);
	//	return;
	//}

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
	Vec3 final_velocity = Vec3(0.00f, 0.00f, 0.00f);

	UINT uiFront = (1 << 0);
	UINT uiBack = (1 << 0);
	UINT uiRight = (1 << 1);
	UINT uiLeft = (1 << 1);
	UINT uiIdle = (1 << 2);
	UINT flag = uiIdle;

	bool bMove = false;

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
		bMove = true;
	}
	if (KEY_PRESSED(KEY::S))
	{
		flag |= uiBack;
		final_velocity += vPlayerFront * -_fSpeed;
		bMove = true;
	}
	if (KEY_PRESSED(KEY::A))
	{
		flag |= uiLeft;
		final_velocity += vPlayerRight * -_fSpeed;
		bMove = true;
	}

	if (KEY_PRESSED(KEY::D))
	{
		flag |= uiRight;
		final_velocity += vPlayerRight * _fSpeed;
		bMove = true;
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
		final_velocity = Vec3(0.00f, 0.00f, 0.00f);
		pPlayerRB->SetLinearVelocityZero();
	}
	//PxTransform rbPos = pPlayerRB->GetRigidBodyPos();
	//rbPos.p = PxVec3(vPlayerPos.x, vPlayerPos.y, vPlayerPos.z);
	//pPlayerRB->GetDynamicBody()->setKinematicTarget(rbPos);

	pPlayerRB->SetLinearVelocity(final_velocity);
	//if (final_velocity == Vec3::Zero)
	//{
	//	pPlayer->Transform()->SetRelativePosX(vPlayerPos.x);
	//	pPlayer->Transform()->SetRelativePosZ(vPlayerPos.z);
	//}
	//pPlayerRB->AddTorque(final_velocity);

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
		if (bMove)
		{
			fWalkSoundAcc += DT;
			if (fWalkSoundAcc > 0.5f)
			{
				++playerWalkIdx;
				playerWalkIdx %= 5;
				SoundMgr::GetInst()->Play(wsPlayerWalkSound[playerWalkIdx], vPlayerPos, 0, 10.f, SoundMgr::SOUND_TYPE::SFX, 0.5f);
				fWalkSoundAcc = 0.f;
			}
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
		m_pUI_FireHitEffect->SetObjectState(CGameObject::OBJECT_STATE::VISIBLE);
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
	else
	{
		m_pUI_FireHitEffect->SetObjectState(CGameObject::OBJECT_STATE::INVISIBLE);
	}
}

void CPlayerScript::AddExp(int _iExp)
{
	iExp += _iExp;
	if (iExp <= iExpMax)
	{
		iLevel = iExp % iExpMax;
		iExp = 0;
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
	CRigidBody* rb = GetOwner()->RigidBody();
	//rb->GetDynamicBody()->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, false);
}

void CPlayerScript::OnOverlap(CCollider3D* _Other)
{
	OutputDebugStringW(L"CPlayerScript OnOverlap\n");
	bJump = false;
	CRigidBody* rb = GetOwner()->RigidBody();
	//rb->GetDynamicBody()->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, false);
}

void CPlayerScript::EndOverlap(CCollider3D* _Other)
{
	CRigidBody* rb = GetOwner()->RigidBody();
	//rb->GetDynamicBody()->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
}

void CPlayerScript::Raycast(tRayInfo _RaycastInfo)
{
	if (iPlayerHp < _RaycastInfo.fDamage)
		iPlayerHp = 0;
	else
	{
		iPlayerHp -= _RaycastInfo.fDamage;
		Vec3 vRight = GetOwner()->Transform()->GetRelativeDir(DIR_TYPE::RIGHT);
		Vec3 vFront = GetOwner()->Transform()->GetRelativeDir(DIR_TYPE::FRONT);
		Vec3 vPos = GetOwner()->Transform()->GetRelativePos();
		float rightTheta = _RaycastInfo.vDir.Dot(vRight);

		rightTheta *= RadToDeg();

		if (rightTheta < 45.f && rightTheta > -45.f)
		{
			Vec3 vDiff = _RaycastInfo.vStart - vPos;
			vDiff.Normalize();

			if (vFront.Dot(vDiff) > 0.0f)
			{
				if (m_pUI_HPFrontHit)
					m_pUI_HPFrontHit->SetObjectState(CGameObject::OBJECT_STATE::VISIBLE);

				fHpFrontAcc = 0.0f;
			}
			else
			{
				if (m_pUI_HPBackHit)
					m_pUI_HPBackHit->SetObjectState(CGameObject::OBJECT_STATE::VISIBLE);

				fHpBackAcc = 0.0f;
			}
		}

		if (rightTheta < -45.f && rightTheta > -135.f)
		{
			if (m_pUI_HPRightHit)
				m_pUI_HPRightHit->SetObjectState(CGameObject::OBJECT_STATE::VISIBLE);
			fHpRightAcc = 0.0f;
		}
		if (rightTheta > 45.f && rightTheta < 135.f)
		{
			if (m_pUI_HPLeftHit)
				m_pUI_HPLeftHit->SetObjectState(CGameObject::OBJECT_STATE::VISIBLE);
			fHpLeftAcc = 0.0f;
		}
	}

}

void CPlayerScript::CustomReloadSound()
{
	if (tState != PlayerMgr::PLAYER_STATE::RELOAD)
		return;
	CGameObject* pPlayer = GetOwner();
	int weaponIdx = WeaponMgr::GetInst()->GetCurWeaponIdx();
	int animIdx = GetOwner()->Animator3D()->GetCurAnimClip()->GetClipIdx();

	if(weaponIdx == PISTOL_IDX)
	{
		if(animIdx == 10)
		{
			SoundMgr::GetInst()->Play(L"sound\\weapon\\pistol_release_mag.ogg", pPlayer->Transform()->GetRelativePos(), 0, 10.f, SoundMgr::SOUND_TYPE::SFX, 0.2, false);
		}
		if (animIdx == 33)
		{
			SoundMgr::GetInst()->Play(L"sound\\weapon\\pistol_load_mag.ogg", pPlayer->Transform()->GetRelativePos(), 0, 10.f, SoundMgr::SOUND_TYPE::SFX, 0.2, false);
		}
		if (animIdx == 50)
		{
			SoundMgr::GetInst()->Play(L"sound\\weapon\\pistol_slider.ogg", pPlayer->Transform()->GetRelativePos(), 0, 10.f, SoundMgr::SOUND_TYPE::SFX, 0.2, false);
		}
	}

	if (weaponIdx == SMG_IDX)
	{
		if (animIdx == 20)
		{
			SoundMgr::GetInst()->Play(L"sound\\weapon\\pistol_release_mag.ogg", pPlayer->Transform()->GetRelativePos(), 0, 10.f, SoundMgr::SOUND_TYPE::SFX, 0.2, false);
		}
		if (animIdx == 63)
		{
			SoundMgr::GetInst()->Play(L"sound\\weapon\\smg_load_mag.ogg", pPlayer->Transform()->GetRelativePos(), 0, 10.f, SoundMgr::SOUND_TYPE::SFX, 0.2, false);
		}
		if (animIdx == 104)
		{
			SoundMgr::GetInst()->Play(L"sound\\weapon\\smg_slider.ogg", pPlayer->Transform()->GetRelativePos(), 0, 10.f, SoundMgr::SOUND_TYPE::SFX, 0.2, false);
		}
	}

	if (weaponIdx == SNIPER_IDX)
	{
		if (animIdx == 17)
		{
			SoundMgr::GetInst()->Play(L"sound\\weapon\\sniper_release_mag.ogg", pPlayer->Transform()->GetRelativePos(), 0, 10.f, SoundMgr::SOUND_TYPE::SFX, 0.2, false);
		}
		if (animIdx == 50)
		{
			SoundMgr::GetInst()->Play(L"sound\\weapon\\sniper_load_mag.ogg", pPlayer->Transform()->GetRelativePos(), 0, 10.f, SoundMgr::SOUND_TYPE::SFX, 0.2, false);
		}
		if (animIdx == 93)
		{
			SoundMgr::GetInst()->Play(L"sound\\weapon\\sniper_slider.ogg", pPlayer->Transform()->GetRelativePos(), 0, 10.f, SoundMgr::SOUND_TYPE::SFX, 0.2, false);
		}
	}
}

void CPlayerScript::SaveToLevelFile(FILE* _File)
{
	fwrite(&fSpeed, sizeof(float), 1, _File);
}

void CPlayerScript::LoadFromLevelFile(FILE* _File)
{
	fread(&fSpeed, sizeof(float), 1, _File);
}