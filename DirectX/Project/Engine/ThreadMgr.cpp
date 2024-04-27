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
	//if (!m_vecThreads.empty())
	//{
	//	auto iter = m_vecThreads.begin();
	//	for (iter; iter != m_vecThreads.end();)
	//	{
	//		if (!(*iter).joinable())
	//		{
	//			iter = m_vecThreads.erase(iter);
	//		}

	//		if (!m_vecThreads.empty())
	//			++iter;
	//	}
	//}
}

void ThreadMgr::init()
{
}

void ThreadMgr::AddThread(std::function<void()> func)
{
	m_vecThreads.emplace_back(std::thread(func));
	m_vecThreads[m_vecThreads.size() - 1].detach();
}

void ThreadMgr::AddThreadEnd()
{
	vec_ends.push_back(1);
}

void ThreadMgr::RunThread()
{

}

bool ThreadMgr::IsThreadRun()
{
	if (vec_ends.size() == m_vecThreads.size())
		return false;

	return true;
}
