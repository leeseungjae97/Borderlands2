#include "pch.h"
#include "CWarriorScript.h"

#include <Engine/CUI.h>
#include <Engine/RaycastMgr.h>
#include <Engine\AnimationMgr.h>

#include "CAttackBurnScript.h"
#include "CAttackNormalScript.h"
#include "CPlayerScript.h"

CWarriorScript::CWarriorScript()
	: CScript((UINT)SCRIPT_TYPE::WARRIOR_SCRIPT)
	, tState(WARRIOR_STATE::HIDE)
	, m_bRockThrow(false)
	, pBreath(nullptr)
	, pTailBeam(nullptr)
	, pRock(nullptr)
	, pBreathCollider(nullptr)
	, pTailBeamCollider(nullptr)
	, pTailCollider(nullptr)
	, pHeadCollider(nullptr)
	, pMouseCollider(nullptr)
	, pChestCollider(nullptr)
	, pStomachCollider(nullptr)
	, iWarriorHp(500)
	, iWarriorHpCapa(500)
{
	AddScriptParam(SCRIPT_PARAM::VEC4, &m_vRot, "offset rotate");
	AddScriptParam(SCRIPT_PARAM::VEC4, &m_R, "rotate r");
}

CWarriorScript::~CWarriorScript()
{
}

void CWarriorScript::begin()
{
	initAnim();
	createUI();
}

void CWarriorScript::DoFarBreath()
{
	if (tState == WARRIOR_STATE::FAR_BREATH
		|| tState == WARRIOR_STATE::MID_BREATH
		|| tState == WARRIOR_STATE::NEAR_BREATH
		|| tState == WARRIOR_STATE::DIE)
		return;

	CGameObject* pWarrior = GetOwner();

	pWarrior->Animator3D()->Play((UINT)WARRIOR_ANIMATION_TYPE::FAR_BREATH, false);
	tState = WARRIOR_STATE::FAR_BREATH;
}

void CWarriorScript::DoMidBreath()
{
	if (tState == WARRIOR_STATE::FAR_BREATH
		|| tState == WARRIOR_STATE::MID_BREATH
		|| tState == WARRIOR_STATE::NEAR_BREATH
		|| tState == WARRIOR_STATE::DIE)
		return;

	CGameObject* pWarrior = GetOwner();

	pWarrior->Animator3D()->Play((UINT)WARRIOR_ANIMATION_TYPE::MID_BREATH, false);
	tState = WARRIOR_STATE::MID_BREATH;
}

void CWarriorScript::DoNearBreath()
{
	if (tState == WARRIOR_STATE::FAR_BREATH
		|| tState == WARRIOR_STATE::MID_BREATH
		|| tState == WARRIOR_STATE::NEAR_BREATH
		|| tState == WARRIOR_STATE::DIE)
		return;

	CGameObject* pWarrior = GetOwner();

	pWarrior->Animator3D()->Play((UINT)WARRIOR_ANIMATION_TYPE::NEAR_BREATH, false);
	tState = WARRIOR_STATE::NEAR_BREATH;
}

void CWarriorScript::DoTailBeam()
{
	if (tState == WARRIOR_STATE::TAIL_BEAM_ENTER
		|| tState == WARRIOR_STATE::TAIL_BEAM_EXIT
		|| tState == WARRIOR_STATE::TAIL_BEAM_IDLE
		|| tState == WARRIOR_STATE::DIE
		)
		return;

	CGameObject* pWarrior = GetOwner();

	pWarrior->Animator3D()->Play((UINT)WARRIOR_ANIMATION_TYPE::TAIL_BEAM_ENTER, false);
	tState = WARRIOR_STATE::TAIL_BEAM_ENTER;
}

void CWarriorScript::DoThrowRock()
{
	if (tState == WARRIOR_STATE::ROCK_THROW
		|| tState == WARRIOR_STATE::DIE)
		return;

	CGameObject* pWarrior = GetOwner();

	pWarrior->Animator3D()->Play((UINT)WARRIOR_ANIMATION_TYPE::ROCK_THROW, false);
	tState = WARRIOR_STATE::ROCK_THROW;
}

void CWarriorScript::DoTailAttack()
{
	if (tState == WARRIOR_STATE::TAIL_ATTACK
		|| tState == WARRIOR_STATE::DIE)
		return;

	CGameObject* pWarrior = GetOwner();

	pWarrior->Animator3D()->Play((UINT)WARRIOR_ANIMATION_TYPE::TAIL_ATTACK, false);
	tState = WARRIOR_STATE::TAIL_ATTACK;
}

