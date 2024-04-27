#include "pch.h"
#include "CConstructorScript.h"

#include <Engine/PlayerMgr.h>
#include <Engine/AnimationMgr.h>
#include <Engine/RaycastMgr.h>
#include <Engine/CRenderMgr.h>
#include <Engine/CUI.h>
#include <Engine/FieldUIMgr.h>

#include "CAttackNormalScript.h"
#include "CPlayerScript.h"

CConstructorScript::CConstructorScript()
	: CScript((UINT)CONSTRUCTOR_SCRIPT)
	, fRotateSpeed(2.f)
	, tState(CONSTRUCTOR_STATE::HIDE)
	, iConsHp(500)
	, iConsHpCapa(500)
	, bSmash(false)
{
}

CConstructorScript::~CConstructorScript()
{
}

void CConstructorScript::initAnim()
{
	CGameObject* pCons = GetOwner();

	pCons->Animator3D()->SetAnimClipEventIdx(
		(UINT)CONSTRUCTOR_ANIMATION_TYPE::SMASH_GROUND, -1, -1, 78, 40);

	pCons->Animator3D()->SetAnimClipEventIdx(
		(UINT)CONSTRUCTOR_ANIMATION_TYPE::GROUND_TO_SKY, -1, -1, 40, -1);


	pCons->Animator3D()->EndEvent((UINT)CONSTRUCTOR_ANIMATION_TYPE::BEAM_ENTER)
		= std::make_shared<std::function<void()>>([=]()
			{
				//SoundMgr::GetInst()->Play(wsBeamEnterSound, pCons->Transform()->GetRelativePos(), 0, 10.f, SoundMgr::SOUND_TYPE::SFX, 0.8f, false);
				pBeam->SetObjectState(CGameObject::OBJECT_STATE::VISIBLE);
				pBeamCollider->SetObjectState(CGameObject::OBJECT_STATE::VISIBLE);
				tState = CONSTRUCTOR_STATE::BEAM_LOOP;
			});
	pCons->Animator3D()->EndEvent((UINT)CONSTRUCTOR_ANIMATION_TYPE::BEAM_LOOP)
		= std::make_shared<std::function<void()>>([=]()
			{
				//SoundMgr::GetInst()->Stop(wsBeamIdleSound, SoundMgr::SOUND_TYPE::SFX);
				tState = CONSTRUCTOR_STATE::BEAM_EXIT;
			});
	pCons->Animator3D()->StartEvent((UINT)CONSTRUCTOR_ANIMATION_TYPE::BEAM_EXIT)
		= std::make_shared<std::function<void()>>([=]()
			{
				SoundMgr::GetInst()->Stop(wsBeamIdleSound, SoundMgr::SOUND_TYPE::SFX);
	pBeam->SetObjectState(CGameObject::OBJECT_STATE::INVISIBLE);
	pBeamCollider->SetObjectState(CGameObject::OBJECT_STATE::INVISIBLE);
			});
	pCons->Animator3D()->EndEvent((UINT)CONSTRUCTOR_ANIMATION_TYPE::BEAM_EXIT)
		= std::make_shared<std::function<void()>>([=]()
			{
				tState = CONSTRUCTOR_STATE::IDLE;
			});

	pCons->Animator3D()->EndEvent((UINT)CONSTRUCTOR_ANIMATION_TYPE::SMASH_GROUND)
		= std::make_shared<std::function<void()>>([=]()
			{
				tState = CONSTRUCTOR_STATE::GROUND_TO_SKY;
			});

	pCons->Animator3D()->CompleteEvent((UINT)CONSTRUCTOR_ANIMATION_TYPE::SMASH_GROUND)
		= std::make_shared<std::function<void()>>([=]()
			{
				SoundMgr::GetInst()->Play(wsLegSound, pCons->Transform()->GetRelativePos(), 0, 10.f, SoundMgr::SOUND_TYPE::SFX, 1.0f, false);
			});
	pCons->Animator3D()->ProgressEvent((UINT)CONSTRUCTOR_ANIMATION_TYPE::SMASH_GROUND)
		= std::make_shared<std::function<void()>>([=]()
			{
				//tState = CONSTRUCTOR_STATE::GROUND_TO_SKY;
				SoundMgr::GetInst()->Play(wsToGroundSound, pCons->Transform()->GetRelativePos(), 0, 10.f, SoundMgr::SOUND_TYPE::SFX, 1.0f, false);
	pSmashEffect->SetObjectState(CGameObject::OBJECT_STATE::VISIBLE);
	bSmash = true;
			});
	pCons->Animator3D()->EndEvent((UINT)CONSTRUCTOR_ANIMATION_TYPE::GROUND_TO_SKY)
		= std::make_shared<std::function<void()>>([=]()
			{
				tState = CONSTRUCTOR_STATE::IDLE;
	pSmashCollider->SetObjectState(CGameObject::OBJECT_STATE::INVISIBLE);
			});
	pCons->Animator3D()->ProgressEvent((UINT)CONSTRUCTOR_ANIMATION_TYPE::GROUND_TO_SKY)
		= std::make_shared<std::function<void()>>([=]()
			{
				SoundMgr::GetInst()->Play(wsLegSound, pCons->Transform()->GetRelativePos(), 0, 10.f, SoundMgr::SOUND_TYPE::SFX, 1.0f, false);
			});
	pCons->Animator3D()->EndEvent((UINT)CONSTRUCTOR_ANIMATION_TYPE::DIE)
		= std::make_shared<std::function<void()>>([=]()
			{
			});

	pCons->Animator3D()->EndEvent((UINT)CONSTRUCTOR_ANIMATION_TYPE::APPEAR)
		= std::make_shared<std::function<void()>>([=]()
			{
				tState = CONSTRUCTOR_STATE::IDLE;
			});
}

