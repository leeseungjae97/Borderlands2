#include "pch.h"
#include "CSimpleTextMgr.h"
#include "CDevice.h"


CSimpleTextMgr::CSimpleTextMgr()
	: m_pFW1Factory(nullptr)
	, m_pFontWrapper(nullptr)
{
}

CSimpleTextMgr::~CSimpleTextMgr()
{
	if (nullptr != m_pFW1Factory)
		m_pFW1Factory->Release();

	if (nullptr != m_pFontWrapper)
		m_pFontWrapper->Release();
}

void CSimpleTextMgr::init()
{
	if (FAILED(FW1CreateFactory(FW1_VERSION, &m_pFW1Factory)))
	{
		assert(NULL);
	}

	if (FAILED(m_pFW1Factory->CreateFontWrapper(DEVICE, L"Arial", &m_pFontWrapper)))
	{
		assert(NULL);
	}
}

void CSimpleTextMgr::DrawFont(const wchar_t* _pStr, float _fPosX, float _fPosY, float _fFontSize, UINT _Color)
{
	Vector2 vSize = GetTextSize(_pStr, _fFontSize);
	_fPosX += vSize.x / 2.f;
	
	m_pFontWrapper->DrawString(
		CONTEXT,
		_pStr, // String
		_fFontSize,// Font size
		_fPosX,// X position
		_fPosY,// Y position
		_Color,// Text color, 0xAaBbGgRr
		FW1_CENTER | FW1_RESTORESTATE
	);
}

Vec2 CSimpleTextMgr::GetTextSize(const wchar_t* str, float size)
{
	FW1_RECTF measuredSize = { 0.f, 0.f, 0.f, 0.f };

	FW1_RECTF mSize = m_pFontWrapper->MeasureString(str, L"HYGothic", size, &measuredSize, FW1_NOWORDWRAP);
	FLOAT width = mSize.Right - mSize.Left;
	FLOAT height = mSize.Bottom - mSize.Top;
	height += 5.f;
	width += 5.f;
	return Vec2(width, height);
}
