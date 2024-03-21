#include "pch.h"
#include "FieldUIMgr.h"

#include "CMeshRender.h"
#include "CResMgr.h"
#include "CTransform.h"

#include <Script/CScriptMgr.h>

#include "CCamera.h"
#include "CKeyMgr.h"
#include "CRenderMgr.h"
#include "CTimeMgr.h"
#include "physx_util.h"
#include "TextMgr.h"

FieldUIMgr::FieldUIMgr()
{
}

FieldUIMgr::~FieldUIMgr()
{
}

void FieldUIMgr::init()
{
	BilBoardMat = XMMatrixIdentity();
	fTheta = 80.f * Util::DegToRad();
}

void FieldUIMgr::tick()
{
	//if(KEY_TAP(KEY::B))
	//{
	//	AddDamage(10, Vec3(0.f, 500.f, 0.f));
	//}
}

void FieldUIMgr::render()
{
	DamageText dt; Vec3 vPos; int i = 0;
	CCamera* cam  = CRenderMgr::GetInst()->GetMainCam();
	Vec3 vCamPos = cam->GetOwner()->Transform()->GetRelativePos();
	Vec3 vCamUp = cam->GetOwner()->Transform()->GetRelativeDir(DIR_TYPE::UP);

	for (i = 0; i < m_vecText.size(); ++i)
	{
		dt = m_vecText[i];
		vPos = dt.vPos;
		dt.fAlpha = 1.f - (dt.fAcc / 5.f);
		if (dt.fAcc > 5.f)
		{
			m_vecText.erase(m_vecText.begin() + i);
			continue;
		}

		dt.fAcc += DT;
		vPos = vPos + dt.vDir * 50.f * cosf(fTheta) * dt.fAcc;

		vPos.y = vPos.y + 100.f * sinf(fTheta) * dt.fAcc - ((0.5f * 9.81f) * (dt.fAcc * dt.fAcc));

		BilBoardMat = Matrix::CreateConstrainedBillboard(vPos, vCamPos, vCamUp);
		TextMgr::GetInst()->DrawSpriteText(std::to_wstring(dt.iDamage), Vec3::Zero, 0.f, Vec2::Zero, BilBoardMat, dt.fAlpha,true, 0.75f);

		m_vecText[i] = dt;
	}
}

void FieldUIMgr::AddDamage(int _Damage, Vec3 _vPos)
{
	CCamera* cam = CRenderMgr::GetInst()->GetMainCam();
	Vec3 vCamRight = cam->GetOwner()->Transform()->GetRelativeDir(DIR_TYPE::RIGHT);

	Vec3 vDir = Vec3::One;

	if (rand() % 2)
		vDir = vCamRight;
	else
		vDir = -vCamRight;
	
	vDir.Normalize();
	m_vecText.push_back({
		_vPos,
		vDir,
		0.0f,
		1.0f,
		_Damage
	});
}
