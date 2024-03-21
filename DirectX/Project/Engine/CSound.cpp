#include "pch.h"
#include "CSound.h"

#include "CGameObject.h"
#include "CResMgr.h"
#include "CTransform.h"
#include "SoundMgr.h"

FMOD_RESULT CHANNEL_CALLBACK(FMOD_CHANNELCONTROL* channelcontrol, FMOD_CHANNELCONTROL_TYPE controltype
                             , FMOD_CHANNELCONTROL_CALLBACK_TYPE callbacktype
                             , void* commanddata1, void* commanddata2);
extern FMOD::System* g_pFMOD;

CSound::CSound()
	: CRes(RES_TYPE::SOUND)
	, m_pSound(nullptr)
{
}

CSound::~CSound()
{
	if (nullptr != m_pSound)
	{
		m_pSound->release();
		m_pSound = nullptr;
	}
}

int CSound::Play(int _iRoopCount, float _fVolume, bool _bOverlap)
{
	if (!_bOverlap && IsPlaying())
	{
		return -1;
	}

	g_pFMOD->playSound(m_pSound, nullptr, false, &m_pChannel);
	m_pChannel->setVolume(_fVolume);

	m_pChannel->setCallback(CHANNEL_CALLBACK);
	m_pChannel->setUserData(this);

	m_pChannel->setMode(FMOD_3D | FMOD_LOOP_NORMAL);
	m_pChannel->setLoopCount(_iRoopCount);

	int iIdx = -1;
	m_pChannel->getIndex(&iIdx);

	return iIdx;
}

void CSound::Stop()
{
	if (m_pChannel && IsPlaying())
		m_pChannel->stop();
}

void CSound::SetChannelPos(Vec3 _vPos, Vec3 vVel)
{
	if (nullptr == m_pChannel)
		return;

	FMOD_VECTOR fvPos = { _vPos.x, _vPos.y, _vPos.z };
	FMOD_VECTOR fvVel = { vVel.x, vVel.y, vVel.z };

	m_pChannel->set3DAttributes(&fvPos, &fvVel);
}

void CSound::SetVolume(float _f)
{
	m_pChannel->setVolume(_f);
}

void CSound::SetDistance(float _fMaxDist, float fMinDist)
{
	if(m_pChannel)
	{
		m_pChannel->set3DMinMaxDistance(fMinDist, _fMaxDist);
	}
}

bool CSound::IsPlaying()
{
	if (nullptr == m_pSound) return false;
	if (nullptr == m_pChannel) return false;

	bool isPlaying = false;
	m_pChannel->isPlaying(&isPlaying);

	return isPlaying;
}

bool CSound::IsPlaying(const wstring& _wsPath)
{
	wstring strFilePath = CPathMgr::GetInst()->GetContentPath();
	strFilePath += _wsPath;
	if(strFilePath == m_wsSoundPath)
	{
		if (IsPlaying())
			return true;
	}
	return false;
}

void CSound::tick()
{
	if (m_pTransform)
	{
		SetChannelPos(m_pTransform->GetRelativePos(), m_pTransform->GetRelativeDir(DIR_TYPE::FRONT));
	}
}

int CSound::Load(const wstring& _strFilePath)
{
	string path(_strFilePath.begin(), _strFilePath.end());
	
	m_wsSoundPath = _strFilePath;
	if (FMOD_OK != g_pFMOD->createSound(path.c_str(), FMOD_DEFAULT, nullptr, &m_pSound))
	{
		assert(nullptr);
	}

	return S_OK;
}

int CSound::LoadUnity(const wstring& _strFilePath)
{
	wstring strFilePath = CPathMgr::GetInst()->GetContentPath();
	strFilePath += _strFilePath;

	m_wsSoundPath = strFilePath;
	string path(strFilePath.begin(), strFilePath.end());

	if (FMOD_OK != g_pFMOD->createSound(path.c_str(), FMOD_DEFAULT, nullptr, &m_pSound))
	{
		assert(nullptr);
	}

	return S_OK;
}


// =========
// Call Back
// =========
FMOD_RESULT CHANNEL_CALLBACK(FMOD_CHANNELCONTROL* channelcontrol, FMOD_CHANNELCONTROL_TYPE controltype
	, FMOD_CHANNELCONTROL_CALLBACK_TYPE callbacktype
	, void* commanddata1, void* commanddata2)
{
	FMOD::Channel* cppchannel = (FMOD::Channel*)channelcontrol;
	CSound* pSound = nullptr;

	switch (controltype)
	{
	case FMOD_CHANNELCONTROL_CALLBACK_END:
	{
		cppchannel->getUserData((void**)&pSound);
	}
	break;
	}

	return FMOD_OK;
}