void CWarriorScript::makeCollider()
{
	{
		CGameObject* pObj = new CGameObject;
		pObj->AddComponent(new CCollider3D(false, true));
		pObj->Collider3D()->SetUnityOwner(GetOwner());
		pObj->Collider3D()->SetScale(Vec3(1000.f, 1000.f, 1000.f));

		pObj->SetName(L"fbx warrior head collider");

		SpawnGameObject(pObj, Vec3(0.f, 100.f, 0.f), LAYER_TYPE::Enemy, true);
		pHeadCollider = pObj;
	}
	//{
	//	CGameObject* pObj = new CGameObject;
	//	pObj->AddComponent(new CCollider3D(false, true));
	//	pObj->Collider3D()->SetUnityOwner(GetOwner());
	//	pObj->Collider3D()->SetScale(Vec3(500.f, 500.f, 500.f));

	//	pObj->SetName(L"fbx warrior mouse collider");

	//	SpawnGameObject(pObj, Vec3(0.f, 100.f, 0.f), LAYER_TYPE::Enemy, true);
	//	pMouseCollider = pObj;
	//}
	{
		CGameObject* pObj = new CGameObject;
		pObj->AddComponent(new CCollider3D(false, true));
		pObj->Collider3D()->SetUnityOwner(GetOwner());
		pObj->Collider3D()->SetScale(Vec3(3000.f, 3000.f, 3000.f));

		pObj->SetName(L"fbx warrior stomach collider");

		SpawnGameObject(pObj, Vec3(0.f, 100.f, 0.f), LAYER_TYPE::Enemy, true);
		pStomachCollider = pObj;
	}
	{
		CGameObject* pObj = new CGameObject;
		pObj->AddComponent(new CCollider3D(false, true));
		pObj->Collider3D()->SetUnityOwner(GetOwner());
		pObj->Collider3D()->SetScale(Vec3(1000.f, 1000.f, 1000.f));

		pObj->SetName(L"fbx warrior chest collider");

		SpawnGameObject(pObj, Vec3(0.f, 100.f, 0.f), LAYER_TYPE::Enemy, true);
		pChestCollider = pObj;
	}
	{
		CGameObject* pObj = new CGameObject;
		pObj->AddComponent(new CCollider3D(false, true));
		pObj->Collider3D()->SetUnityOwner(GetOwner());
		pObj->Collider3D()->SetScale(Vec3(3000.f, 300.f, 3000.f));
		pObj->AddComponent(new CGizmo(true));
		pObj->AddComponent(new CAttackNormalScript);

		pObj->SetName(L"fbx tail attack collider");

		SpawnGameObject(pObj, Vec3(0.f, 100.f, 0.f), LAYER_TYPE::NoRaycastingCollider, true);
		pTailCollider = pObj;
	}
	{
		CGameObject* pObj = new CGameObject;
		pObj->AddComponent(new CCollider3D(false, true));
		pObj->Collider3D()->SetUnityOwner(GetOwner());
		pObj->Collider3D()->SetScale(Vec3(1000.f, 300.f, 1000.f));
		pObj->AddComponent(new CGizmo(true));
		pObj->AddComponent(new CAttackBurnScript);

		pObj->SetName(L"fbx fire breath collider");

		SpawnGameObject(pObj, Vec3(0.f, 100.f, 0.f), LAYER_TYPE::NoRaycastingCollider, true);
		pBreathCollider = pObj;
	}
	{
		CGameObject* pObj = new CGameObject;
		pObj->AddComponent(new CCollider3D(false, true));
		pObj->Collider3D()->SetUnityOwner(GetOwner());
		pObj->Collider3D()->SetScale(Vec3(500.f, 500.f, 500.f));
		pObj->AddComponent(new CGizmo(true));
		pObj->AddComponent(new CAttackNormalScript);

		pObj->SetName(L"fbx tail beam collider");

		SpawnGameObject(pObj, Vec3(0.f, 100.f, 0.f), LAYER_TYPE::NoRaycastingCollider, true);
		pTailBeamCollider = pObj;
	}
	{
		CGameObject* pObj = new CGameObject;
		pObj->AddComponent(new CCollider3D(false, true));
		pObj->Collider3D()->SetUnityOwner(GetOwner());
		pObj->Collider3D()->SetScale(Vec3(5000.f, 5000.f, 5000.f));
		pObj->AddComponent(new CGizmo(true));
		pObj->AddComponent(new CAttackNormalScript);

		pObj->SetName(L"fbx warrior attack boundary collider");

		SpawnGameObject(pObj, Vec3(0.f, 0.f, 0.f), LAYER_TYPE::NoRaycastingCollider, true);
		pAttackBoundCollider = pObj;
	}
}

