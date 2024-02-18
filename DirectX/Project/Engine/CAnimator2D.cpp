#include "pch.h"
#include "CAnimator2D.h"

#include "CMaterial.h"
#include "CMeshRender.h"

CAnimator2D::CAnimator2D()
	: CComponent(COMPONENT_TYPE::ANIMATOR2D)
	, m_iMtrlIdx(0)
	, m_iLoopCount(0)
	, m_bLoop(false)

{
}

CAnimator2D::~CAnimator2D()
{
}

void CAnimator2D::finaltick()
{
	if (m_pActiveSprite == nullptr
		|| m_pActiveSprite->IsStop())
		return;

	if (m_pActiveSprite)
		m_pActiveSprite->finaltick();

	Events* events = FindEvents(m_pActiveSprite->GetName());

	if (m_pActiveSprite->IsComplete() && m_bLoop)
	{
		++m_iLoopCount;

		if (events)
			events->completeEvent();

		m_pActiveSprite->Reset();
	}
	if (m_pActiveSprite->IsProgress())
	{
		if (events)
			events->progressEvent();
	}
	if (m_pActiveSprite->IsComplete() && !m_bLoop)
	{
		if (events)
			events->endEvent();

		m_pActiveSprite->SetStop();
	}
}

void CAnimator2D::UpdateData()
{
	Ptr<CMaterial> pMtrl = MeshRender()->GetMaterial(m_iMtrlIdx);

	const CAnimSprite::Sprite curSprite = m_pActiveSprite->GetCurrentSprite();

	int iAnimUse = 1;
	pMtrl->SetScalarParam(INT_0, &iAnimUse);
	pMtrl->SetScalarParam(VEC2_0, &curSprite.leftTop);
	pMtrl->SetScalarParam(VEC2_1, &curSprite.size);
	pMtrl->SetScalarParam(VEC2_2, &curSprite.offset);
	pMtrl->SetScalarParam(VEC2_3, &curSprite.sheetSize);

	pMtrl->SetTexParam(TEX_0, m_pActiveSprite->GetSheet());
}

void CAnimator2D::ClearData()
{
	Ptr<CMaterial> pMtrl = MeshRender()->GetMaterial(m_iMtrlIdx);

	int iAnimUse = 0;
	pMtrl->SetScalarParam(INT_0, &iAnimUse);

	Ptr<CTexture> pTex = nullptr;
	pMtrl->SetTexParam(TEX_0, pTex);
}

void CAnimator2D::begin()
{

}

void CAnimator2D::Create(const std::wstring& name, Ptr<CTexture> sheet, Vector2 leftTop, Vector2 size,
	UINT columnLength, Vector2 offset, Vector2 offsetOfCenterPos, float duration, float alpha, int mtrlIdx)
{
	CAnimSprite* animSprite = FindAnimSprite(name);
	if (nullptr != animSprite)
		return;

	animSprite = new CAnimSprite;
	animSprite->SetAnimator(this);
	animSprite->SetName(name);

	//if (GetOwner()->MeshRender()->GetMaterial(m_iMtrlIdx) != nullptr)
	//	sheet = GetOwner()->MeshRender()->GetMaterial(m_iMtrlIdx)->GetTexParam(TEX_0);

	animSprite->Create(name
		, sheet
		, leftTop
		, size
		, columnLength
		, offset
		, offsetOfCenterPos
		, duration
		, alpha);

	mAnimations.insert(std::make_pair(name, animSprite));

	Events* events = FindEvents(name);
	if (nullptr != events)
		return;

	events = new Events();
	mEvents.insert(std::make_pair(name, events));

	m_iMtrlIdx = mtrlIdx;
}

void CAnimator2D::Create(const std::wstring& name, Ptr<CTexture> sheet, Vector2 leftTop, Vector2 size,
	UINT columnLength, UINT rowLength, Vector2 offset, Vector2 offsetOfCenterPos, float duration, float alpha, int mtrlIdx)
{
	CAnimSprite* animSprite = FindAnimSprite(name);
	if (nullptr != animSprite)
		return;

	animSprite = new CAnimSprite;
	animSprite->SetAnimator(this);
	animSprite->SetName(name);

	animSprite->Create(name
		, sheet
		, leftTop
		, size
		, columnLength
		, rowLength
		, offset
		, offsetOfCenterPos
		, duration
		, alpha);

	mAnimations.insert(std::make_pair(name, animSprite));

	Events* events = FindEvents(name);
	if (nullptr != events)
		return;

	events = new Events();
	mEvents.insert(std::make_pair(name, events));

	m_iMtrlIdx = mtrlIdx;
}

