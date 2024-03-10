#include "pch.h"
#include "CUI.h"

#include "CSimpleTextMgr.h"

#include "CCamera.h"
#include "CEngine.h"
#include "CKeyMgr.h"
#include "CMeshRender.h"
#include "CRenderMgr.h"
#include "CResMgr.h"
#include "CTransform.h"
#include "MouseMgr.h"
#include "physx_util.h"
#include "TextMgr.h"


CUI::CUI()
	: CGameObject()
	, wsText(L"")
	, vTextColor(Vector4(1.f, 1.f, 1.f, 1.f))
	, vTextHoverColor(Vector4(1.f, 1.f, 1.f, 1.f))
	, vTextNormalColor(Vector4(1.f, 1.f, 1.f, 1.f))
	, vTextOffset(Vector2::Zero)
	, bOutline(false)
	, bDraw3D(false)
	, fTextScale(1.f)
	, bHoverSound(false)
	, bUseHover(false)
	, bHoverSoundPlayed(false)
{
	
}
CUI ::~CUI()
{
	
}

void CUI::begin()
{
	CGameObject::begin();
	
}

void CUI::finaltick()
{
	CGameObject::finaltick();
	if (GetObjectState() == OBJECT_STATE::INVISIBLE)
	{
		bHover = false;
		return;
	}
	;
	Vec3 vPos = Transform()->GetRelativePos();
	Vec3 vScale = Transform()->GetRelativeScale();

	Vec2 vPosV2 = Vec2(vPos.x, vPos.y);
	Vec2 vScaleV2 = Vec2(vScale.x, vScale.y);

	Vec2 vUnproPos = MouseMgr::GetInst()->UnprojectionMouse2DPos(vPos.z);

	if (Vec2::PointIntersectRect(vPosV2
		, vScaleV2
		, vUnproPos))
	{
		bHover = true;
	}
	else bHover = false;

	if(bUseHover)
	{
		if (bHover)
		{
			if (KEY_TAP(KEY::LBTN))
			{
				if (pFunction)
					(*pFunction)();
			}
			if (bHoverSound)
			{
				//AudioSource* as = GET_COMP(this, AudioSource);
				//if (!bHoverSoundPlayed)
				//{
				//	if (as)
				//		as->Play(eUISoundType::ButtonHover);
				//	bHoverSoundPlayed = true;
				//}
			}
			vTextColor = vTextHoverColor;
		}
		else
		{
			bHoverSoundPlayed = false;
			vTextColor = vTextNormalColor;
		}
	}
}

void CUI::render()
{
	CGameObject::render();
	drawText();
}

void CUI::drawText()
{
	if (wsText == L"") return;

	Vec3 vPos = Transform()->GetRelativePos();
	Vec3 vRot = Transform()->GetRelativeRot();
	Vec2 vResol = CEngine::GetInst()->GetWindowResolution();
	vResol /= 2.f;

	Vec2 vSize = TextMgr::GetInst()->GetTextSize(wsText, fTextScale);
	vTextSize = vSize;
	Vec2 vCenterPos = Vec2(vResol.x - (vSize.x / 2.f), vResol.y - (vSize.y / 2.f));

	if(bDraw3D)
	{
		TextMgr::GetInst()->DrawSpriteText(wsText, Vec3::Zero, vRot.z, Vec2::Zero, Transform()->GetWorldMat(),1.0f,  bOutline, fTextScale);
	}
	else
		TextMgr::GetInst()->DrawSpriteText(wsText, vCenterPos + Vec2(vPos.x, -vPos.y), vRot.z, Vec2::Zero, Vec3(vTextColor.x, vTextColor.y, vTextColor.z), bOutline, fTextScale);
	
	Transform()->SetRelativeScale(Vec3(vSize.x, vSize.y, 1.f));
	DrawDebugRect(vPos, Vec2(vSize.x, vSize.y), Vec4(0.f, 1.f, 0.f, 1.f), vRot, 0.f, false);
}
