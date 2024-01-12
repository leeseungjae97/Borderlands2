#pragma once
#include "CEntity.h"

class CAnimator3D;

class CAnimClip
	: public CEntity
{
private:
	CAnimator3D*			m_pOwner;
	vector<tAnimClipFrame>	m_vecClip;

	int						m_iCurIdx;
	float					m_fTime;

	int						m_iStartIdx;
	int						m_iEndIdx;

	bool					m_bFinish;

	bool					m_bRepeat;

public:
	void finlatick();
	void Create(const std::wstring& name, vector<tMTAnimClip>& _AnimClips, int _StartIdx, int _EndIdx, bool _bRepeat);

	const tAnimClipFrame& GetCurFrame() { return m_vecClip[m_iCurIdx]; }
	int GetClipSize() { return m_vecClip.size(); }
	int GetClipEndFrame() { return m_iStartIdx + m_iEndIdx; }
	int GetClipStartIdx() { return m_iStartIdx; }
	int GetClipEndIdx() { return m_iEndIdx; }
	float GetClipTime() { return m_fTime; }
	bool IsFinish() { return m_bFinish; }

	void Reset()
	{
		m_iCurIdx = 0;
		m_fTime = 0.f;
		m_bFinish = false;
	}

	void SaveToLevelFile(FILE* _File);
	void LoadFromLevelFile(FILE* _File);

public:
	

public:
	CLONE(CAnimClip);

public:
	CAnimClip();
	~CAnimClip();
};

