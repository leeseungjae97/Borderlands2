#include "pch.h"
#include "CAttackNormalScript.h"

#include "CPlayerScript.h"

CAttackNormalScript::CAttackNormalScript()
	: CScript((UINT)ATTACK_NORMAL_SCRIPT)
{
}

CAttackNormalScript::~CAttackNormalScript()
{
}

void CAttackNormalScript::BeginOverlap(CCollider3D* _Other)
{
	OutputDebugStringW(L"CAttackNormalScript BeginOverlap\n");
	CPlayerScript* player = _Other->GetOwner()->GetScript<CPlayerScript>();
	if (nullptr == player)
		return;

	player->Attacked(10.f);
}