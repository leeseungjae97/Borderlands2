#include "pch.h"
#include "CAnimClip.h"

#include "CAnimation3D_CShader.h"
#include "CAnimator3D.h"
#include "CMeshRender.h"
#include "CResMgr.h"
#include "CTimeMgr.h"

CAnimClip::CAnimClip()
	: m_pOwner(nullptr)
	, m_iCurIdx(0)
	, m_iNextIdx(0)
	, m_fTime(0.f)
	, m_dStartTime(0.0)
	, m_dEndTime(0.0)
	, m_iStartIdx(0)
	, m_iEndIdx(0)
	, m_bFinish(false)
	, m_bLoop(false)
	, m_fRatio(0.f)
	, m_iFramePerSecond(0)
{
}

CAnimClip::~CAnimClip()
{
}


void CAnimClip::finlatick()
{
	if (m_bFinish)
		return;

	m_fTime += DT;

	if (m_vecClip[m_iCurIdx].fTimeLength < m_fTime)
	{
		//TODO 
		m_fTime = 0.f;

		if (m_iEndIdx <= m_iCurIdx)
		{
			if(m_bLoop)
			{
				m_iCurIdx = m_iStartIdx;
			}
			else
			{
				m_iCurIdx = m_iEndIdx;
				m_bFinish = true;
			}
		}
	}
	double dCurtime = m_dStartTime + m_fTime;
	double dFrameIdx = dCurtime * (double)m_iFramePerSecond;
	m_fRatio = (float)(dFrameIdx - (double)m_iCurIdx);
}

void CAnimClip::Create(const std::wstring& name, vector<tMTAnimClip>& _AnimClips, int _StartIdx, int _EndIdx, bool _bRepeat)
{
	SetName(name);

	m_vecClip.resize(_AnimClips.size());

	for(int i = 0 ; i < _AnimClips.size(); ++i)
	{
		m_vecClip[i].fCurClipUpdateTime = _AnimClips[i].fUpdateTime;
	}
	
	m_iStartIdx = _StartIdx;
	m_iEndIdx = _EndIdx;
}

void CAnimClip::SaveToLevelFile(FILE* _File)
{
}

void CAnimClip::LoadFromLevelFile(FILE* _File)
{
}