void CWarriorScript::initAnim()
{
	CGameObject* pWarrior = GetOwner();
	pWarrior->Animator3D()->SetAnimClipEventIdx(
		(UINT)WARRIOR_ANIMATION_TYPE::FAR_BREATH, -1, -1, 103, 130);

	pWarrior->Animator3D()->SetAnimClipEventIdx(
		(UINT)WARRIOR_ANIMATION_TYPE::NEAR_BREATH, -1, -1, 57, 114);

	pWarrior->Animator3D()->SetAnimClipEventIdx(
		(UINT)WARRIOR_ANIMATION_TYPE::MID_BREATH, -1, -1, 87, 140);

	pWarrior->Animator3D()->SetAnimClipEventIdx(
		(UINT)WARRIOR_ANIMATION_TYPE::ROCK_THROW, -1, -1, 93, -1);

	pWarrior->Animator3D()->EndEvent((UINT)WARRIOR_ANIMATION_TYPE::FAR_BREATH)
		= std::make_shared<std::function<void()>>([=]()
			{
				tState = WARRIOR_STATE::IDLE;
			});
	pWarrior->Animator3D()->EndEvent((UINT)WARRIOR_ANIMATION_TYPE::NEAR_BREATH)
		= std::make_shared<std::function<void()>>([=]()
			{
				tState = WARRIOR_STATE::IDLE;
			});
	pWarrior->Animator3D()->EndEvent((UINT)WARRIOR_ANIMATION_TYPE::MID_BREATH)
		= std::make_shared<std::function<void()>>([=]()
			{
				tState = WARRIOR_STATE::IDLE;
			});
	pWarrior->Animator3D()->ProgressEvent((UINT)WARRIOR_ANIMATION_TYPE::FAR_BREATH)
		= std::make_shared<std::function<void()>>([=]()
			{
				pBreath->SetObjectState(CGameObject::OBJECT_STATE::VISIBLE);
			});

	pWarrior->Animator3D()->CompleteEvent((UINT)WARRIOR_ANIMATION_TYPE::FAR_BREATH)
		= std::make_shared<std::function<void()>>([=]()
			{
				pBreath->SetObjectState(CGameObject::OBJECT_STATE::INVISIBLE);
			});
	pWarrior->Animator3D()->ProgressEvent((UINT)WARRIOR_ANIMATION_TYPE::MID_BREATH)
		= std::make_shared<std::function<void()>>([=]()
			{
				pBreath->SetObjectState(CGameObject::OBJECT_STATE::VISIBLE);
			});

	pWarrior->Animator3D()->CompleteEvent((UINT)WARRIOR_ANIMATION_TYPE::MID_BREATH)
		= std::make_shared<std::function<void()>>([=]()
			{
				//pBreath->SetObjectState(CGameObject::OBJECT_STATE::INVISIBLE);
			});
	pWarrior->Animator3D()->ProgressEvent((UINT)WARRIOR_ANIMATION_TYPE::NEAR_BREATH)
		= std::make_shared<std::function<void()>>([=]()
			{
				pBreath->SetObjectState(CGameObject::OBJECT_STATE::VISIBLE);
			});

	pWarrior->Animator3D()->CompleteEvent((UINT)WARRIOR_ANIMATION_TYPE::NEAR_BREATH)
		= std::make_shared<std::function<void()>>([=]()
			{
				pBreath->SetObjectState(CGameObject::OBJECT_STATE::INVISIBLE);
			});
	pWarrior->Animator3D()->EndEvent((UINT)WARRIOR_ANIMATION_TYPE::TAIL_BEAM_ENTER)
		= std::make_shared<std::function<void()>>([=]()
			{
				pTailBeam->SetObjectState(CGameObject::OBJECT_STATE::VISIBLE);
				tState = WARRIOR_STATE::TAIL_BEAM_IDLE;
			});
	pWarrior->Animator3D()->EndEvent((UINT)WARRIOR_ANIMATION_TYPE::TAIL_BEAM_IDLE)
		= std::make_shared<std::function<void()>>([=]()
			{
				pTailBeam->SetObjectState(CGameObject::OBJECT_STATE::INVISIBLE);
				tState = WARRIOR_STATE::TAIL_BEAM_EXIT;
			});

	pWarrior->Animator3D()->EndEvent((UINT)WARRIOR_ANIMATION_TYPE::TAIL_BEAM_EXIT)
		= std::make_shared<std::function<void()>>([=]()
			{
				tState = WARRIOR_STATE::IDLE;
			});


	pWarrior->Animator3D()->EndEvent((UINT)WARRIOR_ANIMATION_TYPE::TAIL_ATTACK)
		= std::make_shared<std::function<void()>>([=]()
			{
				tState = WARRIOR_STATE::IDLE;
			});

	pWarrior->Animator3D()->StartEvent((UINT)WARRIOR_ANIMATION_TYPE::ROCK_THROW)
		= std::make_shared<std::function<void()>>([=]()
			{
				m_bRockThrow = false;
				m_bRockFollow = true;
				pRock->SetObjectState(CGameObject::OBJECT_STATE::VISIBLE);
			});
	pWarrior->Animator3D()->ProgressEvent((UINT)WARRIOR_ANIMATION_TYPE::ROCK_THROW)
		= std::make_shared<std::function<void()>>([=]()
			{
				m_bRockFollow = false;
			});
	pWarrior->Animator3D()->EndEvent((UINT)WARRIOR_ANIMATION_TYPE::ROCK_THROW)
		= std::make_shared<std::function<void()>>([=]()
			{
				tState = WARRIOR_STATE::IDLE;
			});
	pWarrior->Animator3D()->EndEvent((UINT)WARRIOR_ANIMATION_TYPE::APPEAR)
		= std::make_shared<std::function<void()>>([=]()
			{
				tState = WARRIOR_STATE::IDLE;
			});
	pWarrior->Animator3D()->EndEvent((UINT)WARRIOR_ANIMATION_TYPE::DIE)
		= std::make_shared<std::function<void()>>([=]()
			{

			});


}

