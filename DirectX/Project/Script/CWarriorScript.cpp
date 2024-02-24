#include "pch.h"
#include "CWarriorScript.h"

#include <Engine/RaycastMgr.h>
#include <Engine\AnimationMgr.h>

#include "CAttackBurnScript.h"
#include "CAttackNormalScript.h"

CWarriorScript::CWarriorScript()
	: CScript((UINT)SCRIPT_TYPE::WARRIOR_SCRIPT)
	, tState(WarriorMgr::WARRIOR_STATE::IDLE)
	, m_bRockThrow(false)
	, pBreath(nullptr)
	, pTailBeam(nullptr)
	, pRock(nullptr)
	, pBreathCollisionPoint(nullptr)
	, pTailBeamCollisionPoint(nullptr)
	, pTailCollisionPoint(nullptr)
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
}

void CWarriorScript::DoFarBreath()
{
	if (tState == WarriorMgr::WARRIOR_STATE::FAR_BREATH
		|| tState == WarriorMgr::WARRIOR_STATE::MID_BREATH
		|| tState == WarriorMgr::WARRIOR_STATE::NEAR_BREATH)
		return;

	CGameObject* pWarrior = GetOwner();

	pWarrior->Animator3D()->Play((UINT)WARRIOR_ANIMATION_TYPE::FAR_BREATH, false);
	tState = WarriorMgr::WARRIOR_STATE::FAR_BREATH;
}

void CWarriorScript::DoMidBreath()
{
	if (tState == WarriorMgr::WARRIOR_STATE::FAR_BREATH
		|| tState == WarriorMgr::WARRIOR_STATE::MID_BREATH
		|| tState == WarriorMgr::WARRIOR_STATE::NEAR_BREATH)
		return;

	CGameObject* pWarrior = GetOwner();

	pWarrior->Animator3D()->Play((UINT)WARRIOR_ANIMATION_TYPE::MID_BREATH, false);
	tState = WarriorMgr::WARRIOR_STATE::MID_BREATH;
}

void CWarriorScript::DoNearBreath()
{
	if (tState == WarriorMgr::WARRIOR_STATE::FAR_BREATH
		|| tState == WarriorMgr::WARRIOR_STATE::MID_BREATH
		|| tState == WarriorMgr::WARRIOR_STATE::NEAR_BREATH)
		return;

	CGameObject* pWarrior = GetOwner();

	pWarrior->Animator3D()->Play((UINT)WARRIOR_ANIMATION_TYPE::NEAR_BREATH, false);
	tState = WarriorMgr::WARRIOR_STATE::NEAR_BREATH;
}

void CWarriorScript::DoTailBeam()
{
	if (tState == WarriorMgr::WARRIOR_STATE::TAIL_BEAM_ENTER
		|| tState == WarriorMgr::WARRIOR_STATE::TAIL_BEAM_EXIT
		|| tState == WarriorMgr::WARRIOR_STATE::TAIL_BEAM_IDLE)
		return;

	CGameObject* pWarrior = GetOwner();

	pWarrior->Animator3D()->Play((UINT)WARRIOR_ANIMATION_TYPE::TAIL_BEAM_ENTER, false);
	tState = WarriorMgr::WARRIOR_STATE::TAIL_BEAM_ENTER;
}

void CWarriorScript::DoThrowRock()
{
	if (tState == WarriorMgr::WARRIOR_STATE::ROCK_THROW)
		return;

	CGameObject* pWarrior = GetOwner();

	pWarrior->Animator3D()->Play((UINT)WARRIOR_ANIMATION_TYPE::ROCK_THROW, false);
	tState = WarriorMgr::WARRIOR_STATE::ROCK_THROW;
}

void CWarriorScript::DoTailAttack()
{
	if (tState == WarriorMgr::WARRIOR_STATE::TAIL_ATTACK)
		return;

	CGameObject* pWarrior = GetOwner();

	pWarrior->Animator3D()->Play((UINT)WARRIOR_ANIMATION_TYPE::TAIL_ATTACK, false);
	tState = WarriorMgr::WARRIOR_STATE::TAIL_ATTACK;
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
	{
		CGameObject* pObj = new CGameObject;
		pObj->AddComponent(new CCollider3D(false, true));
		pObj->Collider3D()->SetUnityOwner(GetOwner());
		pObj->Collider3D()->SetScale(Vec3(500.f, 500.f, 500.f));

		pObj->SetName(L"fbx warrior mouse collider");

		SpawnGameObject(pObj, Vec3(0.f, 100.f, 0.f), LAYER_TYPE::Enemy, true);
		pMouseCollider = pObj;
	}
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
		pTailCollisionPoint = pObj;
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
		pBreathCollisionPoint = pObj;
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
		pTailBeamCollisionPoint = pObj;
	}
}