void CConstructorScript::createUI()
{
	Ptr<CGraphicsShader> UIShader = (CGraphicsShader*)CResMgr::GetInst()->FindRes<CGraphicsShader>(L"UI2DShader").Get();
	Ptr<CGraphicsShader> adjustUIShader = (CGraphicsShader*)CResMgr::GetInst()->FindRes<CGraphicsShader>(L"AdjustUI2DShader").Get();
	Ptr<CMaterial> pMtrl = nullptr;
	{
		pMtrl = new CMaterial(true);
		pMtrl->SetShader(adjustUIShader);
		CResMgr::GetInst()->AddRes(L"WarriorHpBackMtrl", pMtrl);

		Ptr<CTexture> pTex = CResMgr::GetInst()->FindRes<CTexture>(L"texture\\UI\\boss_hp_back.png");
		pUI_ConsHPBack = new CGameObject;

		pUI_ConsHPBack->SetName(L"UI Warrior Hp Back");
		pUI_ConsHPBack->AddComponent(new CTransform);
		pUI_ConsHPBack->AddComponent(new CMeshRender);

		pUI_ConsHPBack->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
		pUI_ConsHPBack->MeshRender()->SetMaterial(pMtrl, 0);
		pUI_ConsHPBack->MeshRender()->GetMaterial(0)->SetTexParam(TEX_0, pTex);
		pUI_ConsHPBack->Transform()->SetRelativeScale(Vec3(700.f, 16.f, 1.f));
		SpawnGameObject(pUI_ConsHPBack, Vec3(0.f, 320.f, 0.f), LAYER_TYPE::ViewPortUI);

		pUI_ConsHPBack->SetObjectState(CGameObject::OBJECT_STATE::INVISIBLE);
	}

	{
		pMtrl = new CMaterial(true);
		pMtrl->SetShader(adjustUIShader);
		CResMgr::GetInst()->AddRes(L"WarriorHpFrontMtrl", pMtrl);

		Ptr<CTexture> pTex = CResMgr::GetInst()->FindRes<CTexture>(L"texture\\UI\\boss_hp_front.png");
		pUI_ConsHP = new CGameObject;

		pUI_ConsHP->SetName(L"UI Warrior Hp");
		pUI_ConsHP->AddComponent(new CTransform);
		pUI_ConsHP->AddComponent(new CMeshRender);

		pUI_ConsHP->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
		pUI_ConsHP->MeshRender()->SetMaterial(pMtrl, 0);
		pUI_ConsHP->MeshRender()->GetMaterial(0)->SetTexParam(TEX_0, pTex);
		pUI_ConsHP->Transform()->SetRelativeScale(Vec3(695.f, 14.f, 1.f));
		SpawnGameObject(pUI_ConsHP, Vec3(0.f, 320.f, 0.f), LAYER_TYPE::ViewPortUI);

		pUI_ConsHP->SetObjectState(CGameObject::OBJECT_STATE::INVISIBLE);
	}

	pConsText = new CUI();
	pConsText->SetName(L"Constructor Text");
	pConsText->AddComponent(new CTransform);
	pConsText->AddComponent(new CMeshRender);
	pConsText->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	pConsText->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"AdjustUI2DShaderMtrl"), 0);
	pConsText->Transform()->SetRelativeScale(Vec3(100.f, 100.f, 1.f));
	pConsText->SetTextScale(0.6f);
	pConsText->SetText(L"77 Badass Constructor");
	pConsText->SetOutline(true);
	pConsText->SetTextNormalColor(Vec4(1.f, 1.f, 1.f, 1.f));
	SpawnGameObject(pConsText, Vec3(-150.f, 280.f, 1.f), LAYER_TYPE::ViewPortUI);

	pConsText->SetObjectState(CGameObject::OBJECT_STATE::INVISIBLE);
}