void CWarriorScript::breathMove()
{
	CGameObject* pWarrior = GetOwner();
	if (nullptr != pBreath && pBreath->GetObjectState() == CGameObject::OBJECT_STATE::VISIBLE)
	{
		int _FirebreathIdx = pWarrior->Animator3D()->GetFireBreathIdx();
		Vec3 vPos = AnimationMgr::GetInst()->BonePos(_FirebreathIdx, pWarrior);
		Vec3 vRot = AnimationMgr::GetInst()->BoneRot(_FirebreathIdx, pWarrior);

		vRot.y *= -1.f;
		vRot.y -= XM_PI / 2.f;

		pBreath->Transform()->SetRelativePos(vPos);
		pBreath->Transform()->SetRelativeRot(vRot);

		tRayInfo rayInfo{};
		rayInfo.fDamage = 10.f;
		rayInfo.iLayerIdx = (int)LAYER_TYPE::EnemyBullet;
		rayInfo.vDir = pBreath->Transform()->GetRelativeDir(DIR_TYPE::FRONT);
		rayInfo.vStart = pBreath->Transform()->GetRelativePos();
		rayInfo.matWorld = pBreath->Transform()->GetDrawRayMat();
		rayInfo.tRayType = (UINT)RAYCAST_TYPE::SHOOT;

		Vec3 vPosition;
		RaycastMgr::GetInst()->DoOneHitRaycast(rayInfo, &vPosition, RAYCAST_GROUP_TYPE::Enemy);
		Vec3 vCurPos = pBreath->Transform()->GetRelativePos();

		Vec3 vScale = pBreath->Transform()->GetRelativeScale();
		vScale.z = fabs(vPosition.z);
		pBreath->Transform()->SetRelativeScale(vScale);

		pBreathCollider->Collider3D()->SetColliderPos(vCurPos + (rayInfo.vDir * vPosition.z));
	}
}

void CWarriorScript::beamMove()
{
	CGameObject* pWarrior = GetOwner();
	if (nullptr != pTailBeam && pTailBeam->GetObjectState() == CGameObject::OBJECT_STATE::VISIBLE)
	{
		int _TailIdx = pWarrior->Animator3D()->GetTailIdx();
		Vec3 vPos = AnimationMgr::GetInst()->BonePos(_TailIdx, pWarrior);
		//Vec3 vRot = AnimationMgr::GetInst()->BoneRot(_TailIdx, pWarrior);
		Vec3 vRot = pTailBeam->Transform()->GetRelativeRot();
		Vec3 vPlayerPos = PlayerMgr::GetInst()->GetPlayer()->Transform()->GetRelativePos();
		Vec3 vUp = pTailBeam->Transform()->GetRelativeDir(DIR_TYPE::UP);
		Vec3 vFront = pTailBeam->Transform()->GetRelativeDir(DIR_TYPE::FRONT);
		Vec3 vDiff = vPlayerPos - vPos;
		Vec3 vDir = vDiff;
		vDir.Normalize();


		Matrix rotMat = XMMatrixLookToLH(vPos, vDir, vUp);

		Quat quat;
		Vec3 vS, vT;
		rotMat.Decompose(vS, quat, vT);

		vRot = physx::Util::QuaternionToVector3(quat);

		pTailBeam->Transform()->SetRelativePos(vPos);
		pTailBeam->Transform()->SetRelativeRot(vRot);

		tRayInfo rayInfo{};
		rayInfo.fDamage = 10.f;
		rayInfo.iLayerIdx = (int)LAYER_TYPE::EnemyBullet;
		rayInfo.vDir = pTailBeam->Transform()->GetRelativeDir(DIR_TYPE::FRONT);
		rayInfo.vStart = pTailBeam->Transform()->GetRelativePos();
		rayInfo.matWorld = pTailBeam->Transform()->GetDrawRayMat();
		rayInfo.tRayType = (UINT)RAYCAST_TYPE::SHOOT;


		Vec3 vPosition;
		RaycastMgr::GetInst()->DoOneHitRaycast(rayInfo, &vPosition, RAYCAST_GROUP_TYPE::Enemy);
		Vec3 vCurPos = pTailBeam->Transform()->GetRelativePos();
		//Vec3 vLength = (*vPosition) - vCurPos;

		Vec3 vScale = pTailBeam->Transform()->GetRelativeScale();
		vScale.z = fabs(vPosition.z);
		pTailBeam->Transform()->SetRelativeScale(vScale);

		pTailBeamCollider->Collider3D()->SetColliderPos(vCurPos + (rayInfo.vDir * vPosition.z));
	}
}

void CWarriorScript::rockMove()
{
	CGameObject* pWarrior = GetOwner();
	if (m_bRockFollow && !m_bRockThrow)
	{
		int knuckleIdx = pWarrior->Animator3D()->GetKnuckleIdx();
		Vec3 vPos = AnimationMgr::GetInst()->BonePos(knuckleIdx, pWarrior);
		Vec3 vRot = AnimationMgr::GetInst()->BoneRot(knuckleIdx, pWarrior);
		if (pRock)
		{
			if (pRock->RigidBody()->IsRigidBodyCreate())
				pRock->RigidBody()->SetLinearVelocityZero();

			pRock->Transform()->SetRelativePos(vPos);
			pRock->Transform()->SetRelativeRot(vRot);
		}
	}
	else
	{
		if (!m_bRockThrow)
		{
			if (pRock)
			{
				Vec3 vPlayerPos = PlayerMgr::GetInst()->GetPlayer()->Transform()->GetRelativePos();
				Vec3 vRockPos = pRock->Transform()->GetRelativePos();
				Vec3 vDiff = vPlayerPos - vRockPos;
				Vec3 vDir = vDiff;
				vDir.Normalize();

				if (pRock->RigidBody()->IsRigidBodyCreate())
				{
					pRock->RigidBody()->SetLinearVelocity(vDir * 10000.f);
					pRock->RigidBody()->GetDynamicBody()->setMaxLinearVelocity(50000.f);
				}
			}
			m_bRockThrow = true;
		}else
		{
			if (pRock)
			{
				if (pRock->Collider3D()->IsBeginOverlap() || pRock->Collider3D()->IsOnOverlap())
					pRock->SetObjectState(CGameObject::OBJECT_STATE::INVISIBLE);
			}
		}
	}
}

