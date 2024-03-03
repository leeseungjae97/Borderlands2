#pragma once
#include "CSingleton.h"

class MouseMgr
	: public CSingleton<MouseMgr>
{
private:
	bool mMouseOnUI;
	bool bFreeMouse;

public:
	void SetMouseOnUI(bool onui) { mMouseOnUI = onui; }
	bool GetMouseOnUI() { return mMouseOnUI; }

	void MouseOut();
	Vec2 UnprojectionMouse2DPos(float z);
	Vec3 MousePosRay();

	void init();
	void tick();
	void finaltick();

	void UpdateMouseHoverUI();

public:
	MouseMgr();
	virtual ~MouseMgr();
};

