#pragma once
#include "CEntity.h"
#include "ptr.h"

class CAnimator3D;

class CAnimClip
	: public CEntity
{
private:
	CAnimator3D*			m_pOwner;
	tMTAnimClip				m_Clip;

	int						m_iCurIdx;
	int						m_iNextIdx;
	float					m_fTime;

	bool					m_bFinish;
	bool					m_bLoop;

	float					m_fRatio;

	int						m_iFramePerSecond;

public:
	void finlatick();
	void Create(const std::wstring& name, const tMTAnimClip _AnimClips, int _StartIdx, int _EndIdx, bool _bLoop);
	void Create(const std::wstring& name, const vector<tMTAnimClip>* _AnimClips, bool _bLoop);

	const tMTAnimClip GetCurClip() { return m_Clip; }

	const wstring& GetAnimName() { return m_Clip.strAnimName; }
	int GetClipIdx() { return m_iCurIdx; }
	int GetClipNextIdx() { return m_iNextIdx; }

	int GetClipLength() { return m_Clip.iFrameLength; }
	int GetStartFrame() { return m_Clip.iStartFrame; }
	int GetEndFrame() { return m_Clip.iEndFrame; }

	void SetManualIdx(int idx);

	double GetAnimTimeLength() { return m_Clip.dTimeLength; }

	float GetClipTime() { return m_fTime; }

	bool IsFinish() { return m_bFinish; }
	bool IsLoop() { return m_bLoop; }

	void SetFPS(int _Count) { m_iFramePerSecond = _Count; }
	int GetFPS() { return m_iFramePerSecond; }

	float GetRatio() { return m_fRatio; }
	void SetRatio(float _Ratio) { m_fRatio = _Ratio; }

	void Reset()
	{
		m_iCurIdx = m_Clip.iStartFrame;
		m_iNextIdx = m_iCurIdx;
		m_fTime = 0.0f;
		m_fRatio = 0.0f;
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

