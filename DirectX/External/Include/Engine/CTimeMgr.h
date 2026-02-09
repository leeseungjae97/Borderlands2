#pragma once


class CTimeMgr
	: public CSingleton<CTimeMgr>
{	
	SINGLE(CTimeMgr);
private:
	LARGE_INTEGER	m_llPrevCount;
	LARGE_INTEGER	m_llCurCount;
	LARGE_INTEGER	m_llFrequency;

	UINT			m_iCallCount;
	float			m_fDeltaTime;
	float			m_fTime;
	double			m_dTotalAccTime;
public:
	void init();
	void tick();
	void render();

	UINT CallCount() { return m_iCallCount; }

public:
	float GetDeltaTime() { return m_fDeltaTime; }
	double GetTime() { return m_dTotalAccTime; }
};

