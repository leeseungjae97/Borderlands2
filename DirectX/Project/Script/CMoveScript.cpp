#include "pch.h"
#include "CMoveScript.h"

#include <Engine/CUI.h>
#include <Engine/TextMgr.h>
#include <Engine/WeaponMgr.h>
#include <Engine/ThreadMgr.h>

CMoveScript::CMoveScript()
	: CScript(SCRIPT_TYPE::MOVE_SCRIPT)
	, fMoveAcc(0.f)
	, fMoveTime(10.f)
	, fSpeed(5.f)
{
}

CMoveScript::~CMoveScript()
{
}

void CMoveScript::tick()
{
	//CGameObject* pOwner = GetOwner()->GetWeaponOwner();
	//if (pOwner)
	//{
	//	Vec3 vPos = WeaponMgr::GetInst()->GetCurWeaponMuzzlePos();
	//	Vec3 vRot = WeaponMgr::GetInst()->GetCurWeapon()->Transform()->GetRelativeRot();

	//	GetOwner()->Transform()->SetRelativePos(vPos);
	//	GetOwner()->Transform()->SetRelativeRot(vRot);
	//}
	//Rotate();
}

void CMoveScript::finaltick()
{
	Rotate();
}

void CMoveScript::begin()
{
}

void CMoveScript::Rotate()
{
	CGameObject* pOwner = GetOwner();

	Vec3 vRot = pOwner->Transform()->GetRelativeRot();

	vRot.y += 30.f * DegToRad() * DT;

	pOwner->Transform()->SetRelativeRot(vRot);
}
