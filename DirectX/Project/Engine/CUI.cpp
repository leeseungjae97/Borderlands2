#include "pch.h"
#include "CUI.h"

#include "CFontMgr.h"

#include "CCamera.h"
#include "CEngine.h"
#include "CRenderMgr.h"
#include "CTransform.h"
#include "MouseMgr.h"


CUI::CUI()
	: CGameObject()
	, wsText(L"")
	, fTextSize(20.f)
	, vTextColor(Vector4(1.f, 1.f, 1.f, 1.f))
	, vTextHoverColor(Vector4(1.f, 1.f, 1.f, 1.f))
	, vTextNormalColor(Vector4(1.f, 1.f, 1.f, 1.f))
	, vTextOffset(Vector2::Zero)
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

	Vec2 vUnproPos = MouseMgr::GetInst()->UnprojectionMousePos(vPos.z);
	
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
	Vec2 fontSize = CFontMgr::GetInst()->GetTextSize(wsText.c_str(), fTextSize);

	Vec3 vPos = Transform()->GetRelativePos();
	vPos.x -= (fontSize.x / 2.f);
	vPos.y -= (fontSize.y / 2.f);
	//CCamera* camera = CRenderMgr::GetInst()->GetUICam();

	//Matrix proj = camera->GetProjMat();
	//Matrix view = camera->GetViewMat();

	//Vec2 vResol = CEngine::GetInst()->GetWindowResolution();

	//Viewport viewport = {
	//	0.f,
	//	0.f,
	//	vResol.x / 2.f,
	//	vResol.y / 2.f,
	//	10.f,
	//	-1.f,
	//};
	//Vec3 vCenterPos = viewport.Project(vPos, proj, view, Matrix::Identity);
	//vCenterPos.y -= fontSize.y / 2.f;
	//vCenterPos.x -= fontSize.x / 2.f;

	//vCenterPos.y += vTextOffset.y;
	//vCenterPos.x += vTextOffset.x;

	CFontMgr::GetInst()->DrawFont(wsText.c_str(), vPos.x, vPos.y, fTextSize
		, FONT_RGBA(255.f, 0.f, 0.f, 255.f));
}
