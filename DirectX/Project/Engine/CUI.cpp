#include "pch.h"
#include "CUI.h"

#include "CSimpleTextMgr.h"

#include "CCamera.h"
#include "CEngine.h"
#include "CRenderMgr.h"
#include "CTransform.h"
#include "MouseMgr.h"
#include "physx_util.h"
#include "TextMgr.h"


CUI::CUI()
	: CGameObject()
	, wsText(L"")
	, vTextSize(Vec2(20.f, 20.f))
	, vTextColor(Vector4(1.f, 1.f, 1.f, 1.f))
	, vTextHoverColor(Vector4(1.f, 1.f, 1.f, 1.f))
	, vTextNormalColor(Vector4(1.f, 1.f, 1.f, 1.f))
	, vTextOffset(Vector2::Zero)
	, bOutline(false)
	, fTextScale(1.f)
	, bHoverSound(false)
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

	if (bHover)
	{
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
		if (vTextHoverColor != Vec4::One)
			vTextColor = vTextHoverColor;
	}
	else
	{
		bHoverSoundPlayed = false;
		if (vTextNormalColor != Vec4::One)
			vTextColor = vTextNormalColor;
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
	//Vec2 fontSize = CSimpleTextMgr::GetInst()->GetTextSize(wsText.c_str(), fTextSize);

	Vec3 vPos = Transform()->GetRelativePos();
	Vec3 vRot = Transform()->GetRelativeRot();
	
	//Vec3 vScale = Transform()->GetRelativeScale();

	//vTextSize.x = vScale.x;
	//vTextSize.y = vScale.y;
	//vPos.x -= (fontSize.x / 2.f);
	//vPos.y -= (fontSize.y / 2.f);

	if(bDraw3D)
	{	
		//vPos = Transform()->GetWorldPos();
		vRot.z -= 180.f * Util::DegToRad();
		TextMgr::GetInst()->DrawSpriteText(wsText, vPos, vRot.z, vTextSize, Transform()->GetWorldMat(), bOutline, fTextScale);
	}
	else
		TextMgr::GetInst()->DrawSpriteText(wsText, Vec2(vPos.x, vPos.y), vRot.z, vTextSize, bOutline, fTextScale);


	//pFontTex = TextMgr::GetInst()->GetTextAsTexture();

	//CSimpleTextMgr::GetInst()->DrawFont(wsText.c_str(), vPos.x, vPos.y, fTextSize
	//	, FONT_RGBA(255.f, 0.f, 0.f, 255.f));
}
