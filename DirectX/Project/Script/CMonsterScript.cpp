#include "pch.h"
#include "CMonsterScript.h"
#include <Engine\LandScapeMgr.h>

#include <Engine\func.h>
#include <Engine\PlayerMgr.h>
#include <Engine\physx_util.h>


CMonsterScript::CMonsterScript()
	: CScript((UINT)SCRIPT_TYPE::MONSTERSCRIPT)
	, fMonsterSpeed(100.f)
	, fRotateSpeed(2.f)

{
	AddScriptParam(SCRIPT_PARAM::FLOAT, &fRotateSpeed, "Rotate Speed");
}

CMonsterScript::~CMonsterScript()
{
}

void CMonsterScript::Move()
{
	CGameObject* pPlayer = PlayerMgr::GetInst()->GetPlayer();
	CGameObject* pMonster = GetOwner();

	Vec3 vPlayerPos = pPlayer->Transform()->GetRelativePos();
	Vec3 vMonsterPos = pMonster->Transform()->GetRelativePos();

	Vec3 vMonsterFront = pMonster->Transform()->GetRelativeDir(DIR_TYPE::FRONT);
	Vec3 vMonsterUp = pMonster->Transform()->GetRelativeDir(DIR_TYPE::UP);
	Vec3 vMonsterRight = pMonster->Transform()->GetRelativeDir(DIR_TYPE::RIGHT);

	//Vec3 vDir = vPlayerPos - vMonsterPos;
	//vDir.Normalize();

	Vec3 final_velocity = Vec3(0.f, 0.f, 0.f);
	float fSpeed = fMonsterSpeed;

	final_velocity += vMonsterFront * DT * fSpeed;

	pMonster->RigidBody()->SetLinearVelocity(final_velocity * fSpeed);
}

bool CMonsterScript::Rotate()
{
	CGameObject* pPlayer = PlayerMgr::GetInst()->GetPlayer();
	CGameObject* pMonster = GetOwner();

	Vec3 vPlayerPos = pPlayer->Transform()->GetRelativePos();
	Vec3 vMonsterPos = pMonster->Transform()->GetRelativePos();


	Vec3 vMonsterRight = pMonster->Transform()->GetRelativeDir(DIR_TYPE::RIGHT);

	Vec3 vDir = vPlayerPos - vMonsterPos;
	vDir.Normalize();
	vMonsterRight.Normalize();

	// 벡터의 내적은 90도 일때 0 Dot(A,B)=∣A∣⋅∣B∣⋅cos(θ)
	float includeAngle = vDir.Dot(vMonsterRight);

	pMonster->RigidBody()->SetAngularVelocity(Vec3(0.f, includeAngle * fRotateSpeed, 0.f));

	return true;
}

void CMonsterScript::tick()
{
	//Move();
	//CGameObject* pPlayer = PlayerMgr::GetInst()->GetPlayer();
	//CGameObject* pMonster = GetOwner();

	//Vec3 vPlayerPos = pPlayer->Transform()->GetRelativePos();
	//Vec3 vMonsterPos = pMonster->Transform()->GetRelativePos();

	//Vec3 vMonsterFront = pMonster->Transform()->GetRelativeDir(DIR_TYPE::FRONT);

	//Vec3 vDir = vPlayerPos - vMonsterPos;
	//vDir.Normalize();

	//float mm = vDir.Dot(vMonsterFront);
	//float tt = vMonsterFront.Dot(vDir);

	Rotate();

	if(KEY_PRESSED(KEY::_0))
	{
		Move();
	}
	if (KEY_PRESSED(KEY::_9))
	{
		//pMonster->RigidBody()->SetAngularVelocity(Vec3(0.f, tt, 0.f));
	}
}

//void CMonsterScript::BeginOverlap(CCollider2D* _Other)
//{
//	if (L"Parent Object" == _Other->GetOwner()->GetName())
//	{
//		//Destroy();	
//
//		SetLifeSpan(2.f);
//	}
//}

