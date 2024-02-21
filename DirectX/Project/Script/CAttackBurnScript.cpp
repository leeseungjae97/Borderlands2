#include "pch.h"
#include "CAttackBurnScript.h"

#include "CPlayerScript.h"

CAttackBurnScript::CAttackBurnScript()
	: CScript((UINT)ATTACK_BURN_SCRIPT)
{
}

CAttackBurnScript::~CAttackBurnScript()
{
}


void CAttackBurnScript::BeginOverlap(CCollider3D* _Other)
{
	CPlayerScript* player = _Other->GetOwner()->GetScript<CPlayerScript>();
	if (nullptr == player)
		return;

	if(!player->IsBurn())
	{
		player->SetBurn(true);
		player->SetBurnTime(10.f);
	}
}
