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

	bool bUseHover;
	bool bHover;
	bool bOutline;

	bool bDraw3D;

	Ptr<CTexture> pFontTex;
	std::shared_ptr<std::function<void()>> pFunction;

	CGameObject* pHoverImg;
public:
	void begin() override;
	virtual void finaltick() override;
	void render() override;

	std::shared_ptr<std::function<void()>>& SetClickFunc() { return pFunction; }
	void SetText(const std::wstring text) { wsText = text; }
	const std::wstring& GetText() { return wsText; }
	Vec2 GetTextSize() { return vTextSize; }

	void SetTextNormalColor(Vector4 color) { vTextNormalColor = color; }
	void SetTextHoverColor(Vector4 color) { vTextHoverColor = color; }

	void SetTextOffset(Vector2 textOffset) { vTextOffset = textOffset; }
	void SetTextScale(float _Scale) { fTextScale = _Scale; }
	void HoverSoundMute() { bHoverSound = false; }
	void HoverMakeSound() { bHoverSound = true; }
	bool IsHover() { return bHover; }

	void SetOutline(bool _Outline) { bOutline = _Outline; }
	void SetDraw3D(bool _Draw3D) { bDraw3D = _Draw3D; }
	void SetUseHover(bool _Hover) { bUseHover = _Hover; }

	bool IsUseHover() { return bUseHover; }
	Ptr<CTexture> GetTextAsTexture() { return pFontTex; }
private:
	void drawText();

public:
	CUI();
	virtual ~CUI();
};