void CConstructorScript::createCollider()
{
	{
		CGameObject* pObj = new CGameObject;
		pObj->AddComponent(new CCollider3D(false, true));
		pObj->Collider3D()->SetUnityOwner(GetOwner());
		pObj->Collider3D()->SetScale(Vec3(500.f, 500.f, 500.f));
		pObj->AddComponent(new CGizmo(true));
		pObj->AddComponent(new CAttackNormalScript);

		pObj->SetName(L"fbx beam collider");

		SpawnGameObject(pObj, Vec3(0.f, 100.f, 0.f), LAYER_TYPE::NoRaycastingCollider, true);
		pBeamCollider = pObj;
	}
	{
		CGameObject* pObj = new CGameObject;
		pObj->AddComponent(new CCollider3D(false, true));
		pObj->Collider3D()->SetUnityOwner(GetOwner());
		pObj->Collider3D()->SetScale(Vec3(5000.f, 5000.f, 5000.f));
		pObj->AddComponent(new CGizmo(true));

		pObj->SetName(L"fbx constructor attack boundary collider");

		SpawnGameObject(pObj, Vec3(9910.992, 102.368, 4333.719), LAYER_TYPE::NoRaycastingCollider, true);
		pAttackBoundCollider = pObj;
	}
	{
		CGameObject* pObj = new CGameObject;
		pObj->AddComponent(new CCollider3D(false, true));
		pObj->Collider3D()->SetUnityOwner(GetOwner());
		pObj->Collider3D()->SetScale(Vec3(1000.f, 1000.f, 1000.f));
		pObj->AddComponent(new CGizmo(true));
		pObj->AddComponent(new CAttackNormalScript);

		pObj->SetName(L"fbx smash attack collider");

		SpawnGameObject(pObj, Vec3(0.f, 0.f, 0.f), LAYER_TYPE::NoRaycastingCollider, true);
		pSmashCollider = pObj;
	}
	{
		CGameObject* pObj = new CGameObject;
		pObj->AddComponent(new CCollider3D(false, true));
		pObj->Collider3D()->SetUnityOwner(GetOwner());
		pObj->Collider3D()->SetScale(Vec3(500.f, 500.f, 500.f));

		pObj->SetName(L"fbx constructor eye collider");

		SpawnGameObject(pObj, Vec3(0.f, 100.f, 0.f), LAYER_TYPE::Enemy, true);
		pEyeCollider = pObj;
	}
}

void CConstructorScript::createObject()
{
	CGameObject* pCons = GetOwner();
	int iEyeIdx = pCons->Animator3D()->GetEyeIdx();
	Vec3 vPos = AnimationMgr::GetInst()->BonePos(iEyeIdx, pCons);
	Vec3 vRot = AnimationMgr::GetInst()->BoneRot(iEyeIdx, pCons);
	{
		Ptr<CMeshData> pMeshData = nullptr;
		CGameObject* pObj = nullptr;
		pMeshData = CResMgr::GetInst()->LoadFBX(L"fbx\\tail_beam.fbx");
		pObj = pMeshData->Instantiate(Vec3(1000.f, 1000.f, 3000.f));
		//pObj->AddComponent(new CCollider3D(false));
		//pObj->Collider3D()->SetScale(Vec3(1500.f, 1500.f, 10000.f));
		//pObj->Collider3D()->SetCenter(true);
		pObj->AddComponent(new CGizmo);

		pObj->SetName(L"fbx beam");
		pObj->MeshRender()->GetMaterial(0)->SetTexFlow(true);
		pObj->MeshRender()->GetMaterial(0)->SetFlowSpeed(-2.f);
		pObj->MeshRender()->GetMaterial(0)->SetFlowDir(Vec2(2.f, 0.0f));

		//vRot.z = -50.f;
		vRot.z = (-10.f * DegToRad());
		//vRot.y = 0.f;
		//vRot.x = 0.f;
		pObj->Transform()->SetRelativeRot(vRot);
		SpawnGameObject(pObj, vPos, LAYER_TYPE::EnemyBullet);
		pBeam = pObj;
	}
	pBeam->SetObjectState(CGameObject::OBJECT_STATE::INVISIBLE);

	{
		Ptr<CMeshData> pMeshData = nullptr;
		CGameObject* pObj = nullptr;
		pMeshData = CResMgr::GetInst()->LoadFBX(L"fbx\\circle_impact.fbx");
		pObj = pMeshData->Instantiate(Vec3(200.f, 150.f, 200.f));
		//pObj->AddComponent(new CCollider3D(false));
		//pObj->Collider3D()->SetScale(Vec3(1500.f, 1500.f, 10000.f));
		//pObj->Collider3D()->SetCenter(true);
		//pObj->AddComponent(new CAttackNormalScript);
		pObj->AddComponent(new CGizmo);
		//vPos.y = 150.f;
		pObj->SetName(L"fbx smash effect");
		SpawnGameObject(pObj, vPos, LAYER_TYPE::Environment);
		bool t = true;
		pObj->MeshRender()->GetMaterial(0)->SetScalarParam(INT_5, &t);
		pObj->MeshRender()->GetMaterial(0)->SetFlow(0, true);
		pObj->MeshRender()->GetMaterial(0)->SetFlowDir(Vec2(0.f, 0.3f));
		pObj->MeshRender()->GetMaterial(0)->SetTexFlow(true);
		pObj->MeshRender()->GetMaterial(0)->SetFlowSpeed(-1.f);
		float fAlpha = 0.0f;
		pObj->MeshRender()->GetMaterial(0)->SetScalarParam(FLOAT_1, &fAlpha);

		pSmashEffect = pObj;
	}
}

