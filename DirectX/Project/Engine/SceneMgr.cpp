#include "pch.h"
#include "SceneMgr.h"

#include "Scene.h"

void SceneMgr::create(SCENE_TYPE _Type, Scene* _Scene)
{
	m_vecScenes[(UINT)_Type] = _Scene;
}

void SceneMgr::init()
{
	for (int i = 0; i < (int)SCENE_TYPE::END; ++i)
	{
		if (nullptr == m_vecScenes[i]) continue;

		ChangeScene((SCENE_TYPE)i);
		m_vecScenes[i]->init();
	}
		


	ChangeScene(SCENE_TYPE::S_MAIN);
	//m_vecScenes[(UINT)SCENE_TYPE::S_MAIN];
	//m_vecScenes[SCENE_TYPE::S_GAME];
	//m_vecScenes[SCENE_TYPE::S_END];
}

void SceneMgr::release()
{
	Safe_Del_Array(m_vecScenes);
}

void SceneMgr::ChangeScene(SCENE_TYPE _Type)
{
	if(m_pCurScene)
		m_pCurScene->exit();

	m_pCurScene = m_vecScenes[(UINT)_Type];
	m_pCurScene->MainLvToCurLv();

	if (m_pCurScene)
		m_pCurScene->enter();
}
