#include "pch.h"
#include "TextMgr.h"

#include "CDevice.h"
#include "CEngine.h"

TextMgr::TextMgr()
{
}

TextMgr::~TextMgr()
{
	m_spriteBatch.reset();
	m_font.reset();
}

void TextMgr::init()
{
	m_spriteBatch = std::make_unique<SpriteBatch>(CONTEXT);
	m_font = std::make_unique<SpriteFont>(DEVICE, L"..\\bin\\content\\font\\compacta_bold_bt.spritefont");
	//Vec2 m_fontPos;
	Vec2 resol = CEngine::GetInst()->GetWindowResolution();
	m_spriteBatch->SetRotation(DXGI_MODE_ROTATION_IDENTITY);
}

void TextMgr::DrawSpriteText(Vec2 m_fontPos)
{
	m_spriteBatch->Begin();

	const wchar_t* output = L"Hello World";

	Vector2 origin = m_font->MeasureString(output) / 2.f;

	m_font->DrawString(m_spriteBatch.get(), output,
		m_fontPos, Colors::White, 0.f, origin);

	m_spriteBatch->End();
}