void CConstructorScript::begin()
{
	initAnim();
	createUI();
	createCollider();
	createObject();
}

void CConstructorScript::Raycast(tRayInfo _RaycastInfo)
{
	if ((UINT)RAYCAST_TYPE::SHOOT != _RaycastInfo.tRayType
		|| _RaycastInfo.fDamage == 0)
		return;

	Attacked(_RaycastInfo.fDamage);

	FieldUIMgr::GetInst()->AddDamage(_RaycastInfo.fDamage, _RaycastInfo.vStart + (_RaycastInfo.vDir * _RaycastInfo.fDist));
}

void CConstructorScript::Attacked(int _Damage)
{
	if (iConsHp < _Damage)
		iConsHp = 0;
	else
		iConsHp -= _Damage;
}

void CConstructorScript::DoBeam()
{
	if (tState == CONSTRUCTOR_STATE::BEAM_ENTER ||
		tState == CONSTRUCTOR_STATE::BEAM_EXIT ||
		tState == CONSTRUCTOR_STATE::BEAM_LOOP)
		return;

	tState = CONSTRUCTOR_STATE::BEAM_ENTER;

	CGameObject* pCons = GetOwner();
	pCons->Animator3D()->Play((UINT)CONSTRUCTOR_ANIMATION_TYPE::BEAM_ENTER, false);
}

void CConstructorScript::DoSmash()
{
	if (tState == CONSTRUCTOR_STATE::SMASH_GROUND ||
		tState == CONSTRUCTOR_STATE::GROUND_TO_SKY)
		return;

	tState = CONSTRUCTOR_STATE::SMASH_GROUND;

	CGameObject* pCons = GetOwner();

	Vec3 vPos = pCons->Transform()->GetRelativePos();
	pSmashCollider->Collider3D()->SetColliderPos(vPos);
	vPos.y += 50.f;
	pSmashEffect->Transform()->SetRelativePos(vPos);
	pSmashCollider->SetObjectState(CGameObject::OBJECT_STATE::VISIBLE);

	pCons->Animator3D()->Play((UINT)CONSTRUCTOR_ANIMATION_TYPE::SMASH_GROUND, false);
}

void CConstructorScript::IsDie()
{
	CGameObject* pCons = GetOwner();
	if (pCons->MeshRender()->GetMaterial(0)->IsPaperBurnEnd())
	{
		if (pBeam)
			DestroyObject(pBeam);
		if (pBeamCollider)
			DestroyObject(pBeamCollider);
		if (pSmashCollider)
			DestroyObject(pSmashCollider);
		if (pAttackBoundCollider)
			DestroyObject(pAttackBoundCollider);
		if (pUI_ConsHPBack)
			DestroyObject(pUI_ConsHPBack);
		if (pUI_ConsHP)
			DestroyObject(pUI_ConsHP);
		if (pSmashEffect)
			DestroyObject(pSmashEffect);
		if (pEyeCollider)
			DestroyObject(pEyeCollider);
		if (pConsText)
			DestroyObject(pConsText);

		DestroyObject(GetOwner());
		CLevelMgr::GetInst()->GetCurLevel()->SetLevelEnd(true);
	}
}

