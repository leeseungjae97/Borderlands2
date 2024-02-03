#include "pch.h"
#include "CameraMgr.h"

CameraMgr::CameraMgr()
{
}

CameraMgr::~CameraMgr()
{
}

void CameraMgr::AddCamObj(wstring _Name, CGameObject* _Cam)
{
	auto iter = m_mapCams.find(_Name);
	if (iter != m_mapCams.end())
		m_mapCams.erase(iter);

	m_mapCams.insert(make_pair(_Name, _Cam));
}

CGameObject* CameraMgr::GetCamObj(wstring _Name)
{
	return m_mapCams.find(_Name)->second;
}

