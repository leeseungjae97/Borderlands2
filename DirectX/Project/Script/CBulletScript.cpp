#include "pch.h"
#include "CBulletScript.h"

#include <Engine\CRenderMgr.h>
CBulletScript::CBulletScript()
	: CScript((UINT)SCRIPT_TYPE::BULLETSCRIPT)
	, m_fBulletSpeed(100.f)
	, m_fAlive(5.f)
	, m_fAliveAcc(0.0f)
{
	CCamera* MainCam = CRenderMgr::GetInst()->GetMainCam();
	tRay Ray = MainCam->GetRay();
	m_vDir = Ray.vDir;
}

CBulletScript::~CBulletScript()
{

}

void CBulletScript::BeginOverlap(CCollider3D* _Other)
{
	OutputDebugStringW(L"Bullet Begin Over lap\n");
	DestroyObject(GetOwner());
}

void CBulletScript::tick()
{
	m_fAliveAcc += DT;
	if(m_fAliveAcc > m_fAlive)
	{
		OutputDebugStringW(L"Bullet Destroy\n");

		m_fAliveAcc = 0.f;
		DestroyObject(GetOwner());
		return;
	}

	Vec3 vPos = GetOwner()->Transform()->GetRelativePos();

	vPos.x += m_vDir.x * DT * m_fBulletSpeed;
	vPos.y += m_vDir.y * DT * m_fBulletSpeed;
	vPos.z += m_vDir.z * DT * m_fBulletSpeed;

	GetOwner()->Transform()->SetRelativePos(vPos);
}