void CConstructorScript::beamMove()
{
	CGameObject* pCons = GetOwner();
	if (nullptr != pBeam && pBeam->GetObjectState() == CGameObject::OBJECT_STATE::VISIBLE)
	{
		//Vec3 vConUp = pCons->Transform()->GetRelativeDir(DIR_TYPE::UP);
		//Vec3 vRot = pBeam->Transform()->GetRelativeRot();
		//int _EyeIdx = pCons->Animator3D()->GetEyeIdx();
		//Vec3 vPos = AnimationMgr::GetInst()->BonePos(_EyeIdx, pCons);

		PxVec3 eyePos = pEyeCollider->Collider3D()->GetColliderPos().p;
		Vec3 vEyePos = Vec3(eyePos.x, eyePos.y, eyePos.z);

		Vec3 vPlayerPos = PlayerMgr::GetInst()->GetPlayer()->Transform()->GetRelativePos();

		Vec3 vUp = pBeam->Transform()->GetRelativeDir(DIR_TYPE::UP);
		Vec3 vFront = pBeam->Transform()->GetRelativeDir(DIR_TYPE::FRONT);
		Vec3 vDiff = vPlayerPos - vEyePos;
		vDiff.Normalize();

		Matrix rotMat = XMMatrixLookToLH(vEyePos, vDiff, vUp);

		Quat quat; Vec3 vS, vT;
		rotMat.Decompose(vS, quat, vT);

		Vec3 vRot = physx::Util::QuaternionToVector3(quat);

		pBeam->Transform()->SetRelativePos(vEyePos);
		pBeam->Transform()->SetRelativeRot(vRot);
		rotMat.Forward(vFront);
		vFront.Normalize();

		tRayInfo rayInfo{};
		rayInfo.fDamage = 10.f;
		rayInfo.iLayerIdx = (int)LAYER_TYPE::EnemyBullet;
		rayInfo.vDir = vFront;
		rayInfo.vStart = pBeam->Transform()->GetRelativePos();
		rayInfo.matWorld = pBeam->Transform()->GetDrawRayMat();
		rayInfo.tRayType = (UINT)RAYCAST_TYPE::SHOOT;

		Vec3 vPosition;
		RaycastMgr::GetInst()->DoOneHitRaycast(rayInfo, &vPosition, RAYCAST_GROUP_TYPE::Enemy);
		Vec3 vCurPos = vEyePos;
		//Vec3 vLength = (*vPosition) - vCurPos;

		Vec3 vScale = pBeam->Transform()->GetRelativeScale();
		vScale.z = fabs(vPosition.z);
		vScale.z /= 1.5f;
		pBeam->Transform()->SetRelativeScale(vScale);
		if (!SoundMgr::GetInst()->IsPlaying(wsBeamIdleSound, SoundMgr::SOUND_TYPE::SFX))
			SoundMgr::GetInst()->Play(wsBeamIdleSound, vCurPos + (rayInfo.vDir * vPosition.z), -1, 10.f, SoundMgr::SOUND_TYPE::SFX, 0.8f, false);
		else
			SoundMgr::GetInst()->SetSoundPos(wsBeamIdleSound, SoundMgr::SOUND_TYPE::SFX, vCurPos + (rayInfo.vDir * vPosition.z));

		pBeamCollider->Collider3D()->SetColliderPos(vCurPos + (rayInfo.vDir * vPosition.z));
	}
}

void CConstructorScript::eyeMove()
{
	CGameObject* pCons = GetOwner();
	CAnimator3D* pAnimator = pCons->Animator3D();
	{
		if (pEyeCollider)
		{
			int eyeIdx = pAnimator->GetEyeIdx();
			Vec3 vPos = AnimationMgr::GetInst()->BonePos(eyeIdx, pCons);
			Vec3 vRot = AnimationMgr::GetInst()->BoneRot(eyeIdx, pCons);

			pEyeCollider->Collider3D()->SetColliderPos(vPos);
			pEyeCollider->Collider3D()->SetColliderRot(vRot);
		}
	}
}

void CConstructorScript::paperBurn()
{
	if (tState == CONSTRUCTOR_STATE::DIE)
		return;

	PlayerMgr::GetInst()->GetPlayer()->GetScript<CPlayerScript>()->AddExp(1);

	CGameObject* pCons = GetOwner();
	SoundMgr::GetInst()->Play(wsDieSound, pCons->Transform()->GetRelativePos(), 2, 1.f, SoundMgr::SOUND_TYPE::SFX, 0.5f, false);
	double dTime = AnimationMgr::GetInst()->GetCurAnimationTime(pCons->Animator3D());
	pCons->MeshRender()->SetPaparBurn(true, (float)dTime);
}

void CConstructorScript::smashEffect()
{
	if (!bSmash)
		return;

	static float fsmashAcc = 0.0f;

	fsmashAcc += DT;

	float fAlpha = 1.f - (fsmashAcc / 2.f);

	if (fsmashAcc > 2.f)
	{
		bSmash = false;
		fsmashAcc = 0.0f;
		pSmashEffect->Transform()->SetRelativeScale(Vec3(200.f, 150.f, 200.f));
		pSmashCollider->Collider3D()->SetScale(Vec3(200.f, 150.f, 200.f));

		pSmashEffect->SetObjectState(CGameObject::OBJECT_STATE::INVISIBLE);
		pSmashCollider->SetObjectState(CGameObject::OBJECT_STATE::INVISIBLE);
	}
	else
	{
		pSmashEffect->SetObjectState(CGameObject::OBJECT_STATE::VISIBLE);
		pSmashCollider->SetObjectState(CGameObject::OBJECT_STATE::VISIBLE);

		pSmashEffect->MeshRender()->GetMaterial(0)->SetScalarParam(FLOAT_1, &fAlpha);
		Vec3 vScale = pSmashEffect->Transform()->GetRelativeScale();

		vScale.x += 20.f;
		vScale.z += 20.f;
		vScale.y += 5.f;

		pSmashEffect->Transform()->SetRelativeScale(vScale);

		vScale.x += 20.f;
		vScale.z += 20.f;
		vScale.y += 5.f;

		pSmashCollider->Collider3D()->SetScale(vScale);
	}
}

