#pragma once

#include "CSingleton.h"
#include "ptr.h"

class CSound;

class SoundMgr
	: public CSingleton<SoundMgr>
{
public:
	enum class SOUND_TYPE
	{
		BGM,
		SFX,
		END
	};
public:
	
	Ptr<CSound> m_pBGM;
	vector<Ptr<CSound>> m_vecSFX;
	vector<Ptr<CSound>> m_vecSounds;

	CGameObject* m_pAudioListener;

public:
	void init();
	void tick();
	void begin();

	void clear(SOUND_TYPE _tSoundType);
	void allClear();

	bool IsPlaying(const wstring& path, SOUND_TYPE _tSoundType);

	void SetSoundPos(const wstring& path, SOUND_TYPE _tSoundType, Vec3 _vPos);
	Ptr<CSound> FindSound(const wstring& path, SOUND_TYPE _tSoundType);

	void Play(const wstring& path, class CTransform* _pTrans, int _iRoopCount, float distance, SOUND_TYPE _tSoundType = SOUND_TYPE::END
		, float _fVolume = 1.f, bool _bOverlap = false);
	void Play(const wstring& path, Vec3 _vPos, int _iRoopCount, float distance = 100.f, SOUND_TYPE _tSoundType = SOUND_TYPE::END
		, float _fVolume = 1.f, bool _bOverlap = false);

	void Stop(const wstring& path, SOUND_TYPE _tSoundType);
public:
	SoundMgr();
	~SoundMgr();
};