void CWarriorScript::tailMove()
{
	CGameObject* pWarrior = GetOwner();
	if (nullptr != pTailCollider)
	{
		int _TailIdx = pWarrior->Animator3D()->GetTailIdx();
		Vec3 vPos = AnimationMgr::GetInst()->BonePos(_TailIdx, pWarrior);

		pTailCollider->Collider3D()->SetColliderPos(vPos);
	}
}

void CWarriorScript::paperBurn()
{
	if (tState == WARRIOR_STATE::DIE)
		return;

	PlayerMgr::GetInst()->GetPlayer()->GetScript<CPlayerScript>()->AddExp(1);

	CGameObject* pWarrior = GetOwner();
	double dTime = AnimationMgr::GetInst()->GetCurAnimationTime(pWarrior->Animator3D());
	pWarrior->MeshRender()->SetPaparBurn(true, (float)dTime);
}
void CWarriorScript::IsDie()
{
	CGameObject* pWarrior = GetOwner();
	if (pWarrior->MeshRender()->GetMaterial(0)->IsPaperBurnEnd())
	{
		if (pTailBeam)
		{
			DestroyObject(pTailBeam);
		}
		if (pRock)
		{
			DestroyObject(pRock);
		}
		if (pBreath)
		{
			DestroyObject(pBreath);
		}
		if (pBreathCollider)
		{
			DestroyObject(pBreathCollider);
		}
		if (pTailBeamCollider)
		{
			DestroyObject(pTailBeamCollider);
		}
		if (pTailCollider)
		{
			DestroyObject(pTailCollider);
		}
		if (pHeadCollider)
		{
			DestroyObject(pHeadCollider);
		}
		if(pAttackBoundCollider)
		{
			DestroyObject(pAttackBoundCollider);
		}
		//if (pMouseCollider)
		//{
		//	DestroyObject(pMouseCollider);
		//}
		if (pUI_WarriorHP)
		{
			DestroyObject(pUI_WarriorHP);
		}
		if (pUI_WarriorHPBack)
		{
			DestroyObject(pUI_WarriorHPBack);
		}
		if(pWarriorText)
		{
			DestroyObject(pWarriorText);
		}
		if (pChestCollider)
		{
			DestroyObject(pChestCollider);
		}
		if (pStomachCollider)
		{
			DestroyObject(pStomachCollider);
		}
		//pWarrior->SetDead(true);
		DestroyObject(pWarrior);


		//pWarrior->SetObjectState(CGameObject::OBJECT_STATE::INVISIBLE);
	}
}
void CWarriorScript::createUI()
{
	Ptr<CGraphicsShader> UIShader = (CGraphicsShader*)CResMgr::GetInst()->FindRes<CGraphicsShader>(L"UI2DShader").Get();
	Ptr<CGraphicsShader> adjustUIShader = (CGraphicsShader*)CResMgr::GetInst()->FindRes<CGraphicsShader>(L"AdjustUI2DShader").Get();
	Ptr<CMaterial> pMtrl = nullptr;
	{
		pMtrl = new CMaterial(true);
		pMtrl->SetShader(adjustUIShader);
		CResMgr::GetInst()->AddRes(L"WarriorHpBackMtrl", pMtrl);

		Ptr<CTexture> pTex = CResMgr::GetInst()->FindRes<CTexture>(L"texture\\UI\\boss_hp_back.png");
		pUI_WarriorHPBack = new CGameObject;

		pUI_WarriorHPBack->SetName(L"UI Warrior Hp Back");
		pUI_WarriorHPBack->AddComponent(new CTransform);
		pUI_WarriorHPBack->AddComponent(new CMeshRender);

		pUI_WarriorHPBack->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
		pUI_WarriorHPBack->MeshRender()->SetMaterial(pMtrl, 0);
		pUI_WarriorHPBack->MeshRender()->GetMaterial(0)->SetTexParam(TEX_0, pTex);
		pUI_WarriorHPBack->Transform()->SetRelativeScale(Vec3(700.f, 16.f, 1.f));
		SpawnGameObject(pUI_WarriorHPBack, Vec3(0.f, 320.f, 0.f), LAYER_TYPE::ViewPortUI);
	}
	{
		pMtrl = new CMaterial(true);
		pMtrl->SetShader(adjustUIShader);
		CResMgr::GetInst()->AddRes(L"WarriorHpFrontMtrl", pMtrl);

		Ptr<CTexture> pTex = CResMgr::GetInst()->FindRes<CTexture>(L"texture\\UI\\boss_hp_front.png");
		pUI_WarriorHP = new CGameObject;

		pUI_WarriorHP->SetName(L"UI Warrior Hp");
		pUI_WarriorHP->AddComponent(new CTransform);
		pUI_WarriorHP->AddComponent(new CMeshRender);

		pUI_WarriorHP->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
		pUI_WarriorHP->MeshRender()->SetMaterial(pMtrl, 0);
		pUI_WarriorHP->MeshRender()->GetMaterial(0)->SetTexParam(TEX_0, pTex);
		pUI_WarriorHP->Transform()->SetRelativeScale(Vec3(695.f, 14.f, 1.f));
		SpawnGameObject(pUI_WarriorHP, Vec3(0.f, 320.f, 0.f), LAYER_TYPE::ViewPortUI);
	}
	pWarriorText = new CUI();
	pWarriorText->SetName(L"Warrior Text");
	pWarriorText->AddComponent(new CTransform);
	pWarriorText->AddComponent(new CMeshRender);
	pWarriorText->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	pWarriorText->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"AdjustUI2DShaderMtrl"), 0);
	pWarriorText->Transform()->SetRelativeScale(Vec3(100.f, 100.f, 1.f));
	pWarriorText->SetTextScale(0.6f);
	pWarriorText->SetText(L"99 The Warrior");
	pWarriorText->SetOutline(true);
	pWarriorText->SetTextNormalColor(Vec4(1.f, 1.f, 1.f, 1.f));
	SpawnGameObject(pWarriorText, Vec3(-150.f, 280.f, 1.f), LAYER_TYPE::ViewPortUI);
}