float CConstructorScript::GetHpRatio()
{
	return (float)iConsHp / (float)iConsHpCapa;
}

bool CConstructorScript::Rotate()
{
	if (tState == CONSTRUCTOR_STATE::DIE)
		return false;

	CGameObject* pPlayer = PlayerMgr::GetInst()->GetPlayer();
	if (nullptr == pPlayer) return false;

	CGameObject* pCons = GetOwner();
	Vec3 vRot = pCons->Transform()->GetRelativeRot();

	Vec3 vPlayerPos = pPlayer->Transform()->GetRelativePos();
	Vec3 vConsPos = pCons->Transform()->GetRelativePos();

	Vec3 vConsRight = pCons->Transform()->GetRelativeDir(DIR_TYPE::RIGHT);

	Vec3 vDir = vPlayerPos - vConsPos;
	vDir.Normalize();
	vConsRight.Normalize();

	// 벡터의 내적은 90도 일때 0 Dot(A,B)=∣A∣⋅∣B∣⋅cos(θ)
	float includeAngle = vDir.Dot(vConsRight);
	vRot.y += includeAngle * DT * fRotateSpeed;
	pCons->Transform()->SetRelativeRot(vRot);

	if (areAlmostEqual(includeAngle, 0.0f, 0.01f))
	{
		return true;
	}
	return false;
}

