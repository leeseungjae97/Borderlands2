#include "pch.h"
#include "CPlayerScript.h"

#include <Engine\CMeshRender.h>
#include <Engine\CMaterial.h>
#include <Engine\CRenderMgr.h>

#include "CBulletScript.h"
#include "CMissileScript.h"


CPlayerScript::CPlayerScript()
	: CScript((UINT)SCRIPT_TYPE::PLAYERSCRIPT)
	, m_fSpeed(100.f)
	, m_fJump(50.f)
	, m_fRateOfFire(0.2f)
	, m_fRateOfFireAcc(0.0f)
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
	//MeshRender()->GetDynamicMaterial(0);


}

void CPlayerScript::tick()
{
	Move();
}

void CPlayerScript::Shoot()
{
	CCamera* MainCam = CRenderMgr::GetInst()->GetMainCam();

	tRay Ray = MainCam->GetRay();
	Vec3 vStart = Ray.vStart;

	CGameObject* pBullet = new CGameObject;
	pBullet->SetName(L"Player Bullet");
	pBullet->AddComponent(new CTransform);
	pBullet->AddComponent(new CRigidBody(RIGID_BODY_SHAPE_TYPE::BOX));
	pBullet->AddComponent(new CCollider3D(false));
	pBullet->AddComponent(new CBulletScript);

	SpawnGameObject(pBullet, vStart, L"Bullet");
}

void CPlayerScript::Move()
{
	m_fRateOfFireAcc += DT;

	CGameObject* pCamObj = GetOwner()->GetFollowObj();
	CGameObject* pPlayerObj = GetOwner();

	Vec3 vPlayerPos = pPlayerObj->Transform()->GetRelativePos();
	Vec3 vCamRot = pCamObj->Transform()->GetRelativeRot();

	Vec3 vPlayerFront = pPlayerObj->Transform()->GetRelativeDir(DIR_TYPE::FRONT);
	Vec3 vPlayerUp = pPlayerObj->Transform()->GetRelativeDir(DIR_TYPE::UP);
	Vec3 vPlayerRight = pPlayerObj->Transform()->GetRelativeDir(DIR_TYPE::RIGHT);

	CRigidBody* pPlayerRB = pPlayerObj->RigidBody();

	Vec2 vMouseDir = CKeyMgr::GetInst()->GetMouseDir();

	vCamRot.y += (DT * vMouseDir.x * 1.f);
	vCamRot.x -= (DT * vMouseDir.y * 1.f);
	vCamRot.z = 0;

	pPlayerObj->Transform()->SetRelativeRot(Vec3(0.f, vCamRot.y, 0.f));
	pCamObj->Transform()->SetRelativeRot(vCamRot);

	//vPos -= vFront;
	vPlayerPos += pCamObj->Transform()->GetFollowOffset();
	pCamObj->Transform()->SetRelativePos(vPlayerPos);

	float fSpeed = m_fSpeed;

	Vec3 final_velocity = Vec3(0.f, 0.f, 0.f);

	if (KEY_PRESSED(KEY::LSHIFT))
	{
		fSpeed *= 2.f;
	}
	if (KEY_PRESSED(KEY::W))
	{
		final_velocity += vPlayerFront * DT * fSpeed;
	}

	if (KEY_PRESSED(KEY::S))
	{
		final_velocity += vPlayerFront * DT * -fSpeed;
	}

	if (KEY_PRESSED(KEY::A))
	{
		final_velocity += vPlayerRight * DT * -fSpeed;
	}

	if (KEY_PRESSED(KEY::D))
	{
		final_velocity += vPlayerRight * DT * fSpeed;
	}

	if (KEY_PRESSED(KEY::SPACE))
	{
		final_velocity += vPlayerUp * DT * m_fJump;
	}
	if (KEY_PRESSED(KEY::V))
	{
		if (m_fRateOfFireAcc >= m_fRateOfFire)
		{
			m_fRateOfFireAcc = 0.0f;
			Shoot();
		}
	}
	if (KEY_PRESSED(KEY::F))
	{
		final_velocity = Vec3(0.f, 0.f, 0.f);
		pPlayerRB->SetLinearVelocityZero();
	}

	pPlayerRB->SetLinearVelocity(final_velocity * fSpeed);
}

void CPlayerScript::BeginOverlap(CCollider3D* _Other)
{

}

void CPlayerScript::SaveToLevelFile(FILE* _File)
{
	fwrite(&m_fSpeed, sizeof(float), 1, _File);
}

void CPlayerScript::LoadFromLevelFile(FILE* _File)
{
	fread(&m_fSpeed, sizeof(float), 1, _File);
}