#pragma once
#include "CComponent.h"

#include "ptr.h"
#include "CTexture.h"
#include "CAnimSprite.h"

class CAnimator2D :
    public CComponent
{

private:
	std::map<std::wstring, CAnimSprite*> mAnimations;
	std::map<std::wstring, Events*> mEvents;
	Ptr<CTexture> m_pSpriteSheet;
	CAnimSprite* m_pActiveSprite;

	int m_iLoopCount;
	bool m_bLoop;

	int m_iMtrlIdx;
public:
	CAnimSprite* FindAnimSprite(const std::wstring& name);
	Events* FindEvents(const std::wstring& name);
	void Play(const std::wstring& name, bool loop);

public:
	std::shared_ptr<std::function<void()>>& StartEvent(const std::wstring key);
	std::shared_ptr<std::function<void()>>& CompleteEvent(const std::wstring key);
	std::shared_ptr<std::function<void()>>& EndEvent(const std::wstring key);
	std::shared_ptr<std::function<void()>>& ProgressEvent(const std::wstring key);

public:
	void SetSpriteStartIndex(int index) { m_pActiveSprite->SetCustomStartIndex(index); }
	void SetSpriteProgressIndex(int index) { m_pActiveSprite->SetCustomProgressIndex(index); }
	void SetSpriteEndIndex(int index) { index != 0 ? m_pActiveSprite->SetCustomEndIndex(index) : void(); }
	void SetSpriteIndex(int index) { m_pActiveSprite->SetCurIndex(index); }
	int GetSpriteIndex() { return m_pActiveSprite->GetCurIndex(); }

	void SetAnimations(std::map<std::wstring, CAnimSprite*> animations) { mAnimations = animations; }
	void SetEvents(std::map<std::wstring, Events*> events) { mEvents = events; }
	void SetSpriteSheet(Ptr<CTexture> sheet) { m_pSpriteSheet = sheet; }
	void SetActiveAnimation(CAnimSprite* animation) { m_pActiveSprite = animation; }

	CAnimSprite* GetActiveAnimation() { return m_pActiveSprite; }
	Ptr<CTexture>& GetCurSpriteSheet() { return m_pSpriteSheet; }

public:
    void Create(const std::wstring& name
		, Ptr<CTexture> sheet
		, Vec2 leftTop
		, Vec2 size
		, UINT columnLength
		, Vec2 offset
		, Vec2 offsetOfCenterPos
		, float duration
		, float alpha
		, int mtrlIdx = 0
	);

	void Create(const std::wstring& name
		, Ptr<CTexture> sheet
		, Vec2 leftTop
		, Vec2 size
		, UINT columnLength
		, Vec2 offset
		, float duration
		, float alpha
		, int mtrlIdx = 0
	);
	void Create(const std::wstring& name
		, Ptr<CTexture> sheet
		, Vec2 leftTop
		, Vec2 size
		, UINT columnLength
		, UINT rowLength
		, Vec2 offset
		, Vec2 offsetOfCenterPos
		, float duration
		, float alpha
		, int mtrlIdx = 0
	);

public:
	void begin() override;
    void finaltick() override;
	void UpdateData();
	void ClearData();

    void LoadFromLevelFile(FILE* _FILE) override;
    void SaveToLevelFile(FILE* _File) override;

public:
    CLONE(CAnimator2D);

public:
    CAnimator2D();
    virtual ~CAnimator2D();
};


