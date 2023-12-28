#include "pch.h"
#include "KeyUseInfoMgr.h"

#include "CFontMgr.h"

void KeyUseInfoMgr::render()
{
	static wchar_t szBuff[256] = {};
	swprintf_s(szBuff, L"Q : HighLight");
	CFontMgr::GetInst()->DrawFont(szBuff, 10.f, 36.f, 30.f, FONT_RGBA(255, 0, 0, 255));
}
