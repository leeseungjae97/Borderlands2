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
}

// Test FinalTick
void CGunScript::finaltick()
{
	// Test Code;
	//SetWeaponPos();
}

void CGunScript::SetWeaponPos()
{
	if (!m_bEquiWeapon)
		return;

	CGameObject* pPlayer = PlayerMgr::GetInst()->GetPlayer();
	CGameObject* pGun = GetOwner();

	Vec3 vvRot = pPlayer->Transform()->GetRelativeRot();
	Vec3 vRot = PlayerMgr::GetInst()->GetPlayerWeaponRot();
	Vec3 vPos = PlayerMgr::GetInst()->GetPlayerWeaponPos();

	//int muzzleIdx = WeaponMgr::GetInst()->GetCurWeaponMuzzleIdx();
	Vec3 vScale = pGun->Transform()->GetRelativeScale();

	vPos.y += vScale.y / 2.f;
	pGun->Transform()->SetRelativePos(vPos);
	//pGun->Transform()->AddRelativeRot(vvRot);
	pGun->Transform()->SetRelativeRot(vRot + vvRot);
}
