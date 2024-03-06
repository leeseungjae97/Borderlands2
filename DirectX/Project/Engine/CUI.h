#pragma once
#include "CGameObject.h"
#include "CTexture.h"
#include "ptr.h"

class CUI :
    public CGameObject
{
protected:
	wstring wsText;
	Vec2 vTextSize;
	float fTextScale;
	bool bHoverSound;
	bool bHoverSoundPlayed;
	Vec2 vTextOffset;
	Vec4 vTextColor;
	Vec4 vTextNormalColor;
	Vec4 vTextHoverColor;

	bool bHover;
	bool bOutline;

	bool bDraw3D;

	Ptr<CTexture> pFontTex;
public:
	void begin() override;
	virtual void finaltick() override;
	void render() override;

	void SetText(const std::wstring text) { wsText = text; }
	const std::wstring& GetText() { return wsText; }
	Vec2 GetTextSize() { return vTextSize; }
	void SetTextNormalColor(Vector4 color) { vTextNormalColor = color; }
	void SetTextClickColor(Vector4 color) { vTextHoverColor = color; }
	void SetTextSize(Vec2 textSize) { vTextSize = textSize; }
	void SetTextOffset(Vector2 textOffset) { vTextOffset = textOffset; }
	void SetTextScale(float _Scale) { fTextScale = _Scale; }
	void HoverSoundMute() { bHoverSound = false; }
	void HoverMakeSound() { bHoverSound = true; }
	void SetOutline(bool _Outline) { bOutline = _Outline; }
	void SetDraw3D(bool _Draw3D) { bDraw3D = _Draw3D; }
	Ptr<CTexture> GetTextAsTexture() { return pFontTex; }
private:
	void drawText();

public:
	CUI();
	virtual ~CUI();
};