void CWarriorScript::colliderMove()
{
	CGameObject* pWarrior = GetOwner();
	CAnimator3D* pAnimator = pWarrior->Animator3D();
	{
		if(pHeadCollider)
		{
			int headIdx = pAnimator->GetHeadIdx();
			Vec3 vPos = AnimationMgr::GetInst()->BonePos(headIdx, pWarrior);
			Vec3 vRot = AnimationMgr::GetInst()->BoneRot(headIdx, pWarrior);

			pHeadCollider->Collider3D()->SetColliderPos(vPos);
			pHeadCollider->Collider3D()->SetColliderRot(vRot);
		}
		
	}
	{
		if(pStomachCollider)
		{
			int stomachIdx = pAnimator->GetStomachIdx();
			Vec3 vPos = AnimationMgr::GetInst()->BonePos(stomachIdx, pWarrior);
			Vec3 vRot = AnimationMgr::GetInst()->BoneRot(stomachIdx, pWarrior);

			pStomachCollider->Collider3D()->SetColliderPos(vPos);
		}
		//pStomachCollider->Transform()->SetRelativeRot(vRot);
	}
	//{
	//	int mouseIdx = pAnimator->GetMouseIdx();
	//	Vec3 vPos = AnimationMgr::GetInst()->BonePos(mouseIdx, pWarrior);
	//	Vec3 vRot = AnimationMgr::GetInst()->BoneRot(mouseIdx, pWarrior);

	//	pMouseCollider->Collider3D()->SetColliderPos(vPos);
	//	pMouseCollider->Collider3D()->SetColliderRot(vRot);
	//}
	{
		if(pChestCollider)
		{
			int chestIdx = pAnimator->GetChestIdx();
			Vec3 vPos = AnimationMgr::GetInst()->BonePos(chestIdx, pWarrior);
			Vec3 vRot = AnimationMgr::GetInst()->BoneRot(chestIdx, pWarrior);

			pChestCollider->Collider3D()->SetColliderPos(vPos);
			pChestCollider->Collider3D()->SetColliderRot(vRot);
		}
	}
}

