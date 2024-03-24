#include "pch.h"
#include "CCameraMoveScript.h"

#include <Engine\CTransform.h>
#include <Engine\CRigidBody.h>
#include <Engine\CCamera.h>
#include <Engine\WeaponMgr.h>
#include <Engine\CRenderMgr.h>

CCameraMoveScript::CCameraMoveScript()
	: CScript((UINT)SCRIPT_TYPE::CAMERA_MOVE_SCRIPT)
	, m_fCamSpeed(5000.f)
	, m_bMove(false)
	, m_vTargetPos(Vec4::Zero)
{
}

CCameraMoveScript::~CCameraMoveScript()
{
}

void CCameraMoveScript::mainMenuCameraMove()
{
	CGameObject* pCamera = GetOwner();

	if (pCamera->GetName() != L"MainMenuCamera")
		return;

	Vec3 vPos = pCamera->Transform()->GetRelativePos();
	Vec3 vUp = pCamera->Transform()->GetRelativeDir(DIR_TYPE::UP);

	Vec3 vDiff = m_vTargetPos - vPos;
	vDiff.Normalize();

	Matrix rotMat = XMMatrixLookToLH(vPos, vDiff, vUp);

	Quat quat;
	Vec3 vS, vT;
	rotMat.Decompose(vS, quat, vT);

	Vec3 vRot = physx::Util::QuaternionToVector3(quat);

	vPos.x += DT * sinf(50.f * DegToRad()) * 200.f;
	vPos.z += DT * cosf(50.f * DegToRad()) * 200.f;

	pCamera->Transform()->SetRelativeRot(vRot);
	pCamera->Transform()->SetRelativePos(vPos);
}

void CCameraMoveScript::scopeCameraMove()
{
	CGameObject* pCamera = GetOwner();
	CCamera* cam = CRenderMgr::GetInst()->GetMainCam();

	if (pCamera->GetName() != L"ScopeCamera")
		return;

	if (nullptr == WeaponMgr::GetInst()->GetCurWeapon())
		return;

	if (nullptr == cam)
		return;

	Vec3 vPos = WeaponMgr::GetInst()->GetCurWeaponMuzzlePos();
	Vec3 vRot = cam->GetOwner()->Transform()->GetRelativeRot();
	pCamera->Transform()->SetRelativeRot(vRot);
	Vec3 vFront = pCamera->Transform()->GetRelativeDir(DIR_TYPE::FRONT);
	vPos += vFront * 150.f;

	pCamera->Transform()->SetRelativePos(vPos);
}

void CCameraMoveScript::warriorCameraMove()
{

}

void CCameraMoveScript::tick()
{
	//if (CLevelMgr::GetInst()->GetCurLevel()->GetState() != LEVEL_STATE::PLAY)
	//	cameraDebugMove();
	//else
	cameraFollowMove();
	scopeCameraMove();
}

void CCameraMoveScript::finaltick()
{
	if (CLevelMgr::GetInst()->GetCurLevel()->GetState() != LEVEL_STATE::PLAY)
		cameraDebugMove();




	//if(KEY_TAP(KEY::G))
	//{
	//	CGameObject* pCamera = GetOwner();
	//	Vec3 vPos = pCamera->Transform()->GetRelativePos();
	//	Vec3 vFront = pCamera->Transform()->GetRelativeDir(DIR_TYPE::FRONT);
	//	m_vTargetPos = vPos + vFront * 1000.f;
	//	m_bMove = !m_bMove;
	//}

	//if(m_bMove)
	//	mainMenuCameraMove();
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
}
