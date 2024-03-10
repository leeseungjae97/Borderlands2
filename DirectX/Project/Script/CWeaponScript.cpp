#include "pch.h"
#include "CWeaponScript.h"

#include <Engine/CameraMgr.h>
#include <Engine/PlayerMgr.h>
#include <Engine/CRenderMgr.h>
#include <Engine/WeaponMgr.h>

#include "CEnemyScript.h"

CWeaponScript::CWeaponScript()
	: CScript(SCRIPT_TYPE::WEAPON_SCRIPT)
	, m_bEquiWeapon(true)
	, m_bStopFollow(false)
	, m_vRot(Vec4::Zero)
	, m_vSign(Vec4::One)
{
	AddScriptParam(SCRIPT_PARAM::INT, &m_bStopFollow, "NO FOLLOW");
	AddScriptParam(SCRIPT_PARAM::VEC4, &m_vRot, "ROT");
	AddScriptParam(SCRIPT_PARAM::VEC4, &m_vSign, "SIGN");
}

CWeaponScript::~CWeaponScript()
{
}

void CWeaponScript::tick()
{
	SetWeaponPos();

	CGameObject* pWeapon = GetOwner();

	if (pWeapon->IsOwned())
	{
		if(pWeapon->GetWeaponOwner()->IsDead())
		{
			DestroyObject(pWeapon);
		}
	}
}

// Test FinalTick
void CWeaponScript::finaltick()
{
	// Test Code;
	//SetWeaponPos();
}

void CWeaponScript::SetWeaponPos()
{
	if (m_bStopFollow)
		return;

	CGameObject* pWeapon = GetOwner();
	CGameObject* pOwner = nullptr;
	bool bRight = true;
	bool bPlayer = false;
	if(pWeapon->IsOwned())
	{
		pOwner = pWeapon->GetWeaponOwner();
		bRight = pOwner->GetScript<CEnemyScript>()->GetEnemyWeaponHand();
	}
	else
	{
		bPlayer = true;
		pOwner = PlayerMgr::GetInst()->GetPlayer();
		//pOwner = CRenderMgr::GetInst()->GetMainCam()->GetOwner();
	}
	if (nullptr == pOwner)
		return;

	Vec3 vCurRot = pOwner->Transform()->GetRelativeRot();
	Vec3 vRot = WeaponMgr::GetInst()->GetOwnerWeaponRot(pOwner, bRight);
	Vec3 vPos = WeaponMgr::GetInst()->GetOwnerWeaponPos(pOwner);
	if (pWeapon->IsMelee())
	{
		//vRot.x *= m_vSign.x;
		//vRot.z *= m_vSign.z;
		//vRot.y *= m_vSign.y;

		//vRot.x += m_vRot.x * DegToRad();
		//vRot.z += m_vRot.z * DegToRad();
		//vRot.y += m_vRot.y * DegToRad();

		vRot.x += 90.f * DegToRad();
		vRot.y += (-90.f * DegToRad());

		Vec3 vScale = pOwner->Transform()->GetRelativeScale();
		vPos.y -= vScale.y / 2.f;

		//vRot.y -= XM_PI / 2.f;
		//vRot.x += XM_PI / 3.f;
	}

	m_vPrevRot = XMVectorLerpV(m_vPrevRot, vCurRot, Vec3(0.3f, 0.3f, 0.3f));
	
	pWeapon->Transform()->SetRelativePos(vPos);
	if(bPlayer)
		pWeapon->Transform()->SetRelativeRot(vRot + m_vPrevRot);
	else
		pWeapon->Transform()->SetRelativeRot(vRot);
}