void CWarriorScript::makeAttackObject()
{
	CGameObject* pWarrior = GetOwner();
	int fireBreathIdx = pWarrior->Animator3D()->GetFireBreathIdx();
	Vec3 vPos = AnimationMgr::GetInst()->BonePos(fireBreathIdx, pWarrior);
	Vec3 vRot = AnimationMgr::GetInst()->BoneRot(fireBreathIdx, pWarrior);
	{
		Ptr<CMeshData> pMeshData = nullptr;
		CGameObject* pObj = nullptr;
		pMeshData = CResMgr::GetInst()->LoadFBX(L"fbx\\fire_breath.fbx");
		pObj = pMeshData->Instantiate(Vec3(1500.f, 1500.f, 3000.f));
		//pObj->AddComponent(new CCollider3D(false));
		//pObj->Collider3D()->SetScale(Vec3(1500.f, 1500.f, 10000.f));
		//pObj->Collider3D()->SetCenter(true);
		pObj->AddComponent(new CGizmo);

		pObj->SetName(L"fbx fire breath");
		pObj->MeshRender()->GetMaterial(0)->SetTexFlow(true);
		pObj->MeshRender()->GetMaterial(0)->SetFlowSpeed(2.f);
		pObj->MeshRender()->GetMaterial(0)->SetFlowDir(Vec2(1.f, 0.0f));

		//vRot.z = -50.f;
		//vRot.y = 0.f;
		//vRot.x = 0.f;
		pObj->Transform()->SetRelativeRot(vRot);
		SpawnGameObject(pObj, vPos, LAYER_TYPE::EnemyBullet);
		pBreath = pObj;
	}
	//pBreath->SetObjectState(CGameObject::OBJECT_STATE::INVISIBLE);


	int tailWeaponIdx = pWarrior->Animator3D()->GetTailIdx();
	vPos = AnimationMgr::GetInst()->BonePos(tailWeaponIdx, pWarrior);
	vRot = AnimationMgr::GetInst()->BoneRot(tailWeaponIdx, pWarrior);
	{
		Ptr<CMeshData> pMeshData = nullptr;
		CGameObject* pObj = nullptr;
		pMeshData = CResMgr::GetInst()->LoadFBX(L"fbx\\tail_beam.fbx");
		pObj = pMeshData->Instantiate(Vec3(1500.f, 1500.f, 3000.f));
		//pObj->AddComponent(new CCollider3D(false));
		//pObj->Collider3D()->SetScale(Vec3(1500.f, 1500.f, 10000.f));
		//pObj->Collider3D()->SetCenter(true);
		pObj->AddComponent(new CGizmo);

		pObj->SetName(L"fbx tail beam");
		pObj->MeshRender()->GetMaterial(0)->SetTexFlow(true);
		pObj->MeshRender()->GetMaterial(0)->SetFlowSpeed(-2.f);
		pObj->MeshRender()->GetMaterial(0)->SetFlowDir(Vec2(1.f, 0.0f));

		//vRot.z = -50.f;
		vRot.z = (-50.f * DegToRad());
		//vRot.y = 0.f;
		//vRot.x = 0.f;
		pObj->Transform()->SetRelativeRot(vRot);
		SpawnGameObject(pObj, vPos, LAYER_TYPE::EnemyBullet);
		pTailBeam = pObj;
	}
	pTailBeam->SetObjectState(CGameObject::OBJECT_STATE::INVISIBLE);

	int knuckleIdx = pWarrior->Animator3D()->GetKnuckleIdx();
	vPos = AnimationMgr::GetInst()->BonePos(knuckleIdx, pWarrior);
	{
		Ptr<CMeshData> pMeshData = nullptr;
		CGameObject* pObj = nullptr;
		pMeshData = CResMgr::GetInst()->LoadFBX(L"fbx\\rock.fbx");
		pObj = pMeshData->Instantiate(Vec3(1500.f, 1500.f, 1500.f));
		pObj->AddComponent(new CRigidBody(RIGID_BODY_SHAPE_TYPE::BOX, RIGID_BODY_TYPE::DYNAMIC));
		pObj->RigidBody()->SetRigidScale(Vec3(300.f, 300.f, 300.f));

		pObj->AddComponent(new CCollider3D());
		pObj->Collider3D()->SetScale(Vec3(305.f, 305.f, 305.f));

		pObj->AddComponent(new CAttackNormalScript());
		pObj->AddComponent(new CGizmo);

		pObj->SetName(L"fbx rock");
		pObj->MeshRender()->GetMaterial(0)->SetTexFlow(true);
		pObj->MeshRender()->GetMaterial(0)->SetFlowSpeed(-2.f);
		pObj->MeshRender()->GetMaterial(0)->SetFlowDir(Vec2(0.5f, 0.0f));

		SpawnGameObject(pObj, vPos, LAYER_TYPE::EnemyBullet);
		pRock = pObj;
	}
	pRock->SetObjectState(CGameObject::OBJECT_STATE::INVISIBLE);
}

void CWarriorScript::tick()
{
	if (tState == WARRIOR_STATE::DIE)
	{
		IsDie();
		return;
	}

	CGameObject* pWarrior = GetOwner();
	static bool binit = false;
	if (!binit)
	{
		if (GetOwner()->MeshRender()->GetMaterial(3) != nullptr)
		{
			GetOwner()->MeshRender()->GetMaterial(3)->SetTexFlow(true);
		}

		makeAttackObject();
		makeCollider();
		binit = true;
	}

	if(pAttackBoundCollider->Collider3D()->IsColliderRigidCreate())
	{
		//Vec3 vPos = GetOwner()->Transform()->GetRelativePos();
		//Vec3 vFront = GetOwner()->Transform()->GetRelativeDir(DIR_TYPE::FRONT);

		//vPos += (vFront * 10000.f);

		pAttackBoundCollider->Collider3D()->SetColliderPos(Vec3(-2083.733f, 1279.689f, 10716.288f));
	}

	//if(pAttackBoundCollider->Collider3D()->IsBeginOverlap() || pAttackBoundCollider->Collider3D()->IsOnOverlap())
	//{
	//	if(tState == WARRIOR_STATE::HIDE)
	//	{
	//		tState = WARRIOR_STATE::APPEAR;
	//	}

	//	if (tState == WARRIOR_STATE::IDLE)
	//	{
	//		m_fActAcc += DT;

	//		int randStart = rand() % 20;
	//		if (randStart == (int)m_fActAcc)
	//		{
	//			m_fActAcc = 0.0f;
	//			m_RandBehavior = rand() % 5;

	//			switch(m_RandBehavior)
	//			{
	//			case 0:
	//			{
	//				DoFarBreath();
	//			}
	//			break;
	//			case 1:
	//			{
	//				DoNearBreath();
	//			}
	//			break;
	//			case 2:
	//			{
	//				DoThrowRock();
	//			}
	//			break;
	//			case 3:
	//			{
	//				DoTailBeam();
	//			}
	//			break;
	//			case 4:
	//			{
	//				DoTailAttack();
	//			}
	//			break;
	//			}
	//		}
	//	}else
	//	{
	//		m_fActAcc = 0.0f;
	//	}
	//}

	//if (KEY_PRESSED(KEY::Q))
	//{
	//	DoFarBreath();
	//}
	//if (KEY_PRESSED(KEY::E))
	//{
	//	DoNearBreath();
	//}
	//if (KEY_PRESSED(KEY::R))
	//{
	//	DoThrowRock();
	//}
	//if (KEY_PRESSED(KEY::T))
	//{
	//	DoTailBeam();
	//}
	//if (KEY_PRESSED(KEY::Y))
	//{
	//	DoTailAttack();
	//}

	beamMove();
	breathMove();
	rockMove();
	tailMove();
	colliderMove();

	
	float ratio = 1.f - GetHpRatio();
	pUI_WarriorHP->MeshRender()->GetMaterial(0)->SetScalarParam(FLOAT_0, &ratio);

	if (iWarriorHp == 0)
	{
		pWarrior->Animator3D()->Play((UINT)WARRIOR_ANIMATION_TYPE::DIE, false, false);
		paperBurn();
		tState = WARRIOR_STATE::DIE;
	}
	if (tState == WARRIOR_STATE::IDLE)
	{
		pWarrior->Animator3D()->Play((UINT)WARRIOR_ANIMATION_TYPE::IDLE, true);
	}
	if (tState == WARRIOR_STATE::TAIL_BEAM_IDLE)
	{
		GetOwner()->Animator3D()->Play((UINT)WARRIOR_ANIMATION_TYPE::TAIL_BEAM_IDLE, false, false);
	}
	if (tState == WARRIOR_STATE::TAIL_BEAM_EXIT)
	{
		GetOwner()->Animator3D()->Play((UINT)WARRIOR_ANIMATION_TYPE::TAIL_BEAM_EXIT, false, false);
	}
	if (tState == WARRIOR_STATE::HIDE)
	{
		GetOwner()->Animator3D()->Play((UINT)WARRIOR_ANIMATION_TYPE::HIDE, true);
	}
	if (tState == WARRIOR_STATE::APPEAR)
	{
		GetOwner()->Animator3D()->Play((UINT)WARRIOR_ANIMATION_TYPE::APPEAR, false, false);
	}
}

