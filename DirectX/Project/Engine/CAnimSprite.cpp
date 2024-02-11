#include "pch.h"
#include "CAnimSprite.h"

#include "CTimeMgr.h"
CAnimSprite::CAnimSprite()
	: mSpriteLength(0)
	, mSpriteEndIndex(0)
	, mSpriteCurIndex(0)
	, mCustomEndIndex(0)
	, mCustomStartIndex(0)
	, mCustomProgressIndex(0)
	, mTime(0.0f)
	, mbComplete(false)
	, mbProgress(false)
	, mbStop(false)
	, mbReverse(false)
{
	SetName(L"AnimSprite");
}

CAnimSprite::~CAnimSprite()
{
}

void CAnimSprite::finaltick()
{
	if (mbComplete || mbStop)
		return;

	mTime += DT;
	if (mSpriteCurIndex >= mSprites.size())
	{
		mSpriteCurIndex = 0;
	}
	if (mSprites[mSpriteCurIndex].duration <= mTime)
	{
		mTime = 0.0f;
		if (mbReverse)
		{
			--mSpriteCurIndex;

			if (mCustomStartIndex >= mSpriteCurIndex)
			{
				mSpriteCurIndex = mCustomStartIndex;
				mbComplete = true;
			}
		}
		else
		{
			++mSpriteCurIndex;

			if (mSpriteEndIndex <= mSpriteCurIndex)
			{
				mSpriteCurIndex = mSpriteEndIndex - 1;
				mbComplete = true;
			}
		}
	}

	if (mSpriteCurIndex == mCustomProgressIndex)
	{
		mbProgress = true;
	}
}

void CAnimSprite::LoadFromLevelFile(FILE* _File)
{
	LoadResRef(mSheet, _File);

	for (int i = 0; i < mSprites.size(); ++i)
	{
		mSprites[i].LoadToLevelFile(_File);
	}
	fread(&mSpriteLength, sizeof(int), 1, _File);
	fread(&mSpriteEndIndex, sizeof(int), 1, _File);
	fread(&mSpriteCurIndex, sizeof(int), 1, _File);
	fread(&mCustomEndIndex, sizeof(int), 1, _File);
	fread(&mCustomStartIndex, sizeof(int), 1, _File);
	fread(&mCustomProgressIndex, sizeof(int), 1, _File);

	//fread(&mTime, sizeof(float), 1, _File);

	fread(&mbComplete, sizeof(bool), 1, _File);
	fread(&mbProgress, sizeof(bool), 1, _File);
	fread(&mbStop, sizeof(bool), 1, _File);
	fread(&mbReverse, sizeof(bool), 1, _File);
}

void CAnimSprite::SaveToLevelFile(FILE* _File)
{
	SaveResRef(mSheet.Get(), _File);

	for(int i = 0 ; i < mSprites.size(); ++i)
	{
		mSprites[i].SaveToLevelFile(_File);
	}
	fwrite(&mSpriteLength, sizeof(int), 1, _File);
	fwrite(&mSpriteEndIndex, sizeof(int), 1, _File);
	fwrite(&mSpriteCurIndex, sizeof(int), 1, _File);
	fwrite(&mCustomEndIndex, sizeof(int), 1, _File);
	fwrite(&mCustomStartIndex, sizeof(int), 1, _File);
	fwrite(&mCustomProgressIndex, sizeof(int), 1, _File);

	//fwrite(&mTime, sizeof(float), 1, _File);

	fwrite(&mbComplete, sizeof(bool), 1, _File);
	fwrite(&mbProgress, sizeof(bool), 1, _File);
	fwrite(&mbStop, sizeof(bool), 1, _File);
	fwrite(&mbReverse, sizeof(bool), 1, _File);
}

void CAnimSprite::Create(const wstring& name, Ptr<CTexture> sheet, Vector2 leftTop, Vector2 size, UINT columnLength,
                         Vector2 offset, Vector2 offsetOfCenterPos, float duration, float alpha)
{
	SetName(name);
	mSheet = sheet;

	float width = (float)sheet->Width();
	float height = (float)sheet->Height();
	mSpriteLength = columnLength;

	mSpriteEndIndex = columnLength;

	for (size_t i = 0; i < columnLength; ++i)
	{
		Sprite sprite = {};
		sprite.leftTop.x = leftTop.x + (i * size.x) / width;
		sprite.leftTop.y = leftTop.y / height;
		sprite.size.x = size.x / width;
		sprite.size.y = size.y / height;
		sprite.offset.x = offset.x / width;
		sprite.offset.y = offset.y / height;
		sprite.offsetOfCenterPos.x = offsetOfCenterPos.x / width;
		sprite.offsetOfCenterPos.y = offsetOfCenterPos.y / height;
		sprite.sheetSize = Vector2(size.x / width, size.y / height);
		sprite.duration = duration;
		sprite.alpha = alpha;

		mSprites.push_back(sprite);
	}

}

