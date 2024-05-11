#include "pch.h"
#include "CLevelMgr.h"

#include "CCamera.h"
#include "CEngine.h"
#include "CEventMgr.h"
#include "CLevel.h"
#include "CLayer.h"
#include "CRenderMgr.h"
#include "CTimeMgr.h"
#include "CTransform.h"
#include "CUI.h"
#include "LoadingMgr.h"
#include "WeaponMgr.h"
#include "ThreadMgr.h"

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
	if (initMainLevel)
	{
		//if(m_mapLevels.size() < 2)
		//	LevelRecognize();
		//{
		//	
		//}

		ChangeCurLevel(GetLevel(L"main menu level"));
		//GetLevel(L"main menu level")->ChangeState(LEVEL_STATE::PLAY);

		initMainLevel = false;
	}

	if (nullptr == m_pCurLevel)
		return;

	if (m_pCurLevel->GetState() == LEVEL_STATE::STOP)
		m_pCurLevel->ChangeState(LEVEL_STATE::PLAY);

	m_pCurLevel->clear();

	if (LEVEL_STATE::PLAY == m_pCurLevel->GetState())
	{
		CheckLevelClear();
		m_pCurLevel->tick();

		if (ThreadMgr::GetInst()->IsThreadRun())
		{
			if (m_pCurLevel->GetName() != L"loading")
			{
				LoadingLevel(L"loading");
				//ChangeCurLevel(GetLevel(L"loading"));
				//GetLevel(L"loading")->ChangeState(LEVEL_STATE::PLAY);
			}
			else
			{
				if(m_pCurLevel->GetState() == LEVEL_STATE::STOP)
					m_pCurLevel->ChangeState(LEVEL_STATE::PLAY);

				if (m_pCurLevel->GetState() == LEVEL_STATE::PLAY
					&& m_pCurLevel->GetTickCnt() > 0 && m_pCurLevel->GetTickCnt() < 10)
				{
					if (CRenderMgr::GetInst()->GetMainCam())
						CRenderMgr::GetInst()->GetMainCam()->SetFadeTime(2.f, true);
				}
			}
		}
	}

	m_pCurLevel->finaltick();
}

void CLevelMgr::begin()
{
	for (auto pair : m_mapLevels)
	{
		m_pCurLevel = pair.second;
		pair.second->begin();
	}
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

	for (const auto& iter : m_mapLevels)
	{
		if (iter.second)
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
		//m_pCurLevel->ChangeState(LEVEL_STATE::NO_UPDATE_RENDER);
	}

	m_pCurLevel = _NextLevel;
	m_pCurLevel->ChangeState(prevState);
}
void CLevelMgr::DeleteLevel(const wstring& name)
{
	if (m_pCurLevel->GetName() == name)
		m_pCurLevel = nullptr;

	map<wstring, CLevel*>::iterator iter = m_mapLevels.begin();
	while (iter != m_mapLevels.end())
	{
		if (iter->first != name)
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

void CLevelMgr::DeleteLevel(CLevel* _DeleteLevel)
{
	if (m_pCurLevel == _DeleteLevel)
		m_pCurLevel = nullptr;

	map<wstring, CLevel*>::iterator iter = m_mapLevels.begin();
	while (iter != m_mapLevels.end())
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
	wstring levelName = m_pCurLevel->GetName();

	DeleteLevel(m_pCurLevel);
	InsertLevel(levelName, _ResetData);
	ChangeLevel(_ResetData);
}

void CLevelMgr::InsertLevel(const wstring& _strName, CLevel* _Level)
{
	m_mapLevels.insert(make_pair(_strName, _Level));
}

void CLevelMgr::LoadLevel(CLevel* _Level)
{
	_Level->ChangeState(LEVEL_STATE::STOP);

	DeleteLevel(_Level->GetName());
	InsertLevel(_Level->GetName(), _Level);

	ChangeLevel(_Level);
}

void CLevelMgr::CheckLevelClear()
{
	static bool levelLoading = false;

	if (!ThreadMgr::GetInst()->IsThreadRun())
	{
		levelLoading = false;
	}

	if (m_pCurLevel->GetName() == L"main menu level")
	{
		if (m_pCurLevel->GetLevelEnd() && !levelLoading)
		{
			static float mL2 = 0.0f;
			mL2 += DT;
			CRenderMgr::GetInst()->GetMainCam()->SetFadeTime(2.f, false);

			if (mL2 > 2.f)
			{
				LoadingMgr::GetInst()->SetStage(0);
				levelLoading = true;
				ThreadMgr::GetInst()->AddThread(
					[this]()
					{
						LoadingLevel(L"main level 1");
						ThreadMgr::GetInst()->AddThreadEnd();
					}
				);
			}
		}
	}

	if (m_pCurLevel->GetName() == L"main level 1")
	{

		if (m_pCurLevel->GetState() == LEVEL_STATE::PLAY
			&& m_pCurLevel->GetTickCnt() > 0 && m_pCurLevel->GetTickCnt() < 10)
		{
			CRenderMgr::GetInst()->GetMainCam()->SetFadeTime(2.f, true);
		}
		if (m_pCurLevel->GetState() == LEVEL_STATE::PLAY
			&& m_pCurLevel->GetTickCnt() > 0 && m_pCurLevel->GetTickCnt() < 10)
		{

		}
		if (m_pCurLevel->GetLevelEnd() && !levelLoading)
		{
			static float mL = 0.0f;
			mL += DT;
			CRenderMgr::GetInst()->GetMainCam()->SetFadeTime(2.f, false);

			if (mL > 2.f)
			{
				LoadingMgr::GetInst()->SetStage(1);
				levelLoading = true;
				ThreadMgr::GetInst()->AddThread(
					[this]()
					{
						LoadingLevel(L"main level 2");
						ThreadMgr::GetInst()->AddThreadEnd();
					}
				);
			}
		}
	}
	if (m_pCurLevel->GetName() == L"main level 2")
	{
		if (m_pCurLevel->GetState() == LEVEL_STATE::PLAY
			&& m_pCurLevel->GetTickCnt() > 0 && m_pCurLevel->GetTickCnt() < 10)
		{
			CRenderMgr::GetInst()->GetMainCam()->SetFadeTime(2.f, true);
		}
	}
}