void CWarriorScript::collision()
{

}

void CWarriorScript::finaltick()
{
	if (tState == WARRIOR_STATE::DIE)
		return;

	CGameObject* pWarrior = GetOwner();
	static bool binit = false;
	if (!binit)
	{
		if (GetOwner()->MeshRender()->GetMaterial(3) != nullptr)
		{
			GetOwner()->MeshRender()->GetMaterial(3)->SetTexFlow(true);
		}

		//makeAttackObject();
		//makeCollider();
		binit = true;
	}

	if (KEY_TAP(KEY::F))
	{
		makeAttackObject();
		makeCollider();
		initAnim();
	}
	if (KEY_PRESSED(KEY::Q))
	{
		DoFarBreath();
	}
	if (KEY_PRESSED(KEY::E))
	{
		DoNearBreath();
	}
	if(KEY_PRESSED(KEY::U))
	{
		DoMidBreath();
	}
	if (KEY_PRESSED(KEY::R))
	{
		DoThrowRock();
	}
	if (KEY_PRESSED(KEY::T))
	{
		DoTailBeam();
	}
	if (KEY_PRESSED(KEY::Y))
	{
		DoTailAttack();
	}


	beamMove();
	breathMove();
	rockMove();
	tailMove();
	colliderMove();

	//if (iWarriorHp == 0)
	//{
	//	tState = WARRIOR_STATE::DIE;
	//}
	//if (tState == WARRIOR_STATE::DIE)
	//{
	//	pWarrior->Animator3D()->Play((UINT)WARRIOR_ANIMATION_TYPE::DIE, false);
	//}
	if (tState == WARRIOR_STATE::IDLE)
	{
		pWarrior->Animator3D()->Play((UINT)WARRIOR_ANIMATION_TYPE::IDLE, true);
	}
	if (tState == WARRIOR_STATE::TAIL_BEAM_IDLE)
	{
		GetOwner()->Animator3D()->Play((UINT)WARRIOR_ANIMATION_TYPE::TAIL_BEAM_IDLE, false, false);
	}
	if (tState == WARRIOR_STATE::TAIL_BEAM_EXIT)
	{
		GetOwner()->Animator3D()->Play((UINT)WARRIOR_ANIMATION_TYPE::TAIL_BEAM_EXIT, false, false);
	}
}

void CWarriorScript::Attacked(int _Damage)
{
	if (iWarriorHp < _Damage)
		iWarriorHp = 0;
	else
		iWarriorHp -= _Damage;
}

void CWarriorScript::Raycast(tRayInfo _RaycastInfo)
{
	if (pHeadCollider)
	{
		if (pHeadCollider->Collider3D()->IsRaycast())
		{
			Attacked(_RaycastInfo.fDamage * 2);
		}
	}
	if (pStomachCollider)
	{
		if (pStomachCollider->Collider3D()->IsRaycast())
		{
			Attacked(_RaycastInfo.fDamage);
		}
	}
	if (pMouseCollider)
	{
		if (pMouseCollider->Collider3D()->IsRaycast())
		{
			Attacked(_RaycastInfo.fDamage * 3);
		}
	}
	if (pChestCollider)
	{
		if (pChestCollider->Collider3D()->IsRaycast())
		{
			Attacked(_RaycastInfo.fDamage * 2);
		}
	}
}

void CWarriorScript::LoadFromLevelFile(FILE* _FILE)
{
	//makeCollider();
	//makeAttackObject();
}

void CWarriorScript::SaveToLevelFile(FILE* _File)
{
	//pBreathCollider;
	//pTailBeamCollider;
	//pTailBeam;
	//pBreath;
}
