#include "pch.h"
#include "SoundMgr.h"

#include "CEventMgr.h"
#include "CLayer.h"
#include "CLevel.h"
#include "CLevelMgr.h"
#include "CResMgr.h"
#include "CSound.h""
#include "CTransform.h"
#include "PlayerMgr.h"

extern FMOD::System* g_pFMOD;

SoundMgr::SoundMgr()
{
}

SoundMgr::~SoundMgr()
{
}


void SoundMgr::init()
{
}

void SoundMgr::begin()
{
	CLevel* curLevel = CLevelMgr::GetInst()->GetCurLevel();
	if (curLevel->GetName() == L"main menu level")
	{
		m_pAudioListener = curLevel->GetLayer((UINT)LAYER_TYPE::Camera)->GetParentObject()[0];
	}

	if (curLevel->GetName() == L"main level 2")
	{
		m_pAudioListener = PlayerMgr::GetInst()->GetPlayer();

		Play(L"sound\\sfx\\lava.ogg", Vec3(0.f, 0.f, 0.f), -1, 100.f, SoundMgr::SOUND_TYPE::SFX, 0.5f);
	}

}

void SoundMgr::clear(SOUND_TYPE _tSoundType)
{
	switch (_tSoundType)
	{
	case SOUND_TYPE::BGM:
	{
		if(nullptr != m_pBGM)
			m_pBGM->Stop();
	}
	break;
	case SOUND_TYPE::SFX:
	{
		for (int i = 0; i < m_vecSFX.size(); ++i)
		{
			m_vecSFX[i]->Stop();
		}
		m_vecSFX.clear();
	}
	break;
	case SOUND_TYPE::END:
	{
		for (int i = 0; i < m_vecSounds.size(); ++i)
		{
			m_vecSounds[i]->Stop();
		}
		m_vecSounds.clear();
	}
	break;
	}
}

void SoundMgr::allClear()
{
	clear(SOUND_TYPE::BGM);
	clear(SOUND_TYPE::SFX);
	clear(SOUND_TYPE::END);
}

bool SoundMgr::IsPlaying(const wstring& path, SOUND_TYPE _tSoundType)
{
	bool bFind = false;
	if(SOUND_TYPE::SFX == _tSoundType)
	{
		for (int i = 0; i < m_vecSFX.size(); ++i)
		{
			if (m_vecSFX[i]->IsPlaying(path))
				bFind = true;
		}
	}else if (SOUND_TYPE::END == _tSoundType)
	{
		for (int i = 0; i < m_vecSounds.size(); ++i)
		{
			if (m_vecSounds[i]->IsPlaying(path))
				bFind = true;
		}
	}else
	{
		if (nullptr != m_pBGM && m_pBGM->IsPlaying(path))
			bFind = true;
	}

	return bFind;
}

void SoundMgr::SetSoundPos(const wstring& path, SOUND_TYPE _tSoundType, Vec3 _vPos)
{
	Ptr<CSound> sound = FindSound(path, _tSoundType);
	sound->SetChannelPos(_vPos);
}

Ptr<CSound> SoundMgr::FindSound(const wstring& path, SOUND_TYPE _tSoundType)
{
	if (SOUND_TYPE::SFX == _tSoundType)
	{
		for (int i = 0; i < m_vecSFX.size(); ++i)
		{
			if (m_vecSFX[i]->IsPlaying(path))
				return m_vecSFX[i];
		}
	}
	else if (SOUND_TYPE::END == _tSoundType)
	{
		for (int i = 0; i < m_vecSounds.size(); ++i)
		{
			if (m_vecSounds[i]->IsPlaying(path))
				return m_vecSounds[i];
		}
	}
	else
	{
		if (nullptr != m_pBGM && m_pBGM->IsPlaying(path))
			return m_pBGM;
	}
}

void SoundMgr::Play(const wstring& path, CTransform* _pTrans, int _iRoopCount, float distance, SOUND_TYPE _tSoundType, float _fVolume, bool _bOverlap)
{
	switch (_tSoundType)
	{
	case SOUND_TYPE::BGM:
	{
		m_pBGM = CResMgr::GetInst()->Load<CSound>(path, path);
		m_pBGM->Play(_iRoopCount, _fVolume, _bOverlap);
		m_pBGM->SetTransform(_pTrans);
		m_pBGM->SetDistance(distance);
	}
	break;
	case SOUND_TYPE::SFX:
	{
		Ptr<CSound> sound = CResMgr::GetInst()->Load<CSound>(path, path);
		sound->SetTransform(_pTrans);
		sound->Play(_iRoopCount, _fVolume, _bOverlap);
		sound->SetDistance(distance);
		m_vecSFX.push_back(sound);
	}
	break;
	case SOUND_TYPE::END:
	{
		Ptr<CSound> sound = CResMgr::GetInst()->Load<CSound>(path, path);
		sound->SetTransform(_pTrans);
		sound->Play(_iRoopCount, _fVolume, _bOverlap);
		sound->SetDistance(distance);
		m_vecSounds.push_back(sound);
	}
	break;
	}
}

