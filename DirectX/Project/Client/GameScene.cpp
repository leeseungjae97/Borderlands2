#include "pch.h"
#include "GameScene.h"

#include <Engine\CLevelMgr.h>
#include <Engine\CLevel.h>
#include <Engine\CLayer.h>
GameScene::GameScene()
{
	CLevel* m_pGameLevel = CreateLevel(L"game", true);
	CreateLevel(L"game_1");
	//ChangeLevel(L"game");

	m_pGameLevel->GetLayer(0)->SetName(L"Default");
	m_pGameLevel->GetLayer(1)->SetName(L"Object");
	m_pGameLevel->GetLayer(10)->SetName(L"Light");
	m_pGameLevel->GetLayer(31)->SetName(L"UI");
}

GameScene::~GameScene()
{
}

void GameScene::init()
{
}

void GameScene::enter()
{
}

void GameScene::exit()
{
}