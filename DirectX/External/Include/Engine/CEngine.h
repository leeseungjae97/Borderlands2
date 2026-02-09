#pragma once

class CEngine
	: public CSingleton<CEngine>
{
private:
	HWND	m_hWnd;
	HCURSOR m_DefaultCursor;
	// 윈도우 해상도
	Vec2	m_vResolution;
	float fixedCycleTime = 60;
	double beforeFixedUpdateTime = 0;

public:
	Vec2 GetWindowResolution() { return m_vResolution; }
	HWND GetMainWnd() { return m_hWnd ; }
	HCURSOR GetDefaultCursor() { return m_DefaultCursor; }
public:
	int init(HWND _hWnd, UINT _iWidth, UINT _iHeight);
	void progress();
	void begin();

private:
	void firsttick();
	void tick();
	void finaltick();
	void fixedtick();
	void render();


public:
	CEngine();
	~CEngine();
};