void SoundMgr::Play(const wstring& path, Vec3 _vPos, int _iRoopCount, float distance, SOUND_TYPE _tSoundType, float _fVolume, bool _bOverlap)
{
	switch (_tSoundType)
	{
	case SOUND_TYPE::BGM:
	{
		m_pBGM = CResMgr::GetInst()->Load<CSound>(path, path);
		m_pBGM->Play(_iRoopCount, _fVolume, _bOverlap);
		m_pBGM->SetChannelPos(_vPos);
		m_pBGM->SetDistance(distance, distance);
	}
	break;
	case SOUND_TYPE::SFX:
	{
		Ptr<CSound> sound;
		if(_bOverlap)
		{
			sound = new CSound;
			sound->LoadUnity(path);
		}else{
			sound = CResMgr::GetInst()->Load<CSound>(path, path);
		}
		sound->Play(_iRoopCount, _fVolume, _bOverlap);
		sound->SetChannelPos(_vPos);
		sound->SetDistance(distance, distance);
		m_vecSFX.push_back(sound);
	}
	break;
	case SOUND_TYPE::END:
	{
		Ptr<CSound> sound;
		if (_bOverlap)
		{
			sound = new CSound;
			sound->LoadUnity(path);
		}
		else
		{
			sound = CResMgr::GetInst()->Load<CSound>(path, path);
		}
		sound->Play(_iRoopCount, _fVolume, _bOverlap);
		sound->SetChannelPos(_vPos);
		sound->SetDistance(distance, distance);
		m_vecSounds.push_back(sound);
	}
	break;
	}
}

void SoundMgr::Stop(const wstring& path, SOUND_TYPE _tSoundType)
{
	switch (_tSoundType)
	{
	case SOUND_TYPE::BGM:
	{
		if(nullptr != m_pBGM)
			m_pBGM->Stop();
	}
	break;
	case SOUND_TYPE::SFX:
	{
		for(int i = 0 ; i < m_vecSFX.size(); ++i)
		{
			if(m_vecSFX[i]->IsPlaying(path))
			{
				m_vecSFX[i]->Stop();
			}
		}
	}
	break;
	case SOUND_TYPE::END:
	{
		for (int i = 0; i < m_vecSounds.size(); ++i)
		{
			if (m_vecSounds[i]->IsPlaying(path))
			{
				m_vecSounds[i]->Stop();
			}
		}
	}
	break;
	}
}

