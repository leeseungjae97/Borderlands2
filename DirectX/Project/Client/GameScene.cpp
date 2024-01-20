#include "pch.h"
#include "GameScene.h"

#include <Engine\CLevelMgr.h>
#include <Engine\CLevel.h>
#include <Engine\CLayer.h>
#include <Engine\CGameObject.h>
#include <Engine\components.h>

#include <Engine\CResMgr.h>
#include <Engine\CRenderMgr.h>
#include <Engine\CCollisionMgr.h>

#include <Script\CPlayerScript.h>
#include <Script\CMonsterScript.h>
#include <Script\CCameraMoveScript.h>


#include <Engine/CSetColorShader.h>

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
	CGameObject* pMainCam = new CGameObject;
	pMainCam->SetName(L"MainCamera");

	pMainCam->AddComponent(new CTransform);
	pMainCam->AddComponent(new CCamera);
	pMainCam->AddComponent(new CCameraMoveScript);

	pMainCam->Camera()->SetFarZ(1000000.f);
	pMainCam->Camera()->SetProjType(PROJ_TYPE::PERSPECTIVE);
	pMainCam->Camera()->SetCameraIndex(0);
	pMainCam->Camera()->SetLayerMaskAll(true);
	pMainCam->Camera()->SetLayerMask(31, false);

	SpawnGameObject(pMainCam, Vec3(0.f, 0.f, 0.f), 0);
	CGameObject* pSkyBox = new CGameObject;
	pSkyBox->SetName(L"SkyBox");

	pSkyBox->AddComponent(new CTransform);
	pSkyBox->AddComponent(new CSkyBox);

	pSkyBox->Transform()->SetRelativeScale(Vec3(100.f, 100.f, 100));
	pSkyBox->SkyBox()->SetSkyBoxType(SKYBOX_TYPE::SPHERE);
	pSkyBox->SkyBox()->SetSkyBoxTexture(CResMgr::GetInst()->FindRes<CTexture>(L"texture\\skybox\\2k_stars_milky_way.jpg"));

	SpawnGameObject(pSkyBox, Vec3(0.f, 0.f, 0.f), 0);
}

void GameScene::enter()
{
}

void GameScene::exit()
{
}