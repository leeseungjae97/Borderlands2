#include "pch.h"
#include "MainScene.h"

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

MainScene::MainScene()
{
	CLevel* m_pMainMenuLevel = CreateLevel(L"main menu", true);
	//ChangeLevel(L"main menu");

	m_pMainMenuLevel->GetLayer(0)->SetName(L"Default");
	m_pMainMenuLevel->GetLayer(1)->SetName(L"Object");
	m_pMainMenuLevel->GetLayer(10)->SetName(L"Light");
	m_pMainMenuLevel->GetLayer(31)->SetName(L"UI");
}

MainScene::~MainScene()
{
}

void MainScene::init()
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

	CGameObject* pSunLight = new CGameObject;
	pSunLight->SetName(L"Light");
	pSunLight->AddComponent(new CTransform);
	pSunLight->AddComponent(new CLight3D);

	pSunLight->Transform()->SetRelativeRot(Vec3(XM_PI / 4.f, XM_PI / 4.f, 0.f));
	pSunLight->Light3D()->SetRadius(500.f);
	pSunLight->Light3D()->SetLightType(LIGHT_TYPE::DIRECTIONAL);
	pSunLight->Light3D()->SetLightColor(Vec3(1.f, 1.f, 1.f));
	pSunLight->Light3D()->SetLightAmbient(Vec3(0.15f, 0.15f, 0.15f));

	SpawnGameObject(pSunLight, Vec3(-2000.f, 3500.f, -2000.f), 0);

	CGameObject* pSphere = new CGameObject;
	pSphere->SetName(L"Sphere1");
	pSphere->AddComponent(new CTransform);
	pSphere->AddComponent(new CMeshRender);

	pSphere->Transform()->SetRelativeScale(Vec3(1000.f, 1000.f, 1000.f));

	pSphere->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"SphereMesh"));
	pSphere->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Std3D_DeferredMtrl"), 0);

	pSphere->MeshRender()->GetMaterial(0)->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"texture\\tile\\TILE_01.tga"));

	SpawnGameObject(pSphere, Vec3(500.f, 500.f, 0.f), L"Object");

	pSphere = new CGameObject;
	pSphere->SetName(L"Sphere2");
	pSphere->AddComponent(new CTransform);
	pSphere->AddComponent(new CMeshRender);

	pSphere->Transform()->SetRelativeScale(Vec3(1000.f, 1000.f, 1000.f));

	pSphere->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"SphereMesh"));
	pSphere->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Std3D_DeferredMtrl"), 0);

	pSphere->MeshRender()->GetMaterial(0)->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"texture\\tile\\TILE_01.tga"));

	SpawnGameObject(pSphere, Vec3(0.f, 0.f, 0.f), L"Object");


	CGameObject* pRectMesh = new CGameObject;
	pRectMesh->SetName(L"Sphere");
	pRectMesh->AddComponent(new CTransform);
	pRectMesh->AddComponent(new CMeshRender);

	pRectMesh->Transform()->SetRelativeScale(Vec3(8000.f, 8000.f, 8000.f));
	pRectMesh->Transform()->SetRelativeRot(Vec3(XM_PI / 2.f, 0.f, 0.f));

	pRectMesh->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	pRectMesh->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Std3D_DeferredMtrl"), 0);

	pRectMesh->MeshRender()->GetMaterial(0)->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"texture\\tile\\TILE_01.tga"));

	SpawnGameObject(pRectMesh, Vec3(0.f, -1000.f, 0.f), L"Object");

	CGameObject* pSkyBox = new CGameObject;
	pSkyBox->SetName(L"SkyBox");

	pSkyBox->AddComponent(new CTransform);
	pSkyBox->AddComponent(new CSkyBox);

	pSkyBox->Transform()->SetRelativeScale(Vec3(100.f, 100.f, 100));
	pSkyBox->SkyBox()->SetSkyBoxType(SKYBOX_TYPE::SPHERE);
	pSkyBox->SkyBox()->SetSkyBoxTexture(CResMgr::GetInst()->FindRes<CTexture>(L"texture\\skybox\\2k_stars_milky_way.jpg"));

	SpawnGameObject(pSkyBox, Vec3(0.f, 0.f, 0.f), 0);

	CGameObject* pObject = new CGameObject;
	pObject->SetName(L"Decal");
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CDecal);
	pObject->Decal()->SetDeferredDecal(true);
	pObject->Decal()->SetEmissiveDecal(true);

	pObject->Transform()->SetRelativeScale(Vec3(100.f, 100.f, 100.f));
	pObject->Transform()->SetRelativeRot(Vec3(0.f, 0.f, 0.f));
	//CCamera* cam = CRenderMgr::GetInst()->GetEditorCam();
	//pObject->SetFollowObj(cam->GetOwner());

	SpawnGameObject(pObject, Vec3(0.f, 0.f, 0.f), L"Default");


	// ============
	// FBX Loading
	// ============	
	{
		Ptr<CMeshData> pMeshData = nullptr;
		CGameObject* pObj = nullptr;

		pMeshData = CResMgr::GetInst()->LoadFBX(L"fbx\\Monster.fbx");
		//pMeshData = CResMgr::GetInst()->FindRes<CMeshData>(L"meshdata\\house.mdat");
		//pObj = pMeshData->Instantiate();
		//pObj->SetName(L"House");

		//pMeshData = CResMgr::GetInst()->LoadFBX(L"fbx\\monster.fbx");
		//pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"meshdata\\monster.mdat", L"meshdata\\monster.mdat");
		//pMeshData = CResMgr::GetInst()->FindRes<CMeshData>(L"meshdata\\anim_NLA.mdat");
		pObj = pMeshData->Instantiate();
		pObj->SetName(L"Monster");

		SpawnGameObject(pObj, Vec3(0.f, 0.f, 100.f), L"Object");
	}
}

void MainScene::enter()
{
}

void MainScene::exit()
{
}