#include "pch.h"
#include "CAttackNormalScript.h"

#include "CPlayerScript.h"

CAttackNormalScript::CAttackNormalScript()
	: CScript((UINT)ATTACK_NORMAL_SCRIPT)
	, bBegin(false)
{
}

CAttackNormalScript::~CAttackNormalScript()
{
}

void CAttackNormalScript::tick()
{
	int a = 0;
}

void CAttackNormalScript::BeginOverlap(CCollider3D* _Other)
{
	if (bBegin)
		return;

	bBegin = true;

	OutputDebugStringW(L"CAttackNormalScript BeginOverlap\n");
	CPlayerScript* player = _Other->GetOwner()->GetScript<CPlayerScript>();
	if (nullptr == player)
		return;

	player->Attacked(10.f);
}

void CAttackNormalScript::OnOverlap(CCollider3D* _Other)
{
	if (bBegin)
		return;

	bBegin = true;

	OutputDebugStringW(L"CAttackNormalScript OnOverlap\n");
	CPlayerScript* player = _Other->GetOwner()->GetScript<CPlayerScript>();

	if (nullptr == player)
		return;

	player->Attacked(10.f);
}

void CAttackNormalScript::EndOverlap(CCollider3D* _Other)
{
	OutputDebugStringW(L"CAttackNormalScript EndOverlap\n");
	bBegin = false;
}
