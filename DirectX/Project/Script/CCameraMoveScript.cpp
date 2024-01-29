#include "pch.h"
#include "CCameraMoveScript.h"

#include <Engine\CTransform.h>
#include <Engine\CRigidBody.h>
#include <Engine\CCamera.h>

CCameraMoveScript::CCameraMoveScript()
	: CScript((UINT)SCRIPT_TYPE::CAMERAMOVESCRIPT)
	, m_fCamSpeed(1000.f)
{
}

CCameraMoveScript::~CCameraMoveScript()
{
}

void CCameraMoveScript::tick()
{
	if (CLevelMgr::GetInst()->GetCurLevel()->GetState() != LEVEL_STATE::PLAY)
		cameraDebugMove();
	else
		cameraFollowMove();
}


void CCameraMoveScript::cameraDebugMove()
{
	Vec3 vPos = Transform()->GetRelativePos();
	Vec3 vRot = Transform()->GetRelativeRot();

	Vec3 vFront = Transform()->GetRelativeDir(DIR_TYPE::FRONT);
	Vec3 vUp = Transform()->GetRelativeDir(DIR_TYPE::UP);
	Vec3 vRight = Transform()->GetRelativeDir(DIR_TYPE::RIGHT);

	float fSpeed = m_fCamSpeed;

	if (KEY_PRESSED(KEY::LSHIFT))
		fSpeed *= 10.f;
	if (KEY_PRESSED(KEY::LCTRL))
		fSpeed *= 0.05f;

	if (KEY_PRESSED(KEY::W))
	{
		vPos += DT * vFront * fSpeed;
	}

	if (KEY_PRESSED(KEY::S))
	{
		vPos -= DT * vFront * fSpeed;
	}

	if (KEY_PRESSED(KEY::A))
	{
		vPos -= DT * vRight * fSpeed;
	}

	if (KEY_PRESSED(KEY::D))
	{
		vPos += DT * vRight * fSpeed;
	}



	if (KEY_PRESSED(KEY::RBTN))
	{
		Vec2 vMouseDir = CKeyMgr::GetInst()->GetMouseDir();
		vRot.y += DT * vMouseDir.x * 5.f;
		vRot.x -= DT * vMouseDir.y * 5.f;
	}

	Transform()->SetRelativePos(vPos);
	Transform()->SetRelativeRot(vRot);

}

void CCameraMoveScript::cameraFollowMove()
{
	if (GetOwner()->GetName() == L"EditCam"
		|| nullptr == GetOwner()->GetFollowObj()) 
		return;

	//Vec3 vPos = GetOwner()->GetFollowObj()->Transform()->GetRelativePos();
	//Vec3 vRot = GetOwner()->GetFollowObj()->Transform()->GetRelativeRot();

	//Vec3 vCurRot = GetOwner()->Transform()->GetRelativeRot();

	//Vec3 vFront = GetOwner()->GetFollowObj()->Transform()->GetRelativeDir(DIR_TYPE::FRONT);
	//Vec3 vUp = GetOwner()->GetFollowObj()->Transform()->GetRelativeDir(DIR_TYPE::UP);
	//Vec3 vRight = GetOwner()->GetFollowObj()->Transform()->GetRelativeDir(DIR_TYPE::RIGHT);

	//CRigidBody* _db = GetOwner()->GetFollowObj()->RigidBody();

	//Vec2 vMouseDir = CKeyMgr::GetInst()->GetMouseDir();

	//vCurRot.y += (DT * vMouseDir.x * 1.f);
	//vCurRot.x -= (DT * vMouseDir.y * 1.f);
	//vCurRot.z = 0;

	//GetOwner()->GetFollowObj()->Transform()->SetRelativeRot(Vec3(0.f, vCurRot.y, 0.f));
	//GetOwner()->Transform()->SetRelativeRot(vCurRot);

	////vPos -= vFront;
	//vPos += GetOwner()->Transform()->GetFollowOffset();
	//GetOwner()->Transform()->SetRelativePos(vPos);

	//float fSpeed = 100.f;
	//Vec3 final_velocity = Vec3(0.f, 0.f, 0.f);

	//if (KEY_PRESSED(KEY::LSHIFT))
	//{
	//	fSpeed *= 2.f;
	//}
	//if (KEY_PRESSED(KEY::W))
	//{
	//	final_velocity += vFront * DT * fSpeed;
	//}

	//if (KEY_PRESSED(KEY::S))
	//{
	//	final_velocity += vFront * DT * -fSpeed;
	//}

	//if (KEY_PRESSED(KEY::A))
	//{
	//	final_velocity += vRight * DT * -fSpeed;
	//}

	//if (KEY_PRESSED(KEY::D))
	//{
	//	final_velocity += vRight * DT * fSpeed;
	//}

	//if (KEY_PRESSED(KEY::SPACE))
	//{
	//	final_velocity += vUp * DT * 50.f;
	//}

	//_db->SetVelocity(final_velocity * fSpeed);
}
