#include "pch.h"
#include "CMonsterScript.h"
#include <Engine\LandScapeMgr.h>

#include <Engine\func.h>


CMonsterScript::CMonsterScript()
	: CScript((UINT)SCRIPT_TYPE::MONSTERSCRIPT)
{
}

CMonsterScript::~CMonsterScript()
{
}

void CMonsterScript::Move()
{
	Vec3 vPos = GetOwner()->Transform()->GetRelativePos();

	//float fPosDegree = RadianToDegree(atan2(vPos.x, vPos.y));

	Vec3 vRayPos = LandScapeMgr::GetInst()->GetLandScapeCrossPos();

	Vec3 vDir = vRayPos - vPos;
	vDir.Normalize();

	float fDirTheta = atan2(vDir.x, vDir.z);
	fDirTheta -= XM_PI / 2.f;
	//float fDirTheta = atan2(vDir.z, vPos.z);

	Vec3 vRot = (Vec3(0.f, fDirTheta, 0.f));

	vPos.x = vPos.x + (100.f * DT * vDir.x);
	vPos.y = vPos.y + (100.f * DT * vDir.y);
	vPos.z = vPos.z + (100.f * DT * vDir.z);

	GetOwner()->Transform()->SetRelativeRot(vRot);
	GetOwner()->Transform()->SetRelativePos(vPos);
}

void CMonsterScript::tick()
{
	if(KEY_PRESSED(KEY::LBTN))
	{
		//Move();
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

