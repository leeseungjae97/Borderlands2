#include "pch.h"

#include <Engine/CRenderMgr.h>

#include "CAttackNormalScript.h"

#include "CCameraMoveScript.h"
#include "CPlayerScript.h"

CAttackNormalScript::CAttackNormalScript()
	: CScript((UINT)ATTACK_NORMAL_SCRIPT)
	, bAttack(false)
	, bManualAttack(false)
	, bManual(false)
{
}

CAttackNormalScript::~CAttackNormalScript()
{
}

void CAttackNormalScript::tick()
{
}

void CAttackNormalScript::BeginOverlap(CCollider3D* _Other)
{

	OutputDebugStringW(L"CAttackNormalScript BeginOverlap\n");

	CPlayerScript* player = _Other->GetOwner()->GetScript<CPlayerScript>();
	if (nullptr == player)
		return;
	tRayInfo raycast;
	Vec3 vHitPos = _Other->GetOwner()->Transform()->GetRelativePos();
	Vec3 vAttackPos = Vec3::Zero;
	if(GetOwner()->Transform())
	{
		vAttackPos = GetOwner()->Transform()->GetRelativePos();
	}else
	{
		PxVec3 v= GetOwner()->Collider3D()->GetColliderPos().p;
		vAttackPos = Vec3(v.x, v.y, v.z);
	}
	

	raycast.vDir = (vHitPos - vAttackPos).Normalize();
	raycast.fDamage = 10.f;
	raycast.tRayType = (UINT)RAYCAST_TYPE::SHOOT;
	raycast.vStart = vAttackPos;

	if (bManual)
	{
		if (bManualAttack)
		{
			bManualAttack = false;
			//player->Attacked(10.f);
			CRenderMgr::GetInst()->GetMainCam()->GetOwner()->GetScript<CCameraMoveScript>()->SetShake(true, 30);
			player->Raycast(raycast);
		}
	}
	else
	{
		if (bAttack)
			return;

		bAttack = true;

		//player->Attacked(10.f);
		CRenderMgr::GetInst()->GetMainCam()->GetOwner()->GetScript<CCameraMoveScript>()->SetShake(true, 30);
		player->Raycast(raycast);
	}

}

void CAttackNormalScript::OnOverlap(CCollider3D* _Other)
{
	OutputDebugStringW(L"CAttackNormalScript OnOverlap\n");
	CPlayerScript* player = _Other->GetOwner()->GetScript<CPlayerScript>();

	if (nullptr == player)
		return;

	tRayInfo raycast;
	Vec3 vHitPos = _Other->GetOwner()->Transform()->GetRelativePos();
	Vec3 vAttackPos = GetOwner()->Transform()->GetRelativePos();

	raycast.vDir = (vHitPos - vAttackPos).Normalize();
	raycast.fDamage = 10.f;
	raycast.tRayType = (UINT)RAYCAST_TYPE::SHOOT;
	raycast.vStart = vAttackPos;

	if (bManual)
	{
		if (bManualAttack)
		{
			bManualAttack = false;
			//player->Attacked(10.f);
			CRenderMgr::GetInst()->GetMainCam()->GetOwner()->GetScript<CCameraMoveScript>()->SetShake(true, 30);
			player->Raycast(raycast);
		}
	}
	else
	{
		if (bAttack)
			return;

		bAttack = true;

		//player->Attacked(10.f);
		CRenderMgr::GetInst()->GetMainCam()->GetOwner()->GetScript<CCameraMoveScript>()->SetShake(true, 30);
		player->Raycast(raycast);
	}
}

void CAttackNormalScript::EndOverlap(CCollider3D* _Other)
{
	OutputDebugStringW(L"CAttackNormalScript EndOverlap\n");
	bAttack = false;
}
