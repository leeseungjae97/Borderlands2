#pragma once
#include "CSingleton.h"

#include <DirectXTK\SpriteFont.h>
#include <DirectXTK\SpriteBatch.h>

#ifdef _DEBUG
#pragma comment(lib, "DirectXTK/debug/DirectXTK.lib")
#else 
#pragma comment(lib, "DirectXTK/release/DirectXTK.lib")
#endif


class TextMgr
	: public CSingleton<TextMgr>
{
private:
	std::unique_ptr<SpriteFont> m_font;
	std::unique_ptr<SpriteBatch> m_spriteBatch;

public:
	void init();
	void DrawSpriteText(Vec2 m_fontPos);

public:
	TextMgr();
	virtual ~TextMgr();
};

