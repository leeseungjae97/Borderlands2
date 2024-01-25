#include "pch.h"
#include "CPlayerScript.h"

#include <Engine\CMeshRender.h>
#include <Engine\CMaterial.h>
#include <Engine\CRenderMgr.h>

#include "CMissileScript.h"


CPlayerScript::CPlayerScript()
	: CScript((UINT)SCRIPT_TYPE::PLAYERSCRIPT)
	, m_fSpeed(100.f)
	, m_MouseAcces(1.f)
{
	AddScriptParam(SCRIPT_PARAM::FLOAT, &m_fSpeed, "Player Speed");
	AddScriptParam(SCRIPT_PARAM::FLOAT, &m_MouseAcces, "Mouse Speed");
}

CPlayerScript::~CPlayerScript()
{

}

void CPlayerScript::begin()
{
	// 동적 재질 생성
	MeshRender()->GetDynamicMaterial(0);
}

void CPlayerScript::tick()
{
	Vec3 vPos = GetOwner()->GetFollowObj()->Transform()->GetRelativePos();
	Vec3 vCurRot = GetOwner()->GetFollowObj()->Transform()->GetRelativeRot();

	Vec3 vFront = GetOwner()->Transform()->GetRelativeDir(DIR_TYPE::FRONT);
	Vec3 vUp = GetOwner()->Transform()->GetRelativeDir(DIR_TYPE::UP);
	Vec3 vRight = GetOwner()->Transform()->GetRelativeDir(DIR_TYPE::RIGHT);

	Vec2 vMouseDir = CKeyMgr::GetInst()->GetMouseDir();
	vCurRot.y += DT * vMouseDir.x * m_MouseAcces;
	vCurRot.x -= DT * vMouseDir.y * m_MouseAcces;

	GetOwner()->Transform()->SetRelativeRot(vCurRot);

	if (KEY_PRESSED(KEY::W))
	{
		vPos += DT * vFront * m_fSpeed;
		//vPos.z += DT * m_fSpeed;
	}

	if (KEY_PRESSED(KEY::S))
	{
		vPos -= DT * vFront * m_fSpeed;
		//vPos.z -= DT * m_fSpeed;
	}

	if (KEY_PRESSED(KEY::A))
	{
		vPos -= DT * vRight * m_fSpeed;
	}

	if (KEY_PRESSED(KEY::D))
	{
		vPos += DT * vRight * m_fSpeed;
	}

	GetOwner()->GetFollowObj()->Transform()->SetRelativePos(vPos);
	GetOwner()->GetFollowObj()->Transform()->SetRelativeRot(vCurRot);		

	//if (KEY_TAP(KEY::SPACE))
	//{
	//	DrawDebugCircle(Transform()->GetWorldPos(), 500.f, Vec4(0.f, 0.f, 1.f, 1.f), Vec3(0.f, 0.f, 0.f), 2.f);

	//	Shoot();
	//}	
}

void CPlayerScript::Shoot()
{
	// 미사일 프리팹 참조
	//Ptr<CPrefab> pMissilePrefab = CResMgr::GetInst()->FindRes<CPrefab>(L"MissilePrefab");
	//Vec3 vMissilePos = Transform()->GetRelativePos() + Vec3(0.f, 0.5f, 0.f) * Transform()->GetRelativeScale();
	//CGameObject* pCloneMissile = pMissilePrefab->Instantiate();

	//// 레벨에 추가
	//SpawnGameObject(pCloneMissile, vMissilePos, L"PlayerProjectile");
}

//void CPlayerScript::BeginOverlap(CCollider2D* _Other)
//{
//	CGameObject* pOtherObject = _Other->GetOwner();
//
//	if (pOtherObject->GetName() == L"Monster")
//	{
//		DestroyObject(pOtherObject);		
//	}
//}





void CPlayerScript::SaveToLevelFile(FILE* _File)
{
	fwrite(&m_fSpeed, sizeof(float), 1, _File);
}

void CPlayerScript::LoadFromLevelFile(FILE* _File)
{
	fread(&m_fSpeed, sizeof(float), 1, _File);
}