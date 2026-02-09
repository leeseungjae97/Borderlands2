#include "pch.h"
#include "ThreadMgr.h"

ThreadMgr::ThreadMgr()
{
}

ThreadMgr::~ThreadMgr()
{
}

void ThreadMgr::tick()
{
	if (!m_vecThreads.empty() && !IsAnyThreadRun()) 
	{
		for (auto& t : m_vecThreads)
		{
			if (t.joinable()) 
				t.join();
		}

		m_vecThreads.clear();
		m_iFinishedCount = 0;
	}
}

void ThreadMgr::init()
{
}

void ThreadMgr::AddThread(std::function<void()> func)
{
	m_vecThreads.emplace_back(func);
}

void ThreadMgr::AddThreadEnd()
{
	m_iFinishedCount++;
}

void ThreadMgr::RunThread()
{

}

bool ThreadMgr::IsAnyThreadRun()
{
	return m_iFinishedCount < (int)m_vecThreads.size();
}

