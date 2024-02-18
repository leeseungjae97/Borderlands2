#include "pch.h"
#include "CWarriorScript.h"

#include <Engine\AnimationMgr.h>
CWarriorScript::CWarriorScript()
	: CScript((UINT)SCRIPT_TYPE::WARRIORSCRIPT)
	, tState(WarriorMgr::WARRIOR_STATE::IDLE)
{
	AddScriptParam(SCRIPT_PARAM::VEC4, &m_vRot, "offset rotate");
	AddScriptParam(SCRIPT_PARAM::VEC4, &m_R, "rotate r");
}

CWarriorScript::~CWarriorScript()
{
}

void CWarriorScript::begin()
{
	CGameObject* pWarrior = GetOwner();

	makeBreath();
		

	pWarrior->Animator3D()->SetAnimClipEventIdx(
		(UINT)WARRIOR_ANIMATION_TYPE::BREATH, -1, -1, 103, 130);

	pWarrior->Animator3D()->EndEvent((UINT)WARRIOR_ANIMATION_TYPE::BREATH)
		= std::make_shared<std::function<void()>>([=]()
			{
				tState = WarriorMgr::WARRIOR_STATE::IDLE;
			});
	pWarrior->Animator3D()->ProgressEvent((UINT)WARRIOR_ANIMATION_TYPE::BREATH)
		= std::make_shared<std::function<void()>>([=]()
			{
				pBreath->SetObjectState(CGameObject::OBJECT_STATE::VISIBLE);
			});

	pWarrior->Animator3D()->CompleteEvent((UINT)WARRIOR_ANIMATION_TYPE::BREATH)
		= std::make_shared<std::function<void()>>([=]()
			{
				pBreath->SetObjectState(CGameObject::OBJECT_STATE::INVISIBLE);
			});
}

void CWarriorScript::DoBreath()
{
	if (tState == WarriorMgr::WARRIOR_STATE::BREATH)
		return;
	// 103 ~ 130
	CGameObject* pWarrior = GetOwner();

	pWarrior->Animator3D()->Play((UINT)WARRIOR_ANIMATION_TYPE::BREATH, false);
	tState = WarriorMgr::WARRIOR_STATE::BREATH;
}

void CWarriorScript::DoTailBeam()
{
}

void CWarriorScript::DoThrowRock()
{
}

void CWarriorScript::DoTailAttack()
{
}

void CWarriorScript::makeBreath()
{
	CGameObject* pWarrior = GetOwner();
	int fireBreathIdx = pWarrior->Animator3D()->GetFireBreathIdx();
	Vec3 vPos = AnimationMgr::GetInst()->BonePos(fireBreathIdx, pWarrior);
	Vec3 vRot = AnimationMgr::GetInst()->BoneRot(fireBreathIdx, pWarrior);
	{
		Ptr<CMeshData> pMeshData = nullptr;

		pMeshData = CResMgr::GetInst()->LoadFBX(L"fbx\\fire_breath.fbx");
		pBreath = pMeshData->Instantiate(Vec3(15000.f, 15000.f, 30000.f));
		pBreath->AddComponent(new CCollider3D(false));
		pBreath->Collider3D()->SetScale(Vec3(15000.f, 15000.f, 30000.f));
		//pObj->Collider3D()->SetCenter(true);
		pBreath->AddComponent(new CGizmo);

		pBreath->SetName(L"fbx fire breath");
		pBreath->MeshRender()->GetMaterial(0)->SetTexFlow(true);
		pBreath->MeshRender()->GetMaterial(0)->SetFlowSpeed(2.f);
		pBreath->MeshRender()->GetMaterial(0)->SetFlowDir(Vec2(-0.5f, 0.0f));

		//vRot.z = -50.f;
		//vRot.y = 0.f;
		//vRot.x = 0.f;
		pBreath->Transform()->SetRelativeRot(vRot);
		PreloadGameObject(pBreath, vPos, LAYER_TYPE::Environment);
	}
	pBreath->SetObjectState(CGameObject::OBJECT_STATE::INVISIBLE);
}

void CWarriorScript::tick()
{
	CGameObject* pWarrior = GetOwner();
	static bool binit = false;
	if (!binit)
	{

		if (GetOwner()->MeshRender()->GetMaterial(3) != nullptr)
		{
			GetOwner()->MeshRender()->GetMaterial(3)->SetTexFlow(true);
		}
		binit = true;

	}

	if (KEY_PRESSED(KEY::Q))
	{
		DoBreath();
		pBreath->SetObjectState(CGameObject::OBJECT_STATE::VISIBLE);
	}

	if (pBreath->GetObjectState() == CGameObject::OBJECT_STATE::VISIBLE)
	{
		CGameObject* pWarrior = GetOwner();
		int tougeIdx = pWarrior->Animator3D()->GetFireBreathIdx();
		Vec3 vPos = AnimationMgr::GetInst()->BonePos(tougeIdx, pWarrior);
		Vec3 vRot = AnimationMgr::GetInst()->BoneRot(tougeIdx, pWarrior);
		vRot.y *= -1.f;

		pBreath->Transform()->SetRelativePos(vPos);
		pBreath->Transform()->SetRelativeRot(vRot);
	}

	if(tState == WarriorMgr::WARRIOR_STATE::IDLE)
	{
		pWarrior->Animator3D()->Play((UINT)WARRIOR_ANIMATION_TYPE::IDLE, true);
	}
}

void CWarriorScript::finaltick()
{
	static bool binit = false;
	if (!binit)
	{

		if (GetOwner()->MeshRender()->GetMaterial(3) != nullptr)
		{
			GetOwner()->MeshRender()->GetMaterial(3)->SetTexFlow(true);
		}
		binit = true;
	}

}

void CWarriorScript::Raycast(tRayInfo _RaycastInfo)
{
}

void CWarriorScript::LoadFromLevelFile(FILE* _FILE)
{
}

void CWarriorScript::SaveToLevelFile(FILE* _File)
{
}
