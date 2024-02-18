#include "pch.h"
#include "TestLevel.h"

#include <Engine\CollisionMgr.h>
#include <Engine\CLevelMgr.h>
#include <Engine\CLevel.h>
#include <Engine\CLayer.h>
#include <Engine\CGameObject.h>
#include <Engine\components.h>

#include <Engine\CResMgr.h>
#include <Engine\PhysXMgr.h>
#include <Engine\CRenderMgr.h>
#include <Engine\ObjPickerMgr.h>
#include <Engine\WeaponMgr.h>

#include <Script\CPlayerScript.h>
#include <Script\CEnemyScript.h>
#include <Script\CCameraMoveScript.h>

#include "CLevelSaveLoad.h"


#include <Engine\CSetColorShader.h>
#include <Engine\PlayerMgr.h>

#include <Script\CGunScript.h>

#include "../Script/CWarriorScript.h"

//#include <Engine/PhysXT.h>


void CreateTestLevel()
{
	//Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"sound\\BGM_Stage1.wav");
	//pSound->Play(1, 0.5f, false);
	CLevel* pCurLevel = CLevelMgr::GetInst()->CreateLevel(L"main level");
	CLevelMgr::GetInst()->ChangeLevel(pCurLevel);
	//CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurLevel();
	pCurLevel->ChangeState(LEVEL_STATE::STOP);

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
	PreloadGameObject(pMainCam, Vec3(0.f, 0.f, 0.f), LAYER_TYPE::Default);

	CGameObject* pUICam = new CGameObject;
	pUICam->SetName(L"UICamera");

	pUICam->AddComponent(new CTransform);
	pUICam->AddComponent(new CCamera);

	pUICam->Camera()->SetFarZ(1000000.f);
	pUICam->Camera()->SetProjType(PROJ_TYPE::ORTHOGRAPHIC);
	pUICam->Camera()->SetCameraIndex(1);
	pUICam->Camera()->SetLayerMaskAll(false);
	pUICam->Camera()->SetLayerMask((int)LAYER_TYPE::ViewPortUI, true);

	PreloadGameObject(pUICam, Vec3(0.f, 0.f, 0.f), LAYER_TYPE::Default);

	{
		CGameObject* pCrossHair = new CGameObject;

		pCrossHair->SetName(L"UI Cross Hair");
		pCrossHair->AddComponent(new CTransform);
		pCrossHair->AddComponent(new CMeshRender);

		pCrossHair->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
		pCrossHair->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"UI2DShaderMtrl"), 0);
		pCrossHair->MeshRender()->GetMaterial(0)->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"texture\\Crosshair.png"));
		pCrossHair->Transform()->SetRelativeScale(Vec3(80.f, 80.f, 1.f));
		PreloadGameObject(pCrossHair, Vec3(0.f, 0.f, 0.f), LAYER_TYPE::ViewPortUI);
	}


	CGameObject* pSunLight = new CGameObject;
	pSunLight->SetName(L"Sun Light");
	pSunLight->AddComponent(new CTransform);
	pSunLight->AddComponent(new CLight3D);

	pSunLight->Transform()->SetRelativeRot(Vec3(XM_PI / 4.f, XM_PI / 4.f, 0.f));
	pSunLight->Light3D()->SetRadius(500.f);
	pSunLight->Light3D()->SetShadow(true);
	pSunLight->Light3D()->SetLightDepthCoeff(0.003f);
	pSunLight->Light3D()->SetFloatConstant(0, 8192.f);
	pSunLight->Light3D()->SetFloatConstant(1, 9.0f);
	pSunLight->Light3D()->SetLightType(LIGHT_TYPE::DIRECTIONAL);
	pSunLight->Light3D()->SetLightColor(Vec3(1.f, 1.f, 1.f));
	pSunLight->Light3D()->SetLightAmbient(Vec3(0.15f, 0.15f, 0.15f));

	PreloadGameObject(pSunLight, Vec3(-2000.f, 60000.f, -2000.f), LAYER_TYPE::Default);

	//{
	//	CGameObject* Light = new CGameObject;
	//	Light->SetName(L"Spot Light");
	//	Light->AddComponent(new CTransform);
	//	Light->AddComponent(new CLight3D);

	//	Light->Transform()->SetRelativeRot(Vec3(XM_PI / 4.f, XM_PI / 4.f, 0.f));
	//	Light->Light3D()->SetRadius(500.f);
	//	Light->Light3D()->SetShadow(true);
	//	Light->Light3D()->SetFloatConstant(0, 5);
	//	Light->Light3D()->SetFloatConstant(1, 0.0004);
	//	Light->Light3D()->SetLightType(LIGHT_TYPE::SPOT);
	//	Light->Light3D()->SetLightColor(Vec3(1.f, 1.f, 1.f));
	//	Light->Light3D()->SetLightAmbient(Vec3(0.15f, 0.15f, 0.15f));

	//	PreloadGameObject(Light, Vec3(100.f, 100.f, 100.f), 0);
	//}
	//{
	//	CGameObject* Light = new CGameObject;
	//	Light->SetName(L"Point Light");
	//	Light->AddComponent(new CTransform);
	//	Light->AddComponent(new CLight3D);

	//	Light->Transform()->SetRelativeRot(Vec3(0.f, 0.f, 0.f));
	//	Light->Light3D()->SetRadius(500.f);
	//	Light->Light3D()->SetShadow(true);
	//	Light->Light3D()->SetFloatConstant(0, 5);
	//	Light->Light3D()->SetFloatConstant(1, 0.0004);
	//	Light->Light3D()->SetLightType(LIGHT_TYPE::POINT);
	//	Light->Light3D()->SetLightColor(Vec3(1.f, 1.f, 1.f));
	//	Light->Light3D()->SetLightAmbient(Vec3(0.15f, 0.15f, 0.15f));

	//	PreloadGameObject(Light, Vec3(100.f, 100.f, 100.f), 0);
	//}

	CGameObject* pDistortion = new CGameObject;
	pDistortion->SetName(L"Distortion");
	pDistortion->AddComponent(new CTransform);
	pDistortion->AddComponent(new CMeshRender);
	pDistortion->Transform()->SetRelativeScale(Vec3(100.f, 100.f, 100.f));

	pDistortion->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"SphereMesh"));
	pDistortion->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"DistortionMtrl"), 0);
	pDistortion->MeshRender()->GetMaterial(0)->SetTexParam(TEX_1, CResMgr::GetInst()->FindRes<CTexture>(L"texture\\noise\\noise_04.jpg"));

	PreloadGameObject(pDistortion, Vec3(0.f, 0.f, 0.f), LAYER_TYPE::Default);

	//CGameObject* pCube = new CGameObject;
	//pCube->SetName(L"Cube1");
	//pCube->AddComponent(new CTransform);
	//pCube->AddComponent(new CMeshRender);
	//pCube->AddComponent(new CRigidBody(RIGID_BODY_SHAPE_TYPE::BOX, RIGID_BODY_TYPE::DYNAMIC));
	//pCube->AddComponent(new CCollider3D);

	//pCube->Transform()->SetRelativeScale(Vec3(100.f, 100.f, 100.f));

	//pCube->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"CubeMesh"));
	//pCube->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Std3D_DeferredMtrl"), 0);

	//pCube->MeshRender()->GetMaterial(0)->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"texture\\tile\\TILE_01.tga"));
	//
	//PreloadGameObject(pCube, Vec3(0.f, 10000.f, 0.f), L"Objects");
	//
	{
		CGameObject* pCube = new CGameObject;
		pCube->SetName(L"Enemy1");
		pCube->AddComponent(new CTransform);
		pCube->AddComponent(new CMeshRender);
		pCube->AddComponent(new CEnemyScript);
		pCube->AddComponent(new CRigidBody(RIGID_BODY_SHAPE_TYPE::BOX, RIGID_BODY_TYPE::DYNAMIC));
		pCube->AddComponent(new CCollider3D);
		pCube->AddComponent(new CGizmo);
		pCube->Transform()->SetRelativeScale(Vec3(100.f, 100.f, 100.f));

		pCube->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"CubeMesh"));
		pCube->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Std3D_DeferredMtrl"), 0);

		//pCube->MeshRender()->GetDynamicMaterial(0)->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"texture\\tile\\TILE_01.tga"));
		//pCube->MeshRender()->GetDynamicMaterial(0)->SetTexParam(TEX_1, CResMgr::GetInst()->FindRes<CTexture>(L"texture\\tile\\TILE_01_N.tga"));

		pCube->MeshRender()->GetMaterial(0)->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"texture\\tile\\TILE_01.tga"));
		pCube->MeshRender()->GetMaterial(0)->SetTexParam(TEX_1, CResMgr::GetInst()->FindRes<CTexture>(L"texture\\tile\\TILE_01_N.tga"));

		PreloadGameObject(pCube, Vec3(500.f, 90.f, 500.f), LAYER_TYPE::Enemy);
	}
	//{
	//	CGameObject* pCube = new CGameObject;
	//	pCube->SetName(L"Monster2");
	//	pCube->AddComponent(new CTransform);
	//	pCube->AddComponent(new CMeshRender);
	//	pCube->AddComponent(new CEnemyScript);
	//	pCube->AddComponent(new CRigidBody(RIGID_BODY_SHAPE_TYPE::BOX, RIGID_BODY_TYPE::STATIC));
	//	pCube->AddComponent(new CCollider3D);
	//	pCube->AddComponent(new CGizmo);
	//	pCube->Transform()->SetRelativeScale(Vec3(100.f, 100.f, 100.f));

	//	pCube->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"SphereMesh"));
	//	pCube->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Std3D_DeferredMtrl"), 0);

	//	pCube->MeshRender()->GetMaterial(0)->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"texture\\tile\\TILE_01.tga"));
	//	pCube->MeshRender()->GetMaterial(0)->SetTexParam(TEX_1, CResMgr::GetInst()->FindRes<CTexture>(L"texture\\tile\\TILE_01_N.tga"));
	//	//pCube->MeshRender()->GetMaterial(0)->SetTexParam(TEX_2, CResMgr::GetInst()->FindRes<CTexture>(L"texture\\tile\\TILE_01_N.tga"));
	//	//pCube->MeshRender()->GetMaterial(0)->SetAnim2D(true);

	//	PreloadGameObject(pCube, Vec3(-500.f, 90.f, -500.f), L"Monster");
	//}

	CGameObject* pPlane = new CGameObject;
	pPlane->SetName(L"Plane");
	pPlane->AddComponent(new CTransform);
	pPlane->AddComponent(new CMeshRender);
	pPlane->AddComponent(new CRigidBody(RIGID_BODY_SHAPE_TYPE::BOX));
	//pPlane->AddComponent(new CCollider3D);

	pPlane->Transform()->SetRelativeScale(Vec3(100000.f, 100000.f, 1.f));
	pPlane->Transform()->SetRelativeRot(Vec3(XM_PI / 2.f, 0.f, 0.f));

	pPlane->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	pPlane->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Std3D_DeferredMtrl"), 0);

	pPlane->MeshRender()->GetMaterial(0)->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"texture\\tile\\TILE_01.tga"));
	pPlane->MeshRender()->GetMaterial(0)->SetTexParam(TEX_1, CResMgr::GetInst()->FindRes<CTexture>(L"texture\\tile\\TILE_01_N.tga"));
	PreloadGameObject(pPlane, Vec3(0.f, -10.f, 0.f), LAYER_TYPE::Environment);

	{
		CGameObject* pPlane = new CGameObject;
		pPlane->SetName(L"Plane");
		pPlane->AddComponent(new CTransform);
		pPlane->AddComponent(new CMeshRender);
		pPlane->AddComponent(new CRigidBody(RIGID_BODY_SHAPE_TYPE::BOX));
		pPlane->AddComponent(new CCollider3D);

		pPlane->Transform()->SetRelativeScale(Vec3(10000.f, 10000.f, 100.f));
		pPlane->Transform()->SetRelativeRot(Vec3(0.f, 0.f, 0.f));

		pPlane->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"CubeMesh"));
		pPlane->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Std3D_DeferredMtrl"), 0);

		pPlane->MeshRender()->GetMaterial(0)->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"texture\\tile\\TILE_01.tga"));
		pPlane->MeshRender()->GetMaterial(0)->SetTexParam(TEX_1, CResMgr::GetInst()->FindRes<CTexture>(L"texture\\tile\\TILE_01_N.tga"));
		PreloadGameObject(pPlane, Vec3(0.f, -10.f, 0.f), LAYER_TYPE::Environment);
	}
	//CGameObject* pObject = new CGameObject;
	//pObject->SetName(L"Tess Object");
	//pObject->AddComponent(new CTransform);
	//pObject->AddComponent(new CMeshRender);

	//pObject->Transform()->SetRelativeScale(Vec3(200.f, 200.f, 200.f));
	//pObject->Transform()->SetRelativeRot(Vec3(0.f, 0.f, 0.f));

	//pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	//pObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Std3D_DeferredMtrl"), 0);
	//pObject->MeshRender()->GetMaterial(0)->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"texture\\skybox\\Sky01.png"));
	//SpawnGameObject(pObject, Vec3(0.f, 0.f, 100.f), 1);

	CGameObject* pSkyBox = new CGameObject;
	pSkyBox->SetName(L"SkyBox");

	pSkyBox->AddComponent(new CTransform);
	pSkyBox->AddComponent(new CSkyBox);

	pSkyBox->Transform()->SetRelativeScale(Vec3(100.f, 100.f, 100));
	pSkyBox->SkyBox()->SetSkyBoxType(SKYBOX_TYPE::SPHERE);
	pSkyBox->SkyBox()->SetSkyBoxTexture(CResMgr::GetInst()->FindRes<CTexture>(L"texture\\skybox\\Sky.tga"));

	//SpawnGameObject(pSkyBox, Vec3(0.f, 0.f, 0.f), 0);
	PreloadGameObject(pSkyBox, Vec3(0.f, 0.f, 0.f), LAYER_TYPE::Environment);

	//CGameObject* pObject = new CGameObject;
	//pObject->SetName(L"Decal");
	//pObject->AddComponent(new CTransform);
	//pObject->AddComponent(new CDecal);
	//pObject->Decal()->SetDeferredDecal(true);
	////pObject->Decal()->SetEmissiveDecal(true);

	//pObject->Transform()->SetRelativeScale(Vec3(100.f, 100.f, 100.f));
	//pObject->Transform()->SetRelativeRot(Vec3(0.f, 0.f, 0.f));
	////CCamera* cam = CRenderMgr::GetInst()->GetEditorCam();
	////pObject->SetFollowObj(cam->GetOwner());

	////SpawnGameObject(pObject, Vec3(0.f, 0.f, 0.f), L"Default");
	//PreloadGameObject(pObject, Vec3(0.f, 0.f, 0.f), 1);

	// LandScape Object
	//CGameObject* pLandScape = new CGameObject;
	//pLandScape->SetName(L"LandScape");

	//pLandScape->AddComponent(new CTransform);
	//PhysXMgr::GetInst()->CreateScene(Vec3(0.f, 0.f, 0.f));
	//pLandScape->Transform()->SetRelativeScale(Vec3(200.f, 1000.f, 200.f));

	//pLandScape->AddComponent(new CLandScape);

	////pLandScape->LandScape()->SetBrushGuideLine(pObject);
	//pLandScape->LandScape()->SetHeightMapName(L"HeightMap2");
	//pLandScape->LandScape()->SetColorMapName(L"ColorMap2");
	////pLandScape->LandScape()->SetFace(32, 32);
	//pLandScape->LandScape()->SetFrustumCheck(false);
	////pLandScape->LandScape()->SetHeightMap(CResMgr::GetInst()->FindRes<CTexture>(L"texture\\HeightMap_01.jpg"));

	//PreloadGameObject(pLandScape, Vec3(-100.f, 0.f, 0.f), 0);

	// ============
	// FBX Loading
	// ============	
	{
		Ptr<CMeshData> pMeshData = nullptr;
		CGameObject* pObj = nullptr;

		//pMeshData = CResMgr::GetInst()->LoadFBX(L"fbx\\Monster.fbx");
		//pMeshData = CResMgr::GetInst()->LoadFBX(L"fbx\\nomad.fbx");
		pMeshData = CResMgr::GetInst()->LoadFBX(L"fbx\\soldier_hands.fbx");
		//pMeshData = CResMgr::GetInst()->LoadFBX(L"fbx\\anim_NLA.fbx");
		//pMeshData = CResMgr::GetInst()->FindRes<CMeshData>(L"meshdata\\house.mdat");
		//pObj = pMeshData->Instantiate();
		//pObj->SetName(L"House");

		//pMeshData = CResMgr::GetInst()->LoadFBX(L"fbx\\monster.fbx");
		//pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"meshdata\\monster.mdat", L"meshdata\\monster.mdat");
		//pMeshData = CResMgr::GetInst()->FindRes<CMeshData>(L"meshdata\\anim_NLA.mdat");
		pObj = pMeshData->Instantiate(Vec3(300.f, 300.f, 300.f));
		pObj->SetName(L"player");
		//pObj->AddComponent(new CPathFind);
		//pObj->AddComponent(new CEnemyScript);
		pObj->AddComponent(new CPlayerScript);
		pObj->AddComponent(new CRigidBody(RIGID_BODY_SHAPE_TYPE::BOX, RIGID_BODY_TYPE::DYNAMIC));
		pObj->RigidBody()->SetCreature(true);
		pObj->AddComponent(new CCollider3D);
		pObj->AddComponent(new CGizmo);
		pObj->Transform()->SetRelativePosOffset(Vec3(0.f, 350.f, 0.f));

		//pObj->AddChild(pMainCam);
		pObj->SetFollowObj(pMainCam);
		//pMainCam->SetFollowObj(pObj);
		//pMainCam->Transform()->SetFollowOffset(Vec3(50.f, 350.f, 0.f));

		//SpawnGameObject(pObj, Vec3(0.f, 0.f, 100.f), L"Default");
		ObjPickerMgr::GetInst()->SetPickObj(pObj);
		PlayerMgr::GetInst()->SetPlayer(pObj);
		PreloadGameObject(pObj, Vec3(50.f, 1000.f, 50.f), LAYER_TYPE::Player);
	}
	{
		Ptr<CMeshData> pMeshData = nullptr;
		CGameObject* pObj = nullptr;
		pMeshData = CResMgr::GetInst()->LoadFBX(L"fbx\\smg.fbx");
		pObj = pMeshData->Instantiate(Vec3(300.f, 300.f, 300.f));
		pObj->SetName(L"smg");

		pObj->AddComponent(new CGunScript);
		pObj->AddComponent(new CGizmo);
		pObj->SetIsItem(true);

		WeaponMgr::GetInst()->AddWeapon(pObj);

		PreloadGameObject(pObj, Vec3(500.f, 100.f, 50.f), LAYER_TYPE::Item);
	}
	{
		Ptr<CMeshData> pMeshData = nullptr;
		CGameObject* pObj = nullptr;
		pMeshData = CResMgr::GetInst()->LoadFBX(L"fbx\\sniper.fbx");
		pObj = pMeshData->Instantiate(Vec3(300.f, 300.f, 300.f));
		pObj->SetName(L"sniper");

		pObj->AddComponent(new CGunScript);
		pObj->AddComponent(new CGizmo);
		pObj->SetIsItem(true);

		WeaponMgr::GetInst()->AddWeapon(pObj);

		PreloadGameObject(pObj, Vec3(500.f, 100.f, 50.f), LAYER_TYPE::Item);
	}
	{
		Ptr<CMeshData> pMeshData = nullptr;
		CGameObject* pObj = nullptr;
		pMeshData = CResMgr::GetInst()->LoadFBX(L"fbx\\pistol.fbx");
		pObj = pMeshData->Instantiate(Vec3(300.f, 300.f, 300.f));
		pObj->SetName(L"pistol");

		pObj->AddComponent(new CGunScript);
		pObj->AddComponent(new CGizmo);
		pObj->SetIsItem(true);

		WeaponMgr::GetInst()->AddWeapon(pObj);

		PreloadGameObject(pObj, Vec3(500.f, 100.f, 50.f), LAYER_TYPE::Item);
	}
	{
		WeaponMgr::GetInst()->ChangeWeapon(SMG_IDX);
	}
	//{
	//	Ptr<CMeshData> pMeshData = nullptr;
	//	CGameObject* pObj = nullptr;
	//	pMeshData = CResMgr::GetInst()->LoadFBX(L"fbx\\nomad.fbx");
	//	pObj = pMeshData->Instantiate();
	//	pObj->SetName(L"Monster1");
	//	pObj->AddComponent(new CEnemyScript);
	//	pObj->AddComponent(new CRigidBody(RIGID_BODY_SHAPE_TYPE::BOX, RIGID_BODY_TYPE::DYNAMIC));
	//	pObj->RigidBody()->SetCreature(true);
	//	pObj->AddComponent(new CCollider3D);
	//	pObj->AddComponent(new CGizmo);
	//	//EnemyMgr::GetInst()->MonsterGen(MONSTER_TYPE::NOMAD);
	//	PreloadGameObject(pObj, Vec3(50.f, 90.f, 500.f), L"Monster");
	//}
	//{
	//	Ptr<CMeshData> pMeshData = nullptr;
	//	CGameObject* pObj = nullptr;

	//	pMeshData = CResMgr::GetInst()->LoadFBX(L"fbx\\map_statics.fbx");
	//	pObj = pMeshData->Instantiate(Vec3(300.f, 300.f, 300.f));
	//	pObj->AddComponent(new CRigidBody(RIGID_BODY_SHAPE_TYPE::MESH));
	//	//pObj->AddComponent(new CCollider3D);

	//	pObj->SetName(L"fbx terrain");
	//	PreloadGameObject(pObj, Vec3(200.f, 100.f, 200.f), LAYER_TYPE::Environment);
	//}
	{
		Ptr<CMeshData> pMeshData = nullptr;
		CGameObject* pObj = nullptr;

		pMeshData = CResMgr::GetInst()->LoadFBX(L"fbx\\nomad.fbx");
		pObj = pMeshData->Instantiate(Vec3(300.f, 300.f, 300.f));
		
		pObj->SetName(L"fbx nomad");

		pObj->AddComponent(new CEnemyScript);
		pObj->AddComponent(new CRigidBody(RIGID_BODY_SHAPE_TYPE::BOX, RIGID_BODY_TYPE::DYNAMIC));
		pObj->AddComponent(new CCollider3D);
		pObj->AddComponent(new CGizmo);

		PreloadGameObject(pObj, Vec3(500.f, 100.f, 500.f), LAYER_TYPE::Enemy);

		{
			Ptr<CMeshData> pMeshData = nullptr;
			CGameObject* pGun = nullptr;
			pMeshData = CResMgr::GetInst()->LoadFBX(L"fbx\\dahl.fbx");
			pGun = pMeshData->Instantiate(Vec3(300.f, 300.f, 300.f));
			pGun->SetName(L"enemy_gun");

			pGun->AddComponent(new CGunScript);
			pGun->AddComponent(new CGizmo);
			pGun->SetIsItem(true);
			pObj->AddGun(pGun);

			PreloadGameObject(pGun, Vec3(500.f, 100.f, 50.f), LAYER_TYPE::Item);
		}

	}

	{
		Ptr<CMeshData> pMeshData = nullptr;
		CGameObject* pObj = nullptr;
		
		pMeshData = CResMgr::GetInst()->LoadFBX(L"fbx\\fire_breath.fbx");
		pObj = pMeshData->Instantiate(Vec3(10000.f, 10000.f, 10000.f));
		pObj->AddComponent(new CCollider3D(false));
		pObj->AddComponent(new CGizmo);
		pObj->Collider3D()->SetScale(Vec3(1500.f, 1500.f, 13000.f));
		//pObj->Collider3D()->SetCenter(true);

		pObj->SetName(L"fbx fire breath");
		pObj->MeshRender()->GetMaterial(0)->SetTexFlow(true);
		pObj->MeshRender()->GetMaterial(0)->SetFlowSpeed(2.f);
		pObj->MeshRender()->GetMaterial(0)->SetFlowDir(Vec2(-0.5f, 0.0f));
		PreloadGameObject(pObj, Vec3(200.f, 100.f, 15000.f), LAYER_TYPE::Environment);
	}
	{
		Ptr<CMeshData> pMeshData = nullptr;
		CGameObject* pObj = nullptr;

		pMeshData = CResMgr::GetInst()->LoadFBX(L"fbx\\warrior_2.fbx");
		pObj = pMeshData->Instantiate(Vec3(1000.f, 1000.f, 1000.f));
		pObj->SetName(L"fbx warrior");
		pObj->SetIsWarrior(true);
		//pObj->AddComponent(new CAnimator2D);
		pObj->AddComponent(new CWarriorScript);
		

		PreloadGameObject(pObj, Vec3(500.f, 100.f, 500.f), LAYER_TYPE::Enemy);
	}



	pCurLevel = CLevelMgr::GetInst()->CreateLevel(L"main menu level");
	CLevelMgr::GetInst()->ChangeLevel(pCurLevel);
	pCurLevel->ChangeState(LEVEL_STATE::STOP);

	pMainCam = new CGameObject;
	pMainCam->SetName(L"MainCamera");

	pMainCam->AddComponent(new CTransform);
	pMainCam->AddComponent(new CCamera);
	pMainCam->AddComponent(new CCameraMoveScript);

	pMainCam->Camera()->SetFarZ(1000000.f);
	pMainCam->Camera()->SetProjType(PROJ_TYPE::PERSPECTIVE);
	pMainCam->Camera()->SetCameraIndex(0);
	pMainCam->Camera()->SetLayerMaskAll(true);
	pMainCam->Camera()->SetLayerMask(31, false);
	PreloadGameObject(pMainCam, Vec3(0.f, 0.f, 0.f), LAYER_TYPE::Default);

	pUICam = new CGameObject;
	pUICam->SetName(L"UICamera");

	pUICam->AddComponent(new CTransform);
	pUICam->AddComponent(new CCamera);

	pUICam->Camera()->SetFarZ(1000000.f);
	pUICam->Camera()->SetProjType(PROJ_TYPE::ORTHOGRAPHIC);
	pUICam->Camera()->SetCameraIndex(1);
	pUICam->Camera()->SetLayerMaskAll(false);
	pUICam->Camera()->SetLayerMask(31, true);
	PreloadGameObject(pUICam, Vec3(0.f, 0.f, 0.f), LAYER_TYPE::Default);
	///*CGameObject* */pSunLight = new CGameObject;
	//pSunLight->SetName(L"SunLight");
	//pSunLight->AddComponent(new CTransform);
	//pSunLight->AddComponent(new CLight3D);

	//pSunLight->Transform()->SetRelativeRot(Vec3(XM_PI / 4.f, XM_PI / 4.f, 0.f));
	//pSunLight->Light3D()->SetRadius(500.f);
	//pSunLight->Light3D()->SetLightType(LIGHT_TYPE::DIRECTIONAL);
	//pSunLight->Light3D()->SetLightColor(Vec3(1.f, 1.f, 1.f));
	//pSunLight->Light3D()->SetLightAmbient(Vec3(0.15f, 0.15f, 0.15f));

	//////SpawnGameObject(pSunLight, Vec3(-2000.f, 3500.f, -2000.f), 0);
	//PreloadGameObject(pSunLight, Vec3(-2000.f, 3500.f, -2000.f), 1);
	//{
	//	CGameObject* pSkyBox = new CGameObject;
	//	pSkyBox->SetName(L"SkyBox");

	//	pSkyBox->AddComponent(new CTransform);
	//	pSkyBox->AddComponent(new CSkyBox);

	//	pSkyBox->Transform()->SetRelativeScale(Vec3(100.f, 100.f, 100));
	//	pSkyBox->SkyBox()->SetSkyBoxType(SKYBOX_TYPE::SPHERE);
	//	pSkyBox->SkyBox()->SetSkyBoxTexture(CResMgr::GetInst()->FindRes<CTexture>(L"texture\\skybox\\2k_stars_milky_way.jpg"));

	//	PreloadGameObject(pSkyBox, Vec3(0.f, 0.f, 0.f), 1);

	//}

	CollisionMgr::GetInst()->SetLayerIntersect(LAYER_TYPE::Enemy, LAYER_TYPE::PlayerBullet, true);
	//CollisionMgr::GetInst()->SetLayerIntersect(L"Default", L"Bullet", true);
	CollisionMgr::GetInst()->SetLayerIntersect(LAYER_TYPE::Player, LAYER_TYPE::EnemyBullet, true);
	//CollisionMgr::GetInst()->SetLayerIntersect(LAYER_TYPE::Enemy, LAYER_TYPE::Environment, true);
	//CollisionMgr::GetInst()->SetLayerIntersect(LAYER_TYPE::Player,LAYER_TYPE::Environment, true);
}