void CWarriorScript::initAnim()
{
	CGameObject* pWarrior = GetOwner();
	pWarrior->Animator3D()->SetAnimClipEventIdx(
		(UINT)WARRIOR_ANIMATION_TYPE::FAR_BREATH, -1, -1, 103, 130);

	pWarrior->Animator3D()->SetAnimClipEventIdx(
		(UINT)WARRIOR_ANIMATION_TYPE::NEAR_BREATH, -1, -1, 57, 114);

	//pWarrior->Animator3D()->SetAnimClipEventIdx(
	//	(UINT)WARRIOR_ANIMATION_TYPE::MID_BREATH, -1, -1, 87, 140);

	pWarrior->Animator3D()->SetAnimClipEventIdx(
		(UINT)WARRIOR_ANIMATION_TYPE::ROCK_THROW, -1, -1, 93, -1);

	pWarrior->Animator3D()->EndEvent((UINT)WARRIOR_ANIMATION_TYPE::FAR_BREATH)
		= std::make_shared<std::function<void()>>([=]()
			{
				tState = WarriorMgr::WARRIOR_STATE::IDLE;
			});
	pWarrior->Animator3D()->EndEvent((UINT)WARRIOR_ANIMATION_TYPE::NEAR_BREATH)
		= std::make_shared<std::function<void()>>([=]()
			{
				tState = WarriorMgr::WARRIOR_STATE::IDLE;
			});
	//pWarrior->Animator3D()->EndEvent((UINT)WARRIOR_ANIMATION_TYPE::MID_BREATH)
	//	= std::make_shared<std::function<void()>>([=]()
	//		{
	//			tState = WarriorMgr::WARRIOR_STATE::IDLE;
	//		});
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
	//pWarrior->Animator3D()->ProgressEvent((UINT)WARRIOR_ANIMATION_TYPE::MID_BREATH)
	//	= std::make_shared<std::function<void()>>([=]()
	//		{
	//			pBreath->SetObjectState(CGameObject::OBJECT_STATE::VISIBLE);
	//		});

	//pWarrior->Animator3D()->CompleteEvent((UINT)WARRIOR_ANIMATION_TYPE::MID_BREATH)
	//	= std::make_shared<std::function<void()>>([=]()
	//		{
	//			pBreath->SetObjectState(CGameObject::OBJECT_STATE::INVISIBLE);
	//		});
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
				tState = WarriorMgr::WARRIOR_STATE::TAIL_BEAM_IDLE;
			});
	pWarrior->Animator3D()->EndEvent((UINT)WARRIOR_ANIMATION_TYPE::TAIL_BEAM_IDLE)
		= std::make_shared<std::function<void()>>([=]()
			{
				pTailBeam->SetObjectState(CGameObject::OBJECT_STATE::INVISIBLE);
				tState = WarriorMgr::WARRIOR_STATE::TAIL_BEAM_EXIT;
			});

	pWarrior->Animator3D()->EndEvent((UINT)WARRIOR_ANIMATION_TYPE::TAIL_BEAM_EXIT)
		= std::make_shared<std::function<void()>>([=]()
			{
				tState = WarriorMgr::WARRIOR_STATE::IDLE;
			});
	pWarrior->Animator3D()->StartEvent((UINT)WARRIOR_ANIMATION_TYPE::ROCK_THROW)
		= std::make_shared<std::function<void()>>([=]()
			{
				m_bRockThrow = false;
				m_bRockFollow = true;
			});
	pWarrior->Animator3D()->ProgressEvent((UINT)WARRIOR_ANIMATION_TYPE::ROCK_THROW)
		= std::make_shared<std::function<void()>>([=]()
			{
				m_bRockFollow = false;
			});
	pWarrior->Animator3D()->EndEvent((UINT)WARRIOR_ANIMATION_TYPE::ROCK_THROW)
		= std::make_shared<std::function<void()>>([=]()
			{
				tState = WarriorMgr::WARRIOR_STATE::IDLE;
			});
	pWarrior->Animator3D()->EndEvent((UINT)WARRIOR_ANIMATION_TYPE::DIE)
		= std::make_shared<std::function<void()>>([=]()
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
				if (pBreathCollisionPoint)
				{
					DestroyObject(pBreathCollisionPoint);
				}
				if (pTailBeamCollisionPoint)
				{
					DestroyObject(pTailBeamCollisionPoint);
				}
				if (pTailCollisionPoint)
				{
					DestroyObject(pTailCollisionPoint);
				}
				if (pHeadCollider)
				{
					DestroyObject(pHeadCollider);
				}
				if (pMouseCollider)
				{
					DestroyObject(pMouseCollider);
				}
				if (pChestCollider)
				{
					DestroyObject(pChestCollider);
				}
				if (pStomachCollider)
				{
					DestroyObject(pStomachCollider);
				}
				pWarrior->SetDead(true);
				DestroyObject(pWarrior);
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
		RaycastMgr::GetInst()->DoOneHitRaycast(rayInfo, &vPosition);
		Vec3 vCurPos = pBreath->Transform()->GetRelativePos();

		Vec3 vScale = pBreath->Transform()->GetRelativeScale();
		vScale.z = fabs(vPosition.z);
		pBreath->Transform()->SetRelativeScale(vScale);

		pBreathCollisionPoint->Collider3D()->SetColliderPos(vCurPos + (rayInfo.vDir * vPosition.z));
	}
}