void CConstructorScript::tick()
{
	if (tState == CONSTRUCTOR_STATE::DIE)
	{
		IsDie();
		return;
	}


	if (CRenderMgr::GetInst()->GetMainCam() && CRenderMgr::GetInst()->GetMainCam()->IsCinematic())
	{
		pUI_ConsHP->SetObjectState(CGameObject::OBJECT_STATE::INVISIBLE);
		pUI_ConsHPBack->SetObjectState(CGameObject::OBJECT_STATE::INVISIBLE);
		pConsText->SetObjectState(CGameObject::OBJECT_STATE::INVISIBLE);
	}
	//if(CONSTRUCTOR_STATE::HIDE == tState)
	//{
	//	Vec3 vPos = GetOwner()->Transform()->GetRelativePos();
	//	vPos.y -=
	//}

	float ratio = 1.f - GetHpRatio();
	pUI_ConsHP->MeshRender()->GetMaterial(0)->SetScalarParam(FLOAT_0, &ratio);


	CGameObject* pCons = GetOwner();
	fActAcc += DT;

	if (tState == CONSTRUCTOR_STATE::IDLE)
	{
		if (CRenderMgr::GetInst()->GetMainCam() && CRenderMgr::GetInst()->GetMainCam()->IsCinematic())
			CRenderMgr::GetInst()->GetMainCam()->SetCinematic(false);

		if (pUI_ConsHP->GetObjectState() == CGameObject::OBJECT_STATE::INVISIBLE)
			pUI_ConsHP->SetObjectState(CGameObject::OBJECT_STATE::VISIBLE);

		if (pUI_ConsHPBack->GetObjectState() == CGameObject::OBJECT_STATE::INVISIBLE)
			pUI_ConsHPBack->SetObjectState(CGameObject::OBJECT_STATE::VISIBLE);

		if (pConsText->GetObjectState() == CGameObject::OBJECT_STATE::INVISIBLE)
			pConsText->SetObjectState(CGameObject::OBJECT_STATE::VISIBLE);

		int randStart = rand() % 20;
		if (ceil(fActAcc) < 20)
		{
			fActAcc = 0.0f;
		}
		if (randStart == ceil(fActAcc))
		{
			int iRandBehavior = rand() % 4;

			switch (iRandBehavior)
			{
			case 2:
			{
				tState = CONSTRUCTOR_STATE::BEAM;
			}
			break;
			case 1:
			{
				DoSmash();
			}
			break;
			}
		}
	}
	else
	{
		fActAcc = 0.0f;
	}

	if (tState == CONSTRUCTOR_STATE::BEAM)
	{
		if (Rotate())
			DoBeam();
	}
	if (bSmash)
	{
		smashEffect();
	}

	//if (tState == CONSTRUCTOR_STATE::BEAM_LOOP)
	//{
	//	beamMove();
	//}

	eyeMove();
	beamMove();

	if (iConsHp == 0)
	{
		pCons->Animator3D()->Play((UINT)CONSTRUCTOR_ANIMATION_TYPE::DIE, false, false);
		paperBurn();
		tState = CONSTRUCTOR_STATE::DIE;
	}

	if (pAttackBoundCollider->Collider3D()->IsColliderRigidCreate())
	{
		pAttackBoundCollider->Collider3D()->SetColliderPos(Vec3(9910.992, 200.368, 4333.719));
	}

	if (pAttackBoundCollider->Collider3D()->IsBeginOverlap() || pAttackBoundCollider->Collider3D()->IsOnOverlap())
	{
		if (tState == CONSTRUCTOR_STATE::HIDE)
		{
			CRenderMgr::GetInst()->GetMainCam()->Transform()->SetRelativePos(Vec3(8738.374, 1260.889, 3812.486));
			CRenderMgr::GetInst()->GetMainCam()->SetCinematic(true);
			tState = CONSTRUCTOR_STATE::APPEAR;
			pUI_ConsHP->SetObjectState(CGameObject::OBJECT_STATE::VISIBLE);
			pUI_ConsHPBack->SetObjectState(CGameObject::OBJECT_STATE::VISIBLE);
			pConsText->SetObjectState(CGameObject::OBJECT_STATE::VISIBLE);
		}
	}

	if (tState == CONSTRUCTOR_STATE::APPEAR)
	{
		//152.368
		if (GetOwner()->Transform()->GetRelativePos().y != 152.368)
		{
			Vec3 vv = GetOwner()->Transform()->GetRelativePos();
			vv.y = 152.368;
			GetOwner()->Transform()->SetRelativePos(vv);
		}
		if (GetOwner()->Animator3D()->GetCurAnimClip()->GetClipIdx() == 10)
		{
			SoundMgr::GetInst()->Play(wsAppearSound, pCons->Transform()->GetRelativePos(), 0, 10.f, SoundMgr::SOUND_TYPE::SFX, 1.0f, false);
		}
		if (GetOwner()->Animator3D()->GetCurAnimClip()->GetClipIdx() == 60)
		{
			SoundMgr::GetInst()->Play(wsLegSound, pCons->Transform()->GetRelativePos(), 0, 10.f, SoundMgr::SOUND_TYPE::SFX, 1.0f, false);
		}
		if (GetOwner()->Animator3D()->GetCurAnimClip()->GetClipIdx() == 88)
		{
			SoundMgr::GetInst()->Play(wsToGroundSound, pCons->Transform()->GetRelativePos(), 0, 10.f, SoundMgr::SOUND_TYPE::SFX, 1.0f, false);
			SoundMgr::GetInst()->Play(wsVoiceSound1, pCons->Transform()->GetRelativePos(), 0, 10.f, SoundMgr::SOUND_TYPE::SFX, 1.0f, false);
		}

		if (GetOwner()->Animator3D()->GetCurAnimClip()->GetClipIdx() == 161)
		{
			SoundMgr::GetInst()->Play(wsLegSound, pCons->Transform()->GetRelativePos(), 0, 10.f, SoundMgr::SOUND_TYPE::SFX, 1.0f, true);
		}

		if (GetOwner()->Animator3D()->GetCurAnimClip()->GetClipIdx() > 43)
		{
			CRenderMgr::GetInst()->GetMainCam()->Transform()->SetRelativePos(Vec3(9207.647, 806.600, 4937.926));
		}
		if (GetOwner()->Animator3D()->GetCurAnimClip()->GetClipIdx() > 90)
		{
			CRenderMgr::GetInst()->GetMainCam()->Transform()->SetRelativePos(Vec3(9180.432, 491.535, 4348.623));
		}

		Vec3 vPos = CRenderMgr::GetInst()->GetMainCam()->GetOwner()->Transform()->GetRelativePos();
		Vec3 vUp = CRenderMgr::GetInst()->GetMainCam()->GetOwner()->Transform()->GetRelativeDir(DIR_TYPE::UP);

		PxVec3 eyePos = pEyeCollider->Collider3D()->GetColliderPos().p;
		Vec3 vEyePos = Vec3(eyePos.x, eyePos.y, eyePos.z);
		//Vec3 vConsPos = pCons->Transform()->GetRelativePos();

		Vec3 vDiff = vEyePos - vPos;
		vDiff.Normalize();

		Matrix rotMat = XMMatrixLookToLH(vPos, vDiff, vUp);

		Quat quat;
		Vec3 vS, vT;
		rotMat.Decompose(vS, quat, vT);

		Vec3 vRot = physx::Util::QuaternionToVector3(quat);

		CRenderMgr::GetInst()->GetMainCam()->GetOwner()->Transform()->SetRelativeRot(Vec3(vRot.z, -vRot.y, 0.f));

		GetOwner()->Animator3D()->Play((UINT)CONSTRUCTOR_ANIMATION_TYPE::APPEAR, false, false);
	}


	if (tState == CONSTRUCTOR_STATE::IDLE)
	{
		pCons->Animator3D()->Play((UINT)CONSTRUCTOR_ANIMATION_TYPE::IDLE, true);
	}
	if (tState == CONSTRUCTOR_STATE::BEAM_LOOP)
	{
		pCons->Animator3D()->Play((UINT)CONSTRUCTOR_ANIMATION_TYPE::BEAM_LOOP, false, false);
	}
	if (tState == CONSTRUCTOR_STATE::BEAM_EXIT)
	{
		pCons->Animator3D()->Play((UINT)CONSTRUCTOR_ANIMATION_TYPE::BEAM_EXIT, false, false);
	}
	if (tState == CONSTRUCTOR_STATE::GROUND_TO_SKY)
	{
		pCons->Animator3D()->Play((UINT)CONSTRUCTOR_ANIMATION_TYPE::GROUND_TO_SKY, false, false);
	}
}

