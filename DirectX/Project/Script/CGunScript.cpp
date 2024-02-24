#include "pch.h"
#include "CGunScript.h"

#include <Engine/CameraMgr.h>
#include <Engine/PlayerMgr.h>
#include <Engine/CRenderMgr.h>
#include <Engine/WeaponMgr.h>

CGunScript::CGunScript()
	: CScript(SCRIPT_TYPE::GUN_SCRIPT)
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
		//pOwner = CRenderMgr::GetInst()->GetMainCam()->GetOwner();
	}
	if (nullptr == pOwner)
		return;

	CGameObject* pCamObj = GetOwner()->GetFollowObj();
	if (nullptr == pCamObj)
		pCamObj = CameraMgr::GetInst()->GetCamObj(L"MainCamera");

	Vec3 vPrevPos = pGun->Transform()->GetRelativePos();

	Vec3 vCurRot = pOwner->Transform()->GetRelativeRot();
	Vec3 vRot = WeaponMgr::GetInst()->GetOwnerWeaponRot(pOwner);
	Vec3 vPos = WeaponMgr::GetInst()->GetOwnerWeaponPos(pOwner);

	//vPos.y += vScale.y / 2.f;
	m_vPrevRot = XMVectorLerpV(m_vPrevRot, vCurRot, Vec3(0.3f, 0.3f, 0.3f));
	
	//int muzzleIdx = WeaponMgr::GetInst()->GetCurWeaponIdx();

	pGun->Transform()->SetRelativePos(vPos);
	//pGun->Transform()->AddRelativeRot(vvRot);
	pGun->Transform()->SetRelativeRot(vRot + m_vPrevRot);
}
