#pragma once
#include "CSingleton.h"
class ThreadMgr
	: public CSingleton<ThreadMgr>
{
private:
	vector<std::thread> m_vecThreads;
	vector<int>	vec_ends;
public:
	void tick();
	void init();

public:
	void AddThread(std::function<void()> func);
	void AddThreadEnd();
	void RunThread();

	bool IsThreadRun();

	void test() { ; }
public:
	ThreadMgr();
	~ThreadMgr();
};