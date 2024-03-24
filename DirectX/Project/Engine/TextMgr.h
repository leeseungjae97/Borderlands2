#pragma once
#include "CSingleton.h"

#include <DirectXTK\SpriteFont.h>
#include <DirectXTK\SpriteBatch.h>

#include "CTexture.h"

#ifdef _DEBUG
#pragma comment(lib, "DirectXTK/debug/DirectXTK.lib")
#else 
#pragma comment(lib, "DirectXTK/release/DirectXTK.lib")
#endif

class TextMgr : public CSingleton<TextMgr>
{
private:
	std::unique_ptr<SpriteFont> m_font;
	std::unique_ptr<SpriteBatch> m_spriteBatch;

public:
	void init();
	void DrawSpriteText(const wstring& str, Vec2 m_fontPos, float _fRotate, Vec2 _vScale, Vec4 vColor, bool Outline = false, float fScale = 1.f);
	void DrawSpriteText(const wstring& str, Vec3 m_fontPos, Vec4 _vColor, float _fRotate, Vec2 _vScale, Matrix matWorld, float alpha = 1.f, bool Outline = false, float fScale = 1.f);
	Vec2 GetTextSize(const wstring& str, float _TextScale);

public:
	TextMgr();
	virtual ~TextMgr();

};