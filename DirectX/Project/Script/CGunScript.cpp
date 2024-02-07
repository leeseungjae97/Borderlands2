#include "pch.h"
#include "CGunScript.h"

#include <Engine/PlayerMgr.h>

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

void CGunScript::SetWeaponPos()
{
	if (!m_bEquiWeapon)
		return;

	CGameObject* pPlayer = PlayerMgr::GetInst()->GetPlayer();

	Vec3 vRot = pPlayer->Transform()->GetRelativeRot();
	Vec3 vPos = PlayerMgr::GetInst()->GetPlayerWeaponPos();
	
	CGameObject* pGun = GetOwner();
	pGun->Transform()->SetRelativePos(vPos);
	pGun->Transform()->SetRelativeRot(vRot);
}