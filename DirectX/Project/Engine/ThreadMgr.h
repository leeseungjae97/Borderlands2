#pragma once
#include "CSingleton.h"
#include "atomic"
class ThreadMgr
	: public CSingleton<ThreadMgr>
{
private:
	vector<std::thread> m_vecThreads;
	std::atomic<int> m_iFinishedCount{ 0 };
	
public:
	void tick();
	void init();

public:
	void AddThread(std::function<void()> func);
	void AddThreadEnd();
	void RunThread();

	bool IsAnyThreadRun();

	void test() { ; }
public:
	ThreadMgr();
	~ThreadMgr();
};