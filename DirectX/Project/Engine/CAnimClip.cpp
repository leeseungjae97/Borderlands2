#include "pch.h"
#include "CAnimClip.h"

#include "CAnimation3D_CShader.h"
#include "CAnimator3D.h"
#include "CMeshRender.h"
#include "CResMgr.h"
#include "CTimeMgr.h"

CAnimClip::CAnimClip()
	: m_pOwner(nullptr)
	, m_Clip{}
	, m_iCurIdx(0)
	, m_iNextIdx(0)
	, m_fTime(0.f)
	, m_bFinish(false)
	, m_bLoop(false)
	, m_fRatio(0.f)
	, m_iFramePerSecond(30)
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

	if (m_Clip.dTimeLength < m_fTime)
	{
		m_fTime = 0.f;
	}

	if (m_Clip.iEndFrame <= m_iCurIdx)
	{
		if (m_bLoop)
		{
			m_iCurIdx = m_Clip.iStartFrame;
			m_iNextIdx = m_iCurIdx;
			m_fTime = 0.f;
		}
		else
		{
			m_iCurIdx = m_Clip.iEndFrame;
			m_iNextIdx = m_iCurIdx;
			m_fTime = 0.f;
			m_bFinish = true;
		}
	}
	else
		m_iNextIdx = m_iCurIdx + 1;

	double dCurtime = m_Clip.dStartTime + m_fTime;
	double dFrameIdx = dCurtime * (double)m_iFramePerSecond;
	m_iCurIdx = (int)dFrameIdx;
	m_fRatio = (float)(dFrameIdx - (double)m_iCurIdx);
}

void CAnimClip::Create(const std::wstring& name, const tMTAnimClip _AnimClips, int _StartFrame, int _EndFrame, bool _bLoop)
{
	SetName(name);

	m_Clip = _AnimClips;

	m_bLoop = _bLoop;

	m_Clip.iStartFrame = _StartFrame;
	m_Clip.iEndFrame = _EndFrame;
}

void CAnimClip::Create(const std::wstring& name, const vector<tMTAnimClip>* _AnimClips, bool _bLoop)
{
	SetName(name);

	m_Clip = _AnimClips->at(0);

	m_bLoop = _bLoop;
}
void CAnimClip::SaveToLevelFile(FILE* _File)
{
	fwrite(&m_pOwner, sizeof(CAnimator3D), 1, _File);
	fwrite(&m_Clip, sizeof(tMTAnimClip), 1, _File);
	fwrite(&m_iCurIdx, sizeof(int), 1, _File);
	fwrite(&m_iNextIdx, sizeof(int), 1, _File);
	fwrite(&m_fTime, sizeof(float), 1, _File);
	fwrite(&m_bFinish, sizeof(bool), 1, _File);
	fwrite(&m_bLoop, sizeof(bool), 1, _File);
	fwrite(&m_fRatio, sizeof(float), 1, _File);
	fwrite(&m_iFramePerSecond, sizeof(int), 1, _File);
}

void CAnimClip::LoadFromLevelFile(FILE* _File)
{
	fread(&m_pOwner, sizeof(CAnimator3D), 1, _File);
	fread(&m_Clip, sizeof(tMTAnimClip), 1, _File);
	fread(&m_iCurIdx, sizeof(int), 1, _File);
	fread(&m_iNextIdx, sizeof(int), 1, _File);
	fread(&m_fTime, sizeof(float), 1, _File);
	fread(&m_bFinish, sizeof(bool), 1, _File);
	fread(&m_bLoop, sizeof(bool), 1, _File);
	fread(&m_fRatio, sizeof(float), 1, _File);
	fread(&m_iFramePerSecond, sizeof(int), 1, _File);
}