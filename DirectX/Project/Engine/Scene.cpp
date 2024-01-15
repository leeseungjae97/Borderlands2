#include "pch.h"
#include "Scene.h"

#include "CLevel.h"
#include "CLevelMgr.h"

Scene::Scene()
	: m_vecLevels{}
{
}

Scene::~Scene()
{
	Safe_Del_Map(m_vecLevels);
}

void Scene::init()
{
}

void Scene::tick()
{
}

void Scene::finaltick()
{
}

CLevel* Scene::CreateLevel(const wstring& name, bool IsMain)
{
	CLevel* m_pLevel = new CLevel();

	m_vecLevels.insert(make_pair(name, m_pLevel));

	if (IsMain)
		m_stringMainLevelName = name;

	return m_pLevel;
}

CLevel* Scene::GetLevel(const wstring& name)
{
	CLevel* level = m_vecLevels.at(name);

	if (nullptr == level)
		return nullptr;

	return level;
}

void Scene::MainLvToCurLv()
{
	CLevel* plevel = GetLevel(m_stringMainLevelName);
	CLevelMgr::GetInst()->ChangeLevel(plevel);
	plevel->ChangeState(LEVEL_STATE::STOP);
}

void Scene::ChangeLevel(const wstring& name)
{
	if(FindLevel(name))
		CLevelMgr::GetInst()->ChangeLevel(GetLevel(name));
}

bool Scene::FindLevel(const wstring& name)
{
	CLevel* level = m_vecLevels.at(name);

	if (nullptr != level)
		return true;

	return false;
}
