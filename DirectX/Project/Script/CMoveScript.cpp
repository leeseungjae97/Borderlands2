#include "pch.h"
#include "CMoveScript.h"

#include <Engine/CUI.h>
#include <Engine/TextMgr.h>

CMoveScript::CMoveScript()
	: CScript(SCRIPT_TYPE::MOVE_SCRIPT)
	, fMoveAcc(0.f)
	, fMoveTime(10.f)
	, fSpeed(5.f)
{
}

CMoveScript::~CMoveScript()
{
}

void CMoveScript::tick()
{
	//CUI* ui = static_cast<CUI*>(GetOwner());
	//if(ui)
	//{
	//	Ptr<CTexture> tex = ui->GetTextAsTexture();
	//	GetOwner()->MeshRender()->GetDynamicMaterial(0)->SetTexParam(TEX_0, tex);
	//}
}

void CMoveScript::finaltick()
{
	Vec3 vPos = GetOwner()->Transform()->GetRelativePos();
	fMoveAcc += DT;

	float theta = 80.f * DegToRad();

	if(iRandSign)
		vPos.x = vPos.x + fSpeed * cosf(theta) * fMoveAcc;
	else
		vPos.x = vPos.x - fSpeed * cosf(theta) * fMoveAcc;

	vPos.y = vPos.y + fSpeed * sinf(theta) * fMoveAcc - ((0.5f * 9.81f) * (fMoveAcc * fMoveAcc));

	if(fMoveAcc >= fMoveTime)
	{
		DestroyObject(GetOwner());
	}

	GetOwner()->Transform()->SetRelativePos(vPos);
}

void CMoveScript::begin()
{
	iRandSign = rand() % 2;
}