void CWarriorScript::beamMove()
{
	CGameObject* pWarrior = GetOwner();
	if (nullptr != pTailBeam && pTailBeam->GetObjectState() == CGameObject::OBJECT_STATE::VISIBLE)
	{
		int _TailIdx = pWarrior->Animator3D()->GetTailIdx();
		Vec3 vPos = AnimationMgr::GetInst()->BonePos(_TailIdx, pWarrior);
		Vec3 vRot = AnimationMgr::GetInst()->BoneRot(_TailIdx, pWarrior);

		vRot.x += (-50.f * DegToRad());

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
		RaycastMgr::GetInst()->DoOneHitRaycast(rayInfo, &vPosition);
		Vec3 vCurPos = pTailBeam->Transform()->GetRelativePos();
		//Vec3 vLength = (*vPosition) - vCurPos;

		Vec3 vScale = pTailBeam->Transform()->GetRelativeScale();
		vScale.z = fabs(vPosition.z);
		pTailBeam->Transform()->SetRelativeScale(vScale);

		pTailBeamCollisionPoint->Collider3D()->SetColliderPos(vCurPos + (rayInfo.vDir * vPosition.z));
	}
}

void CWarriorScript::rockMove()
{
	CGameObject* pWarrior = GetOwner();
	if (m_bRockFollow)
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
				Vec3 vFront = pRock->Transform()->GetRelativeDir(DIR_TYPE::FRONT);
				if (pRock->RigidBody()->IsRigidBodyCreate())
				{
					pRock->RigidBody()->SetLinearVelocity(vFront * 5000.f);
				}
			}
			m_bRockThrow = true;
		}

	}

}

void CWarriorScript::tailMove()
{
	CGameObject* pWarrior = GetOwner();
	if (nullptr != pTailCollisionPoint)
	{
		int _TailIdx = pWarrior->Animator3D()->GetTailIdx();
		Vec3 vPos = AnimationMgr::GetInst()->BonePos(_TailIdx, pWarrior);

		pTailCollisionPoint->Collider3D()->SetColliderPos(vPos);
	}
}

void CWarriorScript::colliderMove()
{
	CGameObject* pWarrior = GetOwner();
	CAnimator3D* pAnimator = pWarrior->Animator3D();
	{
		int headIdx = pAnimator->GetHeadIdx();
		Vec3 vPos = AnimationMgr::GetInst()->BonePos(headIdx, pWarrior);
		Vec3 vRot = AnimationMgr::GetInst()->BoneRot(headIdx, pWarrior);

		pHeadCollider->Collider3D()->SetColliderPos(vPos);
		pHeadCollider->Collider3D()->SetColliderRot(vRot);
	}
	{
		int stomachIdx = pAnimator->GetStomachIdx();
		Vec3 vPos = AnimationMgr::GetInst()->BonePos(stomachIdx, pWarrior);
		Vec3 vRot = AnimationMgr::GetInst()->BoneRot(stomachIdx, pWarrior);

		pStomachCollider->Collider3D()->SetColliderPos(vPos);
		//pStomachCollider->Transform()->SetRelativeRot(vRot);
	}
	{
		int mouseIdx = pAnimator->GetMouseIdx();
		Vec3 vPos = AnimationMgr::GetInst()->BonePos(mouseIdx, pWarrior);
		Vec3 vRot = AnimationMgr::GetInst()->BoneRot(mouseIdx, pWarrior);

		pMouseCollider->Collider3D()->SetColliderPos(vPos);
		pMouseCollider->Collider3D()->SetColliderRot(vRot);
	}
	{
		int chestIdx = pAnimator->GetChestIdx();
		Vec3 vPos = AnimationMgr::GetInst()->BonePos(chestIdx, pWarrior);
		Vec3 vRot = AnimationMgr::GetInst()->BoneRot(chestIdx, pWarrior);

		pChestCollider->Collider3D()->SetColliderPos(vPos);
		pChestCollider->Collider3D()->SetColliderRot(vRot);
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
		pObj->MeshRender()->GetMaterial(0)->SetFlowDir(Vec2(-0.5f, 0.0f));

		//vRot.z = -50.f;
		//vRot.y = 0.f;
		//vRot.x = 0.f;
		pObj->Transform()->SetRelativeRot(vRot);
		SpawnGameObject(pObj, vPos, LAYER_TYPE::EnemyBullet);
		pBreath = pObj;
	}
	pBreath->SetObjectState(CGameObject::OBJECT_STATE::INVISIBLE);


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
		pObj->MeshRender()->GetMaterial(0)->SetFlowSpeed(2.f);
		pObj->MeshRender()->GetMaterial(0)->SetFlowDir(Vec2(-0.5f, 0.0f));

		vRot.z = -50.f;
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
		pObj->Collider3D()->SetScale(Vec3(1500.f, 1500.f, 1500.f));
		pObj->AddComponent(new CGizmo);

		pObj->SetName(L"fbx rock");
		pObj->MeshRender()->GetMaterial(0)->SetTexFlow(true);
		pObj->MeshRender()->GetMaterial(0)->SetFlowSpeed(2.f);
		pObj->MeshRender()->GetMaterial(0)->SetFlowDir(Vec2(-0.5f, 0.0f));

		SpawnGameObject(pObj, vPos, LAYER_TYPE::EnemyBullet);
		pRock = pObj;
	}
	pRock->SetObjectState(CGameObject::OBJECT_STATE::VISIBLE);
}

