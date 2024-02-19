#include "pch.h"
#include "CGunScript.h"

#include <Engine/CameraMgr.h>
#include <Engine/PlayerMgr.h>
#include <Engine/CRenderMgr.h>
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
		//pOwner = CRenderMgr::GetInst()->GetMainCam()->GetOwner();
	}

	CGameObject* pCamObj = GetOwner()->GetFollowObj();
	if (nullptr == pCamObj)
		pCamObj = CameraMgr::GetInst()->GetCamObj(L"MainCamera");

	Vec3 vPrevRot = pGun->Transform()->GetRelativeRot();
	Vec3 vPrevPos = pGun->Transform()->GetRelativePos();

	Vec3 vScale = pGun->Transform()->GetRelativeScale();
	Vec3 vCamRot = pCamObj->Transform()->GetRelativeRot();
	Vec2 vMouseDir = CKeyMgr::GetInst()->GetMouseDir();
	Vec3 vPrevCamRot = vCamRot;
	vCamRot.y += (DT * vMouseDir.x * 0.2f);
	vCamRot.x -= (DT * vMouseDir.y * 0.2f);
	vCamRot.z = 0;

	Vec3 vvRot = pOwner->Transform()->GetRelativeRot();
	Vec3 vRot = WeaponMgr::GetInst()->GetOwnerWeaponRot(pOwner);
	Vec3 vPos = WeaponMgr::GetInst()->GetOwnerWeaponPos(pOwner);

	vPos.y += vScale.y / 2.f;
	//vPrevPos = XMVectorLerpV(vPrevPos, vPos, Vec3(0.1f, 0.1f, 0.1f));

	//int muzzleIdx = WeaponMgr::GetInst()->GetCurWeaponIdx();
	

	
	pGun->Transform()->SetRelativePos(vPos);
	//pGun->Transform()->AddRelativeRot(vvRot);
	pGun->Transform()->SetRelativeRot(vRot + vvRot);
}