void CAnimator2D::Create(const std::wstring& name, Ptr<CTexture> sheet, Vector2 leftTop, Vector2 size,
	UINT columnLength, UINT rowLength, Vector2 offset, Vector2 offsetOfCenterPos, int mtrlIdx)
{
	CAnimSprite* animSprite = FindAnimSprite(name);
	if (nullptr != animSprite)
		return;

	animSprite = new CAnimSprite;
	animSprite->SetAnimator(this);
	animSprite->SetName(name);

	animSprite->Create(name
		, sheet
		, leftTop
		, size
		, columnLength
		, rowLength
		, offset
		, offsetOfCenterPos
	);

	mAnimations.insert(std::make_pair(name, animSprite));

	Events* events = FindEvents(name);
	if (nullptr != events)
		return;

	events = new Events();
	mEvents.insert(std::make_pair(name, events));

	m_iMtrlIdx = mtrlIdx;
}

void CAnimator2D::Create(const std::wstring& name, Ptr<CTexture> sheet, Vector2 leftTop, Vector2 size,
                         UINT columnLength, Vector2 offset, float duration, float alpha, int mtrlIdx)
{
	CAnimSprite* animSprite = FindAnimSprite(name);
	if (nullptr != animSprite)
		return;

	animSprite = new CAnimSprite();
	animSprite->SetAnimator(this);
	animSprite->SetName(name);

	animSprite->Create(name
		, sheet
		, leftTop
		, size
		, columnLength
		, offset
		, duration
		, alpha);

	mAnimations.insert(std::make_pair(name, animSprite));

	Events* events = FindEvents(name);
	if (nullptr != events)
		return;

	events = new Events();
	mEvents.insert(std::make_pair(name, events));

	m_iMtrlIdx = mtrlIdx;
}

CAnimSprite* CAnimator2D::FindAnimSprite(const std::wstring& name)
{
	std::map<std::wstring, CAnimSprite*>::iterator iter
		= mAnimations.find(name);

	if (iter == mAnimations.end())
		return nullptr;

	return iter->second;
}

Events* CAnimator2D::FindEvents(const std::wstring& name)
{
	std::map<std::wstring, Events*>::iterator iter
		= mEvents.find(name);

	if (iter == mEvents.end())
		return nullptr;

	return iter->second;
}

void CAnimator2D::Play(const std::wstring& name, bool loop)
{
	m_iLoopCount = 0;
	CAnimSprite* prevAnimation = m_pActiveSprite;
	Events* events;
	if (prevAnimation != nullptr)
	{
		prevAnimation->Reset();
		events = FindEvents(prevAnimation->GetName());
		if (events)
			events->endEvent();
	}


	CAnimSprite* animation = FindAnimSprite(name);
	if (animation)
	{
		m_pActiveSprite = animation;

	}else
	{
		return;
	}

	events = FindEvents(m_pActiveSprite->GetName());
	if (events)
		events->startEvent();

	m_bLoop = loop;

	m_pActiveSprite->Reset();
}

std::shared_ptr<std::function<void()>>& CAnimator2D::StartEvent(const std::wstring key)
{
	Events* events = FindEvents(key);
	return events->startEvent.mEvent;
}

std::shared_ptr<std::function<void()>>& CAnimator2D::CompleteEvent(const std::wstring key)
{
	Events* events = FindEvents(key);
	return events->completeEvent.mEvent;
}

std::shared_ptr<std::function<void()>>& CAnimator2D::EndEvent(const std::wstring key)
{
	Events* events = FindEvents(key);
	return events->endEvent.mEvent;
}

std::shared_ptr<std::function<void()>>& CAnimator2D::ProgressEvent(const std::wstring key)
{
	Events* events = FindEvents(key);
	return events->progressEvent.mEvent;
}

void CAnimator2D::LoadFromLevelFile(FILE* _FILE)
{
	UINT mapSize = mAnimations.size();
	fread(&mapSize, sizeof(UINT), 1, _FILE);
	for (int i = 0; i < mapSize; ++i)
	{
		wstring str = L"";
		LoadWString(str, _FILE);

		CAnimSprite* animSprite = new CAnimSprite;
		animSprite->LoadFromLevelFile(_FILE);

		mAnimations.insert(make_pair(str, animSprite));
	}

	//std::map<std::wstring, Events*> mEvents;
	//Ptr<CTexture> m_pSpriteSheet;
	//CAnimSprite* m_pActiveSprite;

	SaveResRef(m_pSpriteSheet.Get(), _FILE);
	fread(&m_iLoopCount, sizeof(int), 1, _FILE);
	fread(&m_bLoop, sizeof(bool), 1, _FILE);
}

void CAnimator2D::SaveToLevelFile(FILE* _File)
{
	UINT mapSize = 0;
	fwrite(&mapSize, sizeof(UINT), 1, _File);
	for (auto pair : mAnimations)
	{
		SaveWString(pair.first, _File);
		pair.second->SaveToLevelFile(_File);
	}

	fwrite(&m_iLoopCount, sizeof(int), 1, _File);
	fwrite(&m_bLoop, sizeof(bool), 1, _File);
}