void CAnimSprite::Create(const wstring& name, Ptr<CTexture> sheet, Vector2 leftTop, Vector2 size
	, UINT columnLength, UINT rowLength,
	Vector2 offset, Vector2 offsetOfCenterPos, float duration, float alpha)
{
	SetName(name);
	mSheet = sheet;

	float width = (float)sheet->Width();
	float height = (float)sheet->Height();
	mSpriteLength = rowLength * columnLength;
	mSpriteEndIndex = rowLength * columnLength;

	for(size_t k = 0 ; k < rowLength; ++k)
	{
		for (size_t i = 0; i < columnLength; ++i)
		{
			Sprite sprite = {};
			sprite.leftTop.x = leftTop.x + (i * size.x) / width;
			sprite.leftTop.y = leftTop.y + (k * size.y)/ height;
			sprite.size.x = size.x / width;
			sprite.size.y = size.y / height;
			sprite.offset.x = offset.x / width;
			sprite.offset.y = offset.y / height;
			sprite.offsetOfCenterPos.x = offsetOfCenterPos.x / width;
			sprite.offsetOfCenterPos.y = offsetOfCenterPos.y / height;
			sprite.sheetSize = Vector2(size.x / width, size.y / height);
			sprite.duration = duration;
			sprite.alpha = alpha;

			mSprites.push_back(sprite);
		}
	}
}

void CAnimSprite::Create(const wstring& name, Ptr<CTexture> sheet, Vector2 leftTop, Vector2 size, UINT columnLength,
	UINT rowLength, Vector2 offset, Vector2 offsetOfCenterPos)
{
	SetName(name);
	mSheet = sheet;

	float width = (float)sheet->Width();
	float height = (float)sheet->Height();
	mSpriteLength = rowLength * columnLength;
	mSpriteEndIndex = rowLength * columnLength;

	Sprite sprite = {};
	sprite.leftTop.x = leftTop.x / width;
	sprite.leftTop.y = leftTop.y / height;
	sprite.size.x = size.x / width;
	sprite.size.y = size.y / height;
	sprite.offset.x = offset.x / width;
	sprite.offset.y = offset.y / height;
	sprite.offsetOfCenterPos.x = offsetOfCenterPos.x / width;
	sprite.offsetOfCenterPos.y = offsetOfCenterPos.y / height;
	sprite.sheetSize = Vector2(size.x / width, size.y / height);
	sprite.duration = 1.f;
	sprite.alpha = 1.f;

	mSprites.push_back(sprite);
}

void CAnimSprite::Create(std::wstring name, Ptr<CTexture> sheet, Vector2 leftTop, Vector2 size, UINT columnLength,
                         Vector2 offset, float duration, float alpha)
{
	SetName(name);
	mSheet = sheet;

	float width = (float)sheet->Width();
	float height = (float)sheet->Height();
	mSpriteLength = columnLength;
	mSpriteEndIndex = columnLength;

	for (size_t i = 0; i < columnLength; ++i)
	{
		Sprite sprite = {};
		sprite.leftTop.x = leftTop.x + (i * size.x) / width;
		sprite.leftTop.y = leftTop.y / height;
		sprite.size.x = size.x / width;
		sprite.size.y = size.y / height;
		sprite.offset.x = offset.x / width;
		sprite.offset.y = offset.y / height;
		sprite.sheetSize = Vector2(size.x / width, size.y / height);
		sprite.duration = duration;
		sprite.alpha = alpha;

		mSprites.push_back(sprite);
	}
}

void CAnimSprite::Binds()
{
	//data.spriteLeftTop = mSprites[mSpriteCurIndex].leftTop;
	//data.spriteSize = mSprites[mSpriteCurIndex].size;
	//data.spriteOffset = mSprites[mSpriteCurIndex].offset;
	//data.sheetSize = mSprites[mSpriteCurIndex].sheetSize;
	//data.spriteOffsetOfCenterPos = mSprites[mSpriteCurIndex].offsetOfCenterPos;
	//data.animationType = 1;
	//data.alpha = mSprites[mSpriteCurIndex].alpha;

	//ConstantBuffer* cb = renderer::constantBuffers[(UINT)eCBType::Animator];
	//cb->SetData(&data);
	//

	//cb->Bind(eShaderStage::VS);
	//cb->Bind(eShaderStage::PS);
}

void CAnimSprite::Reset()
{
	mTime = 0.0f;
	mbComplete = false;
	mbProgress = false;
	mbStop = false;
	if(mCustomStartIndex == 0)
	{
		mSpriteCurIndex = 0;
		//mSpriteEndIndex = mSpriteLength;
	}else
	{
		if (mbReverse)
		{
			mSpriteCurIndex = mCustomEndIndex == 0 ? mSprites.size() : mCustomEndIndex;
			mSpriteEndIndex = mCustomStartIndex;
		}
		else
		{
			mSpriteCurIndex = mCustomStartIndex;
			mSpriteEndIndex = mCustomEndIndex == 0 ? mSprites.size() : mCustomEndIndex;
		}
	}
	
}

void CAnimSprite::SetReverse(bool loop)
{
}