void CConstructorScript::finaltick()
{
	//if (tState == CONSTRUCTOR_STATE::DIE)
	//{
	//	IsDie();
	//	return;
	//}

	//CGameObject* pCons = GetOwner();

	//if (KEY_TAP(KEY::Z))
	//{
	//	initAnim();
	//	createCollider();
	//	createObject();
	//}
	//if (KEY_PRESSED(KEY::Q))
	//{
	//	tState = CONSTRUCTOR_STATE::BEAM;
	//	//DoBeam();
	//}
	//if (KEY_PRESSED(KEY::R))
	//{
	//	DoSmash();
	//}

	//if (KEY_TAP(KEY::T))
	//{
	//	CRenderMgr::GetInst()->GetMainCam()->SetCinematic(true);
	//	tState = CONSTRUCTOR_STATE::APPEAR;
	//}

	//eyeMove();

	//if (bSmash)
	//{
	//	smashEffect();
	//}

	//if (tState == CONSTRUCTOR_STATE::BEAM && Rotate())
	//{
	//	DoBeam();
	//}

	//if (tState == CONSTRUCTOR_STATE::BEAM_LOOP ||
	//	tState == CONSTRUCTOR_STATE::BEAM_ENTER ||
	//	tState == CONSTRUCTOR_STATE::BEAM_EXIT)
	//{
	//	if (Rotate())
	//	{
	//		beamMove();
	//	}
	//}

	//if (iConsHp == 0)
	//{
	//	pCons->Animator3D()->Play((UINT)CONSTRUCTOR_ANIMATION_TYPE::DIE, false, false);
	//	paperBurn();
	//	tState = CONSTRUCTOR_STATE::DIE;
	//}

	//if (tState == CONSTRUCTOR_STATE::APPEAR)
	//{

	//	Vec3 vPos = CRenderMgr::GetInst()->GetMainCam()->GetOwner()->Transform()->GetRelativePos();
	//	Vec3 vUp = CRenderMgr::GetInst()->GetMainCam()->GetOwner()->Transform()->GetRelativeDir(DIR_TYPE::UP);

	//	PxVec3 eyePos = pEyeCollider->Collider3D()->GetColliderPos().p;
	//	Vec3 vEyePos = Vec3(eyePos.x, eyePos.y, eyePos.z);
	//	//Vec3 vConsPos = pCons->Transform()->GetRelativePos();

	//	Vec3 vDiff = vEyePos - vPos;
	//	vDiff.Normalize();

	//	Matrix rotMat = XMMatrixLookToLH(vPos, vDiff, vUp);

	//	Quat quat;
	//	Vec3 vS, vT;
	//	rotMat.Decompose(vS, quat, vT);

	//	Vec3 vRot = physx::Util::QuaternionToVector3(quat);

	//	CRenderMgr::GetInst()->GetMainCam()->GetOwner()->Transform()->SetRelativeRot(Vec3(vRot.z, -vRot.y, 0.f));

	//	GetOwner()->Animator3D()->Play((UINT)CONSTRUCTOR_ANIMATION_TYPE::APPEAR, false, false);
	//}

	//if (tState == CONSTRUCTOR_STATE::IDLE)
	//{
	//	pCons->Animator3D()->Play((UINT)CONSTRUCTOR_ANIMATION_TYPE::IDLE, true);
	//}
	//if (tState == CONSTRUCTOR_STATE::BEAM_LOOP)
	//{
	//	pCons->Animator3D()->Play((UINT)CONSTRUCTOR_ANIMATION_TYPE::BEAM_LOOP, false, false);
	//}
	//if (tState == CONSTRUCTOR_STATE::BEAM_EXIT)
	//{
	//	pCons->Animator3D()->Play((UINT)CONSTRUCTOR_ANIMATION_TYPE::BEAM_EXIT, false, false);
	//}
	//if (tState == CONSTRUCTOR_STATE::GROUND_TO_SKY)
	//{
	//	pCons->Animator3D()->Play((UINT)CONSTRUCTOR_ANIMATION_TYPE::GROUND_TO_SKY, false, false);
	//}
}
