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
	int						m_iNextIdx;
	float					m_fTime;

	double					m_dStartTime;
	double					m_dEndTime;

	int						m_iStartIdx;
	int						m_iEndIdx;

	bool					m_bFinish;
	bool					m_bLoop;

	float					m_fRatio;

	int						m_iFramePerSecond;

public:
	void finlatick();
	void Create(const std::wstring& name, vector<tMTAnimClip>& _AnimClips, int _StartIdx, int _EndIdx, bool _bRepeat);

	const tAnimClipFrame& GetCurFrame() { return m_vecClip[m_iCurIdx]; }

	int GetClipFrame() { return m_iCurIdx; }
	int GetClipNextFrame() { return m_iNextIdx; }

	int GetClipSize() { return m_vecClip.size(); }
	int GetClipEndFrame() { return m_iStartIdx + m_iEndIdx; }
	int GetClipStartIdx() { return m_iStartIdx; }
	int GetClipEndIdx() { return m_iEndIdx; }

	float GetClipTime() { return m_fTime; }

	bool IsFinish() { return m_bFinish; }
	bool IsLoop() { return m_bLoop; }

	double GetStartTime() { return m_dStartTime; }
	double GetEndTime() { return m_dEndTime; }

	void SetStartTime(double _Time) { m_dStartTime = _Time; }
	void SetEndTime(double _Time) { m_dEndTime = _Time; }

	void SetFPS(int _Count) { m_iFramePerSecond = _Count; }
	int GetFPS() { return m_iFramePerSecond; }

	float GetRatio() { return m_fRatio; }
	void Reset()
	{
		m_iCurIdx = 0;
		m_fTime = 0.f;
		m_bFinish = false;
	}

	void SaveToLevelFile(FILE* _File);
	void LoadFromLevelFile(FILE* _File);

public:
	CLONE(CAnimClip);

public:
	CAnimClip();
	~CAnimClip();
};

