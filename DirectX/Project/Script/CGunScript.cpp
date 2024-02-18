#include "pch.h"
#include "CGunScript.h"

#include <Engine/PlayerMgr.h>
#include <Engine/WeaponMgr.h>

CGunScript::CGunScript()
	: CScript(SCRIPT_TYPE::GUNSCRIPT)
	, m_bEquiWeapon(true)
{
}

CGunScript::~CGunScript()
{
}

void CGunScript::tick()
{
	SetWeaponPos();

	CGameObject* pGun = GetOwner();

	if (pGun->IsOwned())
	{
		if(pGun->GetGunOwner()->IsDead())
		{
			DestroyObject(pGun);
		}
	}
}

// Test FinalTick
void CGunScript::finaltick()
{
	// Test Code;
	//SetWeaponPos();
}

void CGunScript::SetWeaponPos()
{
	CGameObject* pGun = GetOwner();
	CGameObject* pOwner = nullptr;
	if(pGun->IsOwned())
	{
		pOwner = pGun->GetGunOwner();
	}
	else
	{
		pOwner = PlayerMgr::GetInst()->GetPlayer();
	}
	

	Vec3 vvRot = pOwner->Transform()->GetRelativeRot();
	Vec3 vRot = WeaponMgr::GetInst()->GetOwnerWeaponRot(pOwner);
	Vec3 vPos = WeaponMgr::GetInst()->GetOwnerWeaponPos(pOwner);

	//int muzzleIdx = WeaponMgr::GetInst()->GetCurWeaponIdx();
	Vec3 vScale = pGun->Transform()->GetRelativeScale();

	vPos.y += vScale.y / 2.f;
	pGun->Transform()->SetRelativePos(vPos);
	//pGun->Transform()->AddRelativeRot(vvRot);
	pGun->Transform()->SetRelativeRot(vRot + vvRot);
}
