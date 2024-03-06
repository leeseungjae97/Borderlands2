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
	textWorldMat = XMMatrixIdentity();
	textScaleMat = XMMatrixIdentity();
	textScaleMat = XMMatrixScaling(1.f, 1.f, 1.f);

	textRotMat = XMMatrixIdentity();
	textRotMat = XMMatrixRotationX(0.f);
	textRotMat *= XMMatrixRotationY(0.f);
	textRotMat *= XMMatrixRotationZ(-180.f * Util::DegToRad());

	fTheta = 80.f * Util::DegToRad();
}

void FieldUIMgr::tick()
{
	if(KEY_TAP(KEY::B))
	{
		AddDamage(10, Vec3(0.f, 500.f, 0.f));
	}
}

void FieldUIMgr::render()
{
	DamageText dt; Vec3 vPos; int iRandSign; int i = 0;
	CCamera* cam  = CRenderMgr::GetInst()->GetMainCam();
	Vec3 vCamPos = cam->GetOwner()->Transform()->GetRelativePos();
	Vec3 vCamUp = cam->GetOwner()->Transform()->GetRelativeDir(DIR_TYPE::UP);
	Vec3 vCamFront = cam->GetOwner()->Transform()->GetRelativeDir(DIR_TYPE::FRONT);
	
	for (i = 0; i < m_vecText.size(); ++i)
	{
		dt = m_vecText[i];
		vPos = dt.vPos;
		iRandSign = dt.iSign;
		dt.fAlpha = 1.f - (dt.fAcc / 5.f);
		if (dt.fAcc > 5.f)
		{
			m_vecText.erase(m_vecText.begin() + i);
			continue;
		}

		dt.fAcc += DT;

		if (iRandSign)
			vPos.x = vPos.x + 5.f * cosf(fTheta) * dt.fAcc;
		else
			vPos.x = vPos.x - 5.f * cosf(fTheta) * dt.fAcc;

		vPos.y = vPos.y + 10.f * sinf(fTheta) * dt.fAcc - ((0.5f * 9.81f) * (dt.fAcc * dt.fAcc));

		textTransMat = XMMatrixTranslation(vPos.x, vPos.y, vPos.z);
		BilBoardMat = Matrix::CreateBillboard(vPos, vCamPos, vCamUp);
		textWorldMat = textScaleMat * BilBoardMat * textTransMat;
		TextMgr::GetInst()->DrawSpriteText(std::to_wstring(dt.iDamage), vPos, 0.f, Vec2::Zero, textWorldMat, dt.fAlpha,true, 0.75f);

		m_vecText[i] = dt;
	}
}

void FieldUIMgr::AddDamage(int _Damage, Vec3 _vPos)
{
	m_vecText.push_back({
		_vPos,
		0.0f,
		1.0f,
		rand() % 2,
		_Damage
	});
}
