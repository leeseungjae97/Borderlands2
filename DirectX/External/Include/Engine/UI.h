#pragma once
#include "CGameObject.h"

class UI :
    public CGameObject
{
protected:
	wstring wsText;
	float fTextSize;
	bool bHoverSound;
	bool bHoverSoundPlayed;
	Vec2 vTextOffset;
	Vec4 vTextColor;
	Vec4 vTextNormalColor;
	Vec4 vTextHoverColor;

public:
	void begine();
	virtual void finaltick() override;
	

	void SetText(const std::wstring text) { wsText = text; }
	const std::wstring& GetText() { return wsText; }
	float GetTextSize() { return fTextSize; }
	void SetTextNormalColor(Vector4 color) { vTextNormalColor = color; }
	void SetTextClickColor(Vector4 color) { vTextHoverColor = color; }
	void SetTextSize(float textSize) { fTextSize = textSize; }
	void SetTextOffset(Vector2 textOffset) { vTextOffset = textOffset; }

	void HoverSoundMute() { bHoverSound = false; }
	void HoverMakeSound() { bHoverSound = true; }

private:
	void drawText();

public:
	UI();
	virtual ~UI();
};