void SoundMgr::tick()
{
	g_pFMOD->update();

	if (CEventMgr::GetInst()->IsLevelLoad())
	{
		allClear();

		CLevel* curLevel = CLevelMgr::GetInst()->GetCurLevel();
		if (curLevel->GetName() == L"main menu level")
		{
			m_pAudioListener = curLevel->GetLayer((UINT)LAYER_TYPE::Camera)->GetParentObject()[0];

			Play(L"sound\\bgm\\main_menu.ogg", Vec3(0.f, 0.f, 0.f), -1, 100.f, SoundMgr::SOUND_TYPE::BGM, 0.5f);
			Play(L"sound\\sfx\\wind.ogg", Vec3(0.f, 0.f, 0.f), -1, 100.f, SoundMgr::SOUND_TYPE::SFX, 0.1f);
			Play(L"sound\\sfx\\wind_1.ogg", Vec3(0.f, 0.f, 0.f), -1, 100.f, SoundMgr::SOUND_TYPE::SFX, 0.1f);
			Play(L"sound\\effect\\fire.ogg", Vec3(0.f, 0.f, 0.f), -1, 100.f, SoundMgr::SOUND_TYPE::SFX, 0.3f);
		}
		if (curLevel->GetName() == L"main level 1")
		{
			m_pAudioListener = PlayerMgr::GetInst()->GetPlayer();

			//Play(L"sound\\sfx\\lava.ogg", Vec3(0.f, 0.f, 0.f), -1, 100.f, SoundMgr::SOUND_TYPE::SFX, 0.1f);
			Play(L"sound\\sfx\\level_1_sfx_1.ogg", Vec3(0.f, 0.f, 0.f), -1, 100.f, SoundMgr::SOUND_TYPE::SFX, 0.1f);
			Play(L"sound\\sfx\\level_1_sfx_2.ogg", Vec3(0.f, 0.f, 0.f), -1, 100.f, SoundMgr::SOUND_TYPE::SFX, 0.1f);
			Play(L"sound\\sfx\\wind_2.ogg", Vec3(0.f, 0.f, 0.f), -1, 100.f, SoundMgr::SOUND_TYPE::SFX, 0.1f);
			Play(L"sound\\bgm\\main_level_1.ogg", Vec3(0.f, 0.f, 0.f), -1, 100.f, SoundMgr::SOUND_TYPE::BGM, 0.5f);
		}

		if (curLevel->GetName() == L"main level 2")
		{
			m_pAudioListener = PlayerMgr::GetInst()->GetPlayer();

			Play(L"sound\\sfx\\lava.ogg", Vec3(0.f, 0.f, 0.f), -1, 100.f, SoundMgr::SOUND_TYPE::SFX, 0.1f);
			Play(L"sound\\sfx\\main_level_2_sfx.ogg", Vec3(0.f, 0.f, 0.f), -1, 100.f, SoundMgr::SOUND_TYPE::SFX, 0.1f);

			Play(L"sound\\effect\\fire.ogg", Vec3(-2982.f, 1703.f, 17946.f), -1, 0.8f, SoundMgr::SOUND_TYPE::SFX, 0.1f);
			Play(L"sound\\effect\\fire.ogg", Vec3(-2982.f, 1703.f, 16917.000), -1, 0.8f, SoundMgr::SOUND_TYPE::SFX, 0.1f);
			Play(L"sound\\effect\\fire.ogg", Vec3(-3346.000, 1981.000, 14060.000), -1, 1.f, SoundMgr::SOUND_TYPE::SFX, 0.1f);
		}
	}

	CLevel* curLevel = CLevelMgr::GetInst()->GetCurLevel();
	if (curLevel->GetName() == L"main menu level")
	{
		m_pAudioListener = curLevel->GetLayer((UINT)LAYER_TYPE::Camera)->GetParentObject()[0];

		/*if(IsPlaying(L"sound\\bgm\\wind.ogg", SoundMgr::SOUND_TYPE::SFX))
			Play(L"sound\\bgm\\wind.ogg", Vec3(0.f, 0.f, 0.f), -1, 100.f, SoundMgr::SOUND_TYPE::SFX, 0.2f);
		if (IsPlaying(L"sound\\bgm\\wind_1.ogg", SoundMgr::SOUND_TYPE::SFX))
			Play(L"sound\\bgm\\wind_1.ogg", Vec3(0.f, 0.f, 0.f), -1, 100.f, SoundMgr::SOUND_TYPE::SFX, 0.2f);*/
	}


	if (nullptr != m_pBGM && !m_pBGM->IsPlaying())
	{
		m_pBGM->tick();
	}

	auto iter = m_vecSounds.begin();
	while (iter != m_vecSounds.end())
	{
		if ((*iter)->IsPlaying())
		{
			(*iter)->tick();
			++iter;
		}
		else
		{
			iter = m_vecSounds.erase(iter);
		}
	}

	auto iter2 = m_vecSFX.begin();
	while (iter2 != m_vecSFX.end())
	{
		if ((*iter2)->IsPlaying())
		{
			(*iter2)->tick();
			++iter2;
		}
		else
		{
			iter2 = m_vecSFX.erase(iter2);
		}
	}

	if (m_pAudioListener)
	{
		Vec3 vPos = m_pAudioListener->Transform()->GetRelativePos();
		Vec3 vFront = m_pAudioListener->Transform()->GetRelativeDir(DIR_TYPE::FRONT);
		Vec3 vUp = m_pAudioListener->Transform()->GetRelativeDir(DIR_TYPE::UP);

		const FMOD_VECTOR fvsPos = { vPos.x, vPos.y, vPos.z };
		const FMOD_VECTOR fvVel = { 1.f, 1.f, 1.f };
		const FMOD_VECTOR fvFront = { vFront.x,vFront.y, vFront.z };
		const FMOD_VECTOR fvUp = { vUp.x, vUp.y, vUp.z };

		g_pFMOD->set3DListenerAttributes(0, &fvsPos, &fvVel, &fvFront, &fvUp);
	}
}
