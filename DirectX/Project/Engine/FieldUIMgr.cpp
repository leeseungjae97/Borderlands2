#include "pch.h"
#include "FieldUIMgr.h"

#include "CMeshRender.h"
#include "CResMgr.h"
#include "CTransform.h"

#include <Script/CScriptMgr.h>

#include "CCamera.h"
#include "CEventMgr.h"
#include "CKeyMgr.h"
#include "CLevel.h"
#include "CLevelMgr.h"
#include "CRenderMgr.h"
#include "CTimeMgr.h"
#include "physx_util.h"
#include "TextMgr.h"

FieldUIMgr::FieldUIMgr()
	: pLevelName(nullptr)
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
	static bool spawn = false;
	if(CEventMgr::GetInst()->IsLevelLoad())
	{
		spawn = false;
		if (CLevelMgr::GetInst()->GetCurLevel()->GetName() == L"main menu level")
			return;

		if (nullptr == pLevelName)
		{
			pLevelName = new CUI;
			pLevelName->SetName(L"Level Name");
			pLevelName->AddComponent(new CTransform);
			pLevelName->Transform()->SetRelativeScale(Vec3(100.f, 100.f, 1.f));
			pLevelName->SetTextScale(2.0f);
			pLevelName->SetOutline(true);
			pLevelName->SetTextNormalColor(Vec4(1.f, 1.f, 1.f, 1.f));
		}
		
		

		//if (CLevelMgr::GetInst()->GetCurLevel()->GetName() == L"main level 1")
		//{
		//	SpawnGameObject(pLevelName, Vec3(0.f, 100.f, 1.f), LAYER_TYPE::ViewPortUI);
		//	pLevelName->SetObjectState(CGameObject::OBJECT_STATE::VISIBLE);
		//	pLevelName->SetText(L"Hyperion City");
		//	pLevelName->Transform()->SetRelativePos(Vec3(0.f, 100.f, 0.f));
		//	spawn = true;
		//}
		//if (CLevelMgr::GetInst()->GetCurLevel()->GetName() == L"main level 2")
		//{
		//	SpawnGameObject(pLevelName, Vec3(0.f, 100.f, 1.f), LAYER_TYPE::ViewPortUI);
		//	pLevelName->SetObjectState(CGameObject::OBJECT_STATE::VISIBLE);
		//	pLevelName->SetText(L"Vault of The Warrior");
		//	pLevelName->Transform()->SetRelativePos(Vec3(0.f, 100.f, 0.f));
		//	spawn = true;
		//}
	}

	static float mT = 0.0f;

	if(spawn)
	{
		mT += DT;
		if (pLevelName)
		{
			Vec3 vPos = pLevelName->Transform()->GetRelativePos();
			vPos.y += DT * 10.f;
			pLevelName->Transform()->SetRelativePos(vPos);
		}

		if (mT > 10.f)
		{
			if (pLevelName)
			{
				pLevelName->SetObjectState(CGameObject::OBJECT_STATE::INVISIBLE);
			}

			mT = 0.0f;
		}
	}

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
		TextMgr::GetInst()->DrawSpriteText(dt.wsText, Vec3::Zero, dt.vColor, 0.f, Vec2::Zero, BilBoardMat, dt.fAlpha,true, 0.75f);

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
		Vec4(1.f, 1.f, 1.f, 1.f),
		std::to_wstring(_Damage)
	});
}

void FieldUIMgr::AddText(const wstring& _Text, Vec3 _vPos)
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
		Vec4(1.f, 0.f, 0.f, 1.f),
		_Text
		});
}
