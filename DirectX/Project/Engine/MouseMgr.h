#pragma once
class MouseMgr
	: public CSingleton<MouseMgr>
{
	SINGLE(MouseMgr);
public:
	void init();
	void tick();

private:
};

