#pragma once
#include "CEntity.h"

#include "ptr.h"
#include "CTexture.h"

class CAnimSprite
	: public CEntity
{
public:
	struct Sprite {
		Vec2 leftTop;
		Vec2 size;
		Vec2 offset;
		Vec2 sheetSize;
		Vec2 offsetOfCenterPos;

		float duration;
		float alpha;

		Sprite()
			: leftTop(Vec2::Zero)
			, size(Vec2::Zero)
			, offset(Vec2::Zero)
			, sheetSize(Vec2::Zero)
			, offsetOfCenterPos(Vec2::Zero)
			, duration(0.0f)
		{
		}

		void SaveToLevelFile(FILE* _File)
		{
			fwrite(&leftTop, sizeof(Vec2), 1, _File);
			fwrite(&size, sizeof(Vec2), 1, _File);
			fwrite(&offset, sizeof(Vec2), 1, _File);
			fwrite(&sheetSize, sizeof(Vec2), 1, _File);
			fwrite(&offsetOfCenterPos, sizeof(Vec2), 1, _File);

			fwrite(&duration, sizeof(float), 1, _File);
			fwrite(&alpha, sizeof(float), 1, _File);
		}

		void LoadToLevelFile(FILE* _File)
		{
			fread(&leftTop, sizeof(Vec2), 1, _File);
			fread(&size, sizeof(Vec2), 1, _File);
			fread(&offset, sizeof(Vec2), 1, _File);
			fread(&sheetSize, sizeof(Vec2), 1, _File);
			fread(&offsetOfCenterPos, sizeof(Vec2), 1, _File);

			fread(&duration, sizeof(float), 1, _File);
			fread(&alpha, sizeof(float), 1, _File);
		}
	};

private:
	Ptr<CTexture> mSheet;

	class CAnimator2D* mAnimator;
	vector<Sprite> mSprites;

	int mSpriteLength;
	int mSpriteEndIndex;
	int mSpriteCurIndex;
	int mCustomEndIndex;
	int mCustomStartIndex;
	int mCustomProgressIndex;

	float mTime;
	bool mbComplete;
	bool mbProgress;
	bool mbStop;
	bool mbReverse;

public:
	void Binds();
	void Reset();

	void SetStop() { mbStop = true; }
	void SetReverse(bool loop);
	void SetCurIndex(int index) { mSpriteCurIndex = index; }
	void SetCustomStartIndex(int index) { mCustomStartIndex = index; }
	void SetCustomProgressIndex(int index) { mCustomProgressIndex = index; }
	void SetCustomEndIndex(int index) { mCustomEndIndex = index; }

	const Sprite& GetCurrentSprite() { return mSprites[mSpriteCurIndex]; }
	std::vector<Sprite>& GetSprites() { return mSprites; }
	Ptr<CTexture> GetSheet() { return mSheet; }

	int GetSpriteLength() { return mSprites.size(); }
	int GetCurIndex() { return mSpriteCurIndex; }
	int GetCustomProgressIndex() { return mCustomProgressIndex; }
	int GetCustomEndIndex() { return mSpriteEndIndex; }

	bool IsComplete() { return mbComplete; }
	bool IsProgress() { return mbProgress; }
	bool IsStop() { return mbStop; }

	void SetAnimator(CAnimator2D* animator) { mAnimator = animator; }

public:
	void Create(const wstring& name
		, Ptr<CTexture> sheet
		, Vector2 leftTop
		, Vector2 size
		, UINT columnLength
		, Vector2 offset
		, Vector2 offsetOfCenterPos
		, float duration
		, float alpha
	);

	void Create(const wstring& name
		, Ptr<CTexture> sheet
		, Vector2 leftTop
		, Vector2 size
		, UINT columnLength
		, UINT rowLength
		, Vector2 offset
		, Vector2 offsetOfCenterPos
		, float duration
		, float alpha
	);

	void Create(const wstring& name
		, Ptr<CTexture> sheet
		, Vector2 leftTop
		, Vector2 size
		, UINT columnLength
		, UINT rowLength
		, Vector2 offset
		, Vector2 offsetOfCenterPos
	);

	void Create(std::wstring name
		, Ptr<CTexture> sheet
		, Vector2 leftTop
		, Vector2 size
		, UINT columnLength
		, Vector2 offset
		, float duration
		, float alpha
	);

public:
	void finaltick();
	void LoadFromLevelFile(FILE* _File);
	void SaveToLevelFile(FILE* _File);

public:
	CLONE(CAnimSprite);
public:
	CAnimSprite();
	virtual ~CAnimSprite();
};

