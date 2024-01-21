#include "pch.h"
#include "CLevelMgr.h"

#include "CEventMgr.h"
#include "CLevel.h"
#include "CLayer.h"

CLevelMgr::CLevelMgr()
	: m_pCurLevel(nullptr)
{
}

CLevelMgr::~CLevelMgr()
{
	//if (nullptr != m_pCurLevel)
	//	delete m_pCurLevel;

	Safe_Del_Map(m_mapLevels);
}

void CLevelMgr::init()
{
	//m_pCurLevel = new CLevel;
	//m_pCurLevel->ChangeState(LEVEL_STATE::STOP);
}

void CLevelMgr::tick()
{
	static bool initMainLevel = true;
	if(initMainLevel)
	{
		ChangeCurLevel(CLevelMgr::GetInst()->GetLevel(L"main level"));
		//LevelRecognize();
		initMainLevel = false;
	}
	if (nullptr == m_pCurLevel)
		return;

	m_pCurLevel->clear();

	if (LEVEL_STATE::PLAY == m_pCurLevel->GetState())
	{
		m_pCurLevel->tick();		
	}

	m_pCurLevel->finaltick();
}


CGameObject* CLevelMgr::FindObjectByName(const wstring& _strName)
{
	return m_pCurLevel->FindObjectByName(_strName);	
}

void CLevelMgr::FindObjectByName(const wstring& _strName, vector<CGameObject*>& _vec)
{
	if (nullptr == m_pCurLevel)
		return;

	m_pCurLevel->FindObjectByName(_strName, _vec);
}

CLevel* CLevelMgr::CreateLevel(const wstring& name)
{
	CLevel* pLevel = nullptr;
	pLevel = GetLevel(name);
	if (pLevel)
		return pLevel;

	pLevel = new CLevel();
	pLevel->SetName(name);
	m_mapLevels.insert(make_pair(name, pLevel));

	return pLevel;
}

CLevel* CLevelMgr::GetLevel(const wstring& name)
{
	if (m_mapLevels.empty())
		return nullptr;

	for(const auto& iter : m_mapLevels)
	{
		if(iter.second)
		{
			if (iter.first == name)
				return iter.second;
		}
	}

	return nullptr;
}

void CLevelMgr::ChangeLevel(CLevel* _NextLevel)
{
	LEVEL_STATE prevState = LEVEL_STATE::STOP;
	if (nullptr != m_pCurLevel)
	{
		prevState = m_pCurLevel->GetState();
		//delete m_pCurLevel;
		//m_pCurLevel = nullptr;
		m_pCurLevel->ChangeState(LEVEL_STATE::NO_UPDATE_RENDER);
	}
	
	m_pCurLevel = _NextLevel;
	m_pCurLevel->ChangeState(prevState);
}

void CLevelMgr::DeleteLevel(CLevel* _DeleteLevel)
{
	map<wstring, CLevel*>::iterator iter= m_mapLevels.begin();
	while(iter != m_mapLevels.end())
	{
		if (iter->second != _DeleteLevel)
			++iter;
		else
		{
			delete iter->second;
			iter->second = nullptr;
			iter = m_mapLevels.erase(iter);
			return;
		}
	}
}

void CLevelMgr::ResetLevel(CLevel* _ResetData)
{
	wstring levelName= m_pCurLevel->GetName();

	DeleteLevel(m_pCurLevel);
	InsertLevel(levelName, _ResetData);
	ChangeLevel(_ResetData);
}

void CLevelMgr::InsertLevel(const wstring& _strName, CLevel* _Level)
{
	m_mapLevels.insert(make_pair(_strName, _Level));
}
