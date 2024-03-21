#pragma once
#include "CRes.h"

#include "SoundMgr.h"

class CTransform;

class CSound :
	public CRes
{
public:
	Vec3 m_vPos;

	CTransform* m_pTransform;

private:
	FMOD::Sound* m_pSound;
	FMOD::Channel* m_pChannel;

	wstring m_wsSoundPath;

public:
	int Play(int _iRoopCount, float _fVolume = 1.f, bool _bOverlap = false);
	void Stop();

	void SetChannelPos(Vec3 _vPos, Vec3 vVel = Vec3::One);
	void SetTransform(CTransform* trans) { m_pTransform = trans; }
	void SetVolume(float _f);
	void SetDistance(float _fMaxDist, float fMinDist = 0.f);
	bool IsPlaying();
	bool IsPlaying(const wstring& _wsPath);

	void tick();

private:
	friend FMOD_RESULT CHANNEL_CALLBACK(FMOD_CHANNELCONTROL* channelcontrol, FMOD_CHANNELCONTROL_TYPE controltype
		, FMOD_CHANNELCONTROL_CALLBACK_TYPE callbacktype
		, void* commanddata1, void* commanddata2);

public:
	virtual void UpdateData() {}

	virtual int Save(const wstring&) override { return S_OK; }
	virtual int Load(const wstring& _strFilePath) override;
	int LoadUnity(const wstring& _strFilePath);
		
public:
	CSound();
	virtual ~CSound();
};