void CWarriorScript::tick()
{
	if (tState == WarriorMgr::WARRIOR_STATE::DIE)
		return;

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

	if (KEY_PRESSED(KEY::Q))
	{
		DoFarBreath();
	}
	if (KEY_PRESSED(KEY::E))
	{
		DoNearBreath();
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

	if (iWarriorHp == 0)
	{
		tState = WarriorMgr::WARRIOR_STATE::DIE;
	}
	if (tState == WarriorMgr::WARRIOR_STATE::DIE)
	{
		pWarrior->Animator3D()->Play((UINT)WARRIOR_ANIMATION_TYPE::DIE, false);
	}
	if (tState == WarriorMgr::WARRIOR_STATE::IDLE)
	{
		pWarrior->Animator3D()->Play((UINT)WARRIOR_ANIMATION_TYPE::IDLE, true);
	}
	if (tState == WarriorMgr::WARRIOR_STATE::TAIL_BEAM_IDLE)
	{
		GetOwner()->Animator3D()->Play((UINT)WARRIOR_ANIMATION_TYPE::TAIL_BEAM_IDLE, false, false);
	}
	if (tState == WarriorMgr::WARRIOR_STATE::TAIL_BEAM_EXIT)
	{
		GetOwner()->Animator3D()->Play((UINT)WARRIOR_ANIMATION_TYPE::TAIL_BEAM_EXIT, false, false);
	}

}

void CWarriorScript::collision()
{

}

void CWarriorScript::finaltick()
{
	//CGameObject* pWarrior = GetOwner();
	//static bool binit = false;
	//if (!binit)
	//{
	//	if (GetOwner()->MeshRender()->GetMaterial(3) != nullptr)
	//	{
	//		GetOwner()->MeshRender()->GetMaterial(3)->SetTexFlow(true);
	//	}

	//	makeAttackObject();
	//	makeCollider();
	//	binit = true;
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


	//beamMove();
	//breathMove();
	//rockMove();
	//colliderMove();

	//if (iWarriorHp == 0)
	//{
	//	tState = WarriorMgr::WARRIOR_STATE::DIE;
	//}
	//if (tState == WarriorMgr::WARRIOR_STATE::DIE)
	//{
	//	pWarrior->Animator3D()->Play((UINT)WARRIOR_ANIMATION_TYPE::DIE, false);
	//}
	//if (tState == WarriorMgr::WARRIOR_STATE::IDLE)
	//{
	//	pWarrior->Animator3D()->Play((UINT)WARRIOR_ANIMATION_TYPE::IDLE, true);
	//}
	//if (tState == WarriorMgr::WARRIOR_STATE::TAIL_BEAM_IDLE)
	//{
	//	GetOwner()->Animator3D()->Play((UINT)WARRIOR_ANIMATION_TYPE::TAIL_BEAM_IDLE, false, false);
	//}
	//if (tState == WarriorMgr::WARRIOR_STATE::TAIL_BEAM_EXIT)
	//{
	//	GetOwner()->Animator3D()->Play((UINT)WARRIOR_ANIMATION_TYPE::TAIL_BEAM_EXIT, false, false);
	//}
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
	//pBreathCollisionPoint;
	//pTailBeamCollisionPoint;
	//pTailBeam;
	//pBreath;
}
