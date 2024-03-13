#include "pch.h"
#include "CreateLevels.h"
#include "CLevelSaveLoad.h"

#include <Engine\CUI.h>
#include <Engine\CollisionMgr.h>
#include <Engine\CLevelMgr.h>
#include <Engine\CLevel.h>
#include <Engine\CLayer.h>
#include <Engine\CGameObject.h>
#include <Engine\components.h>

#include <Engine\CResMgr.h>
#include <Engine\CRenderMgr.h>
#include <Engine\ObjPickerMgr.h>
#include <Engine\WeaponMgr.h>
#include <Engine\PlayerMgr.h>

#include <Script\CPlayerScript.h>
#include <Script\CEnemyScript.h>
#include <Script\CCameraMoveScript.h>
#include <Script\CWeaponScript.h>
#include <Script\CWarriorScript.h>
#include <Script\CAttackBurnScript.h>
#include <Script\CAttackNormalScript.h>
#include <Script\CPathFindScript.h>
#include <Script\CMoveScript.h>

void CreateLevels()
{
	float fDefaultScale = 100.f;
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

	pMainCam->Camera()->SetFarZ(1000000.f);
	pMainCam->Camera()->SetProjType(PROJ_TYPE::PERSPECTIVE);
	pMainCam->Camera()->SetCameraIndex(0);
	pMainCam->Camera()->SetLayerMaskAll(true);
	pMainCam->Camera()->SetLayerMask((int)LAYER_TYPE::ViewPortUI, false);
	PreloadGameObject(pMainCam, Vec3(0.f, 0.f, 0.f), LAYER_TYPE::Camera);

	CGameObject* pUICam = new CGameObject;
	pUICam->SetName(L"UICamera");

	pUICam->AddComponent(new CTransform);
	pUICam->AddComponent(new CCamera);

	pUICam->Camera()->SetFarZ(1000000.f);
	pUICam->Camera()->SetProjType(PROJ_TYPE::ORTHOGRAPHIC);
	pUICam->Camera()->SetCameraIndex(1);
	pUICam->Camera()->SetLayerMaskAll(false);
	pUICam->Camera()->SetLayerMask((int)LAYER_TYPE::ViewPortUI, true);

	PreloadGameObject(pUICam, Vec3(0.f, 0.f, 0.f), LAYER_TYPE::Camera);

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

	CGameObject* pPlane = new CGameObject;
	pPlane->SetName(L"Plane");
	pPlane->AddComponent(new CTransform);
	pPlane->AddComponent(new CMeshRender);
	pPlane->AddComponent(new CRigidBody(RIGID_BODY_SHAPE_TYPE::BOX));
	pPlane->AddComponent(new CCollider3D);

	pPlane->Transform()->SetRelativeScale(Vec3(100000.f, 100000.f, 10.f));
	pPlane->Transform()->SetRelativeRot(Vec3(XM_PI / 2.f, 0.f, 0.f));

	pPlane->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	pPlane->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Std3D_DeferredMtrl"), 0);

	pPlane->MeshRender()->GetMaterial(0)->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"texture\\tile\\TILE_01.tga"));
	pPlane->MeshRender()->GetMaterial(0)->SetTexParam(TEX_1, CResMgr::GetInst()->FindRes<CTexture>(L"texture\\tile\\TILE_01_N.tga"));
	PreloadGameObject(pPlane, Vec3(0.f, -10.f, 0.f), LAYER_TYPE::Environment);

	//{
	//	CGameObject* pPlane = new CGameObject;
	//	pPlane->SetName(L"Plane");
	//	pPlane->AddComponent(new CTransform);
	//	pPlane->AddComponent(new CMeshRender);
	//	pPlane->AddComponent(new CRigidBody(RIGID_BODY_SHAPE_TYPE::BOX));
	//	pPlane->AddComponent(new CCollider3D);

	//	pPlane->Transform()->SetRelativeScale(Vec3(10000.f, 10000.f, 100.f));
	//	pPlane->Transform()->SetRelativeRot(Vec3(0.f, 0.f, 0.f));

	//	pPlane->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"CubeMesh"));
	//	pPlane->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Std3D_DeferredMtrl"), 0);

	//	pPlane->MeshRender()->GetMaterial(0)->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"texture\\tile\\TILE_01.tga"));
	//	pPlane->MeshRender()->GetMaterial(0)->SetTexParam(TEX_1, CResMgr::GetInst()->FindRes<CTexture>(L"texture\\tile\\TILE_01_N.tga"));
	//	PreloadGameObject(pPlane, Vec3(0.f, -10.f, 0.f), LAYER_TYPE::Environment);
	//}

	//CGameObject* pSkyBox = new CGameObject;
	//pSkyBox->SetName(L"SkyBox");

	//pSkyBox->AddComponent(new CTransform);
	//pSkyBox->AddComponent(new CSkyBox);

	//pSkyBox->Transform()->SetRelativeScale(Vec3(100.f, 100.f, 100));
	//pSkyBox->SkyBox()->SetSkyBoxType(SKYBOX_TYPE::SPHERE);
	//pSkyBox->SkyBox()->SetSkyBoxTexture(CResMgr::GetInst()->FindRes<CTexture>(L"texture\\skybox\\Zone3BossSky_Dif.png"));

	//PreloadGameObject(pSkyBox, Vec3(0.f, 0.f, 0.f), LAYER_TYPE::Default);

	{
		Ptr<CMeshData> pMeshData = nullptr;
		CGameObject* pSkyBox = nullptr;
		pMeshData = CResMgr::GetInst()->LoadFBX(L"fbx\\warrior_skybox.fbx");
		pSkyBox = pMeshData->Instantiate(Vec3(100.f, 100.f, 100.f), true);
		pSkyBox->SetName(L"SkyBox");

		//pSkyBox->SkyBox()->SetSkyBoxTexture(CResMgr::GetInst()->FindRes<CTexture>(L"texture\\skybox\\Sky.tga"));
		//pSkyBox->SkyBox()->SetSkyBoxType(SKYBOX_TYPE::SPHERE);
		PreloadGameObject(pSkyBox, Vec3(0.f, 0.f, 0.f), LAYER_TYPE::Default);
	}
	//{
	//	Ptr<CMeshData> pMeshData = nullptr;
	//	CGameObject* pSkyBox = nullptr;
	//	pMeshData = CResMgr::GetInst()->LoadFBX(L"fbx\\warrior_smoke_plume01.fbx");
	//	pSkyBox = pMeshData->Instantiate(Vec3(100.f, 100.f, 100.f));
	//	pSkyBox->SetName(L"Dust");

	//	//pSkyBox->SkyBox()->SetSkyBoxTexture(CResMgr::GetInst()->FindRes<CTexture>(L"texture\\skybox\\Sky.tga"));
	//	//pSkyBox->SkyBox()->SetSkyBoxType(SKYBOX_TYPE::SPHERE);
	//	PreloadGameObject(pSkyBox, Vec3(0.f, 0.f, 0.f), LAYER_TYPE::Default);
	//}

	//CGameObject* pObject = new CGameObject;
	//pObject->SetName(L"Decal");
	//pObject->AddComponent(new CTransform);
	//pObject->AddComponent(new CDecal);
	//pObject->Decal()->SetDeferredDecal(true);
	////pObject->Decal()->SetEmissiveDecal(true);

	//pObject->Transform()->SetRelativeScale(Vec3(100.f, 100.f, 100.f));
	//pObject->Transform()->SetRelativeRot(Vec3(0.f, 0.f, 0.f));

	//PreloadGameObject(pObject, Vec3(0.f, 0.f, 0.f), 1);

	{
		Ptr<CMeshData> pMeshData = nullptr;
		CGameObject* pObj = nullptr;
		pMeshData = CResMgr::GetInst()->LoadFBX(L"fbx\\soldier_hands.fbx");
		pObj = pMeshData->Instantiate(Vec3(fDefaultScale, fDefaultScale, fDefaultScale));
		pObj->SetName(L"player");

		pObj->AddComponent(new CRigidBody(RIGID_BODY_SHAPE_TYPE::BOX, RIGID_BODY_TYPE::DYNAMIC));
		pObj->AddComponent(new CCollider3D);
		pObj->AddComponent(new CPlayerScript);

		pObj->RigidBody()->SetCreature(true);

		pObj->AddComponent(new CGizmo);
		pObj->Transform()->SetRelativeScale(Vec3(fDefaultScale, fDefaultScale, fDefaultScale));
		pObj->Transform()->SetRelativePosOffset(Vec3(0.f, 150.f, 0.f));

		PlayerMgr::GetInst()->SetPlayer(pObj);
		//ObjPickerMgr::GetInst()->SetPickObj(pObj);
		PreloadGameObject(pObj, Vec3(800.f, 500.f, 50.f), LAYER_TYPE::Player);
	}
	{
		Ptr<CMeshData> pMeshData = nullptr;
		CGameObject* pObj = nullptr;
		pMeshData = CResMgr::GetInst()->LoadFBX(L"fbx\\smg_dahl.fbx");
		pObj = pMeshData->Instantiate(Vec3(fDefaultScale, fDefaultScale, fDefaultScale));
		pObj->SetName(L"smg");

		pObj->AddComponent(new CWeaponScript);
		pObj->AddComponent(new CGizmo);
		pObj->SetIsItem(true);

		WeaponMgr::GetInst()->AddWeapon(pObj);

		PreloadGameObject(pObj, Vec3(500.f, 100.f, 50.f), LAYER_TYPE::Item);
	}
	{
		Ptr<CMeshData> pMeshData = nullptr;
		CGameObject* pObj = nullptr;
		pMeshData = CResMgr::GetInst()->LoadFBX(L"fbx\\sniper_dahl.fbx");
		pObj = pMeshData->Instantiate(Vec3(fDefaultScale, fDefaultScale, fDefaultScale));
		pObj->SetName(L"sniper");

		pObj->AddComponent(new CWeaponScript);
		pObj->AddComponent(new CGizmo);
		pObj->SetIsItem(true);

		WeaponMgr::GetInst()->AddWeapon(pObj);

		PreloadGameObject(pObj, Vec3(500.f, 100.f, 50.f), LAYER_TYPE::Item);
	}
	{
		Ptr<CMeshData> pMeshData = nullptr;
		CGameObject* pObj = nullptr;
		pMeshData = CResMgr::GetInst()->LoadFBX(L"fbx\\pistol_dahl.fbx");
		pObj = pMeshData->Instantiate(Vec3(fDefaultScale, fDefaultScale, fDefaultScale));
		pObj->SetName(L"pistol");

		pObj->AddComponent(new CWeaponScript);
		pObj->AddComponent(new CGizmo);
		pObj->SetIsItem(true);

		WeaponMgr::GetInst()->AddWeapon(pObj);

		PreloadGameObject(pObj, Vec3(500.f, 100.f, 50.f), LAYER_TYPE::Item);
	}
	{
		WeaponMgr::GetInst()->ChangeWeapon(SMG_IDX);
	}


	//{
	//	CGameObject* pDistortion = new CGameObject;
	//	pDistortion->SetName(L"Lava Distortion");
	//	pDistortion->AddComponent(new CTransform);
	//	pDistortion->AddComponent(new CMeshRender);
	//	pDistortion->Transform()->SetRelativeScale(Vec3(100.f, 100.f, 100.f));

	//	pDistortion->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"mesh\\lava.mesh"));
	//	pDistortion->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"DistortionMtrl"), 0);
	//	pDistortion->MeshRender()->GetMaterial(0)->SetTexParam(TEX_1, CResMgr::GetInst()->FindRes<CTexture>(L"texture\\noise\\noise_03.jpg"));

	//	PreloadGameObject(pDistortion, Vec3(500.f, 100.f, 50.f), LAYER_TYPE::Default);

	//	Ptr<CMeshData> pMeshData = nullptr;
	//	CGameObject* pObj = nullptr;
	//	pMeshData = CResMgr::GetInst()->LoadFBX(L"fbx\\map\\lava.fbx");
	//	pObj = pMeshData->Instantiate(Vec3(100.f, 100.f, 100.f));
	//	pObj->SetName(L"lava");

	//	//pObj->AddComponent(new CWeaponScript);
	//	pObj->AddComponent(new CGizmo);
	//	pObj->SetIsItem(true);

	//	WeaponMgr::GetInst()->AddWeapon(pObj);

	//	PreloadGameObject(pObj, Vec3(500.f, 100.f, 50.f), LAYER_TYPE::Environment);
	//}
	//{
	//	Ptr<CMeshData> pMeshData = nullptr;
	//	CGameObject* pObj = nullptr;

	//	pMeshData = CResMgr::GetInst()->LoadFBX(L"fbx\\volume_lava.fbx");
	//	pObj = pMeshData->Instantiate(Vec3(10.f, 10.f, 10.f));
	//	//pObj->AddComponent(new CRigidBody(RIGID_BODY_SHAPE_TYPE::MESH));
	//	//pObj->AddComponent(new CCollider3D);

	//	pObj->SetName(L"fbx volume lava");
	//	PreloadGameObject(pObj, Vec3(200.f, 0.f, 200.f), LAYER_TYPE::Environment);
	//}
	//{
	//	Ptr<CMeshData> pMeshData = nullptr;
	//	CGameObject* pObj = nullptr;
	//	pMeshData = CResMgr::GetInst()->LoadFBX(L"fbx\\fire_breath.fbx");
	//	pObj = pMeshData->Instantiate(Vec3(1500.f, 1500.f, 3000.f));
	//	//pObj->AddComponent(new CCollider3D(false));
	//	//pObj->Collider3D()->SetScale(Vec3(1500.f, 1500.f, 10000.f));
	//	//pObj->Collider3D()->SetCenter(true);
	//	pObj->AddComponent(new CGizmo);

	//	pObj->SetName(L"fbx tail beam");
	//	pObj->MeshRender()->GetMaterial(0)->SetTexFlow(true);
	//	pObj->MeshRender()->GetMaterial(0)->SetFlowSpeed(2.f);
	//	pObj->MeshRender()->GetMaterial(0)->SetFlowDir(Vec2(-0.5f, 0.0f));

	//	//vRot.z = -50.f;
	//	//vRot.y = 0.f;
	//	//vRot.x = 0.f;
	//	//pObj->Transform()->SetRelativeRot(vRot);
	//	PreloadGameObject(pObj, Vec3(100.f, 100.f, 100.f), LAYER_TYPE::Environment);
	//}
	//{
	//	Ptr<CMeshData> pMeshData = nullptr;
	//	CGameObject* pObj = nullptr;
	//	pMeshData = CResMgr::GetInst()->LoadFBX(L"fbx\\tail_beam.fbx");
	//	pObj = pMeshData->Instantiate(Vec3(1500.f, 1500.f, 3000.f));
	//	//pObj->AddComponent(new CCollider3D(false));
	//	//pObj->Collider3D()->SetScale(Vec3(1500.f, 1500.f, 10000.f));
	//	//pObj->Collider3D()->SetCenter(true);
	//	pObj->AddComponent(new CGizmo);

	//	pObj->SetName(L"fbx tail beam");
	//	pObj->MeshRender()->GetMaterial(0)->SetTexFlow(true);
	//	pObj->MeshRender()->GetMaterial(0)->SetFlowSpeed(2.f);
	//	pObj->MeshRender()->GetMaterial(0)->SetFlowDir(Vec2(-0.5f, 0.0f));

	//	//vRot.z = -50.f;
	//	//vRot.y = 0.f;
	//	//vRot.x = 0.f;
	//	//pObj->Transform()->SetRelativeRot(vRot);
	//	PreloadGameObject(pObj, Vec3(100.f, 100.f, 100.f), LAYER_TYPE::Environment);
	//}
	{
		Ptr<CMeshData> pMeshData = nullptr;
		CGameObject* pObj = nullptr;

		pMeshData = CResMgr::GetInst()->LoadFBX(L"fbx\\map_statics_subdivision.fbx");
		pObj = pMeshData->Instantiate(Vec3(fDefaultScale, fDefaultScale, fDefaultScale / 2.f));
		pObj->AddComponent(new CRigidBody(RIGID_BODY_SHAPE_TYPE::MESH));
		pObj->AddComponent(new CCollider3D(true, false, COLLIDER_SHAPE_TYPE::MESH));
		//pObj->AddComponent(new CMap);
		//pObj->AddComponent(new CCollider3D);

		pObj->SetName(L"fbx map2");
		PreloadGameObject(pObj, Vec3(200.f, 0.f, 200.f), LAYER_TYPE::Terrain);
	}
	//{
	//	CGameObject* pObj = new CGameObject;

	//	pObj->AddComponent(new CTransform);
	//	pObj->AddComponent(new CMeshRender);
	//	pObj->AddComponent(new CGizmo);
	//	pObj->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"SphereMesh"));
	//	pObj->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Std3D_DeferredMtrl"), 0);
	//	pObj->Transform()->SetRelativeScale(Vec3(100.f, 100.f, 100.f));
	//	//pObj->AddComponent(new CMap);
	//	//pObj->AddComponent(new CCollider3D);

	//	pObj->SetName(L"test 1");
	//	PreloadGameObject(pObj, Vec3(0.f, 500.f, 0.f), LAYER_TYPE::Environment);
	//}
	//{
	//	Ptr<CMeshData> pMeshData = nullptr;
	//	CGameObject* pObj = nullptr;

	//	pMeshData = CResMgr::GetInst()->LoadFBX(L"fbx\\map_test_1.fbx");
	//	pObj = pMeshData->Instantiate(Vec3(10.f, 10.f, 10.f));
	//	pObj->AddComponent(new CRigidBody(RIGID_BODY_SHAPE_TYPE::MESH));
	//	//pObj->AddComponent(new CCollider3D);

	//	pObj->SetName(L"fbx map3");
	//	PreloadGameObject(pObj, Vec3(200.f, 0.f, 200.f), LAYER_TYPE::Environment);
	//}

	//for(int i =0 ; i < 2 ; ++i)
	//{
	//	Ptr<CMeshData> pMeshData = nullptr;
	//	CGameObject* pObj = nullptr;

	//	pMeshData = CResMgr::GetInst()->LoadFBX(L"fbx\\nomad.fbx");
	//	pObj = pMeshData->Instantiate(Vec3(fDefaultScale, fDefaultScale, fDefaultScale));
	//	wstring str = L"fbx nomad" + std::to_wstring(i);
	//	pObj->SetName(str);

	//	pObj->AddComponent(new CEnemyScript(CEnemyScript::ENEMY_TYPE::NOMAD));
	//	pObj->AddComponent(new CRigidBody(RIGID_BODY_SHAPE_TYPE::BOX, RIGID_BODY_TYPE::DYNAMIC));
	//	pObj->AddComponent(new CCollider3D);
	//	//pObj->AddComponent(new CPathFindScript);
	//	pObj->AddComponent(new CGizmo);

	//	pObj->RigidBody()->SetCreature(true);

	//	PreloadGameObject(pObj, Vec3(500.f, 100.f, 1000.f + (i * 150.f)), LAYER_TYPE::Enemy);

	//	{
	//		Ptr<CMeshData> pMeshData = nullptr;
	//		CGameObject* pGun = nullptr;
	//		pMeshData = CResMgr::GetInst()->LoadFBX(L"fbx\\dahl.fbx");
	//		pGun = pMeshData->Instantiate(Vec3(fDefaultScale, fDefaultScale, fDefaultScale));
	//		pGun->SetName(L"enemy_gun");

	//		pGun->AddComponent(new CWeaponScript);
	//		pGun->AddComponent(new CGizmo);
	//		pGun->SetIsItem(true);
	//		pObj->AddWeapon(pGun);

	//		PreloadGameObject(pGun, Vec3(500.f, 100.f, 50.f), LAYER_TYPE::Item);
	//	}
	//}
	//{
	//	CGameObject* pObj = new CGameObject;
	//	pObj->SetName(L"Move Tester");

	//	pObj->AddComponent(new CTransform);
	//	pObj->AddComponent(new CMeshRender);
	//	pObj->Transform()->SetRelativeScale(Vec3(50.f, 50.f, 50.f));

	//	pObj->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"SphereMesh"));
	//	pObj->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Std3D_DeferredMtrl"), 0);


	//	//pObj->AddComponent(new CEnemyScript);
	//	pObj->AddComponent(new CPathFindScript);
	//	pObj->AddComponent(new CGizmo);

	//	//pObj->RigidBody()->SetCreature(true);

	//	PreloadGameObject(pObj, Vec3(500.f, 100.f, 1000.f), LAYER_TYPE::Enemy);
	//}

	{
		CGameObject* pObj = new CGameObject;
		pObj->SetName(L"Fire");

		pObj->AddComponent(new CTransform);
		pObj->AddComponent(new CMeshRender);

		pObj->Transform()->SetBilboard(true);
		pObj->Transform()->SetRelativeScale(Vec3(100.f, 100.f, 1.f));
		pObj->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
		Ptr<CMaterial> fireMtrl = new CMaterial(true);

		CResMgr::GetInst()->AddRes(L"FireMtrl", fireMtrl);
		fireMtrl->SetShader(CResMgr::GetInst()->FindRes<CGraphicsShader>(L"Std3DShader"));
		fireMtrl->SetEmissiveCoeff(20.f);
		fireMtrl->SetPerlinNoise(true);
		pObj->MeshRender()->SetMaterial(fireMtrl, 0);

		pObj->AddComponent(new CGizmo);

		PreloadGameObject(pObj, Vec3(500.f, 100.f, 1000.f), LAYER_TYPE::Default);
	}
	{
		CGameObject* pObj = new CGameObject;
		pObj->SetName(L"Fire Light");

		pObj->AddComponent(new CTransform);
		pObj->AddComponent(new CLight3D);

		//pObj->Transform()->SetRelativeScale(Vec3(100.f, 100.f, 1.f));

		pObj->Light3D()->SetLightType(LIGHT_TYPE::POINT);
		pObj->Light3D()->SetRadius(200.f);
		pObj->Light3D()->SetLightColor(Vec3(1.f, 0.f, 0.f));
		pObj->Light3D()->SetLightAmbient(Vec3(1.0f, 0.0, 0.0));

		//pObj->AddComponent(new CGizmo);

		PreloadGameObject(pObj, Vec3(100.f, 100.f, 100.f), LAYER_TYPE::Default);
	}
	{
		CGameObject* pObj = new CGameObject;
		pObj->SetName(L"Fire Particle");

		pObj->AddComponent(new CTransform);
		pObj->AddComponent(new CParticleSystem);
		
		pObj->AddComponent(new CGizmo);
		
		pObj->Transform()->SetRelativeScale(Vec3(500.f, 500.f, 1.f));

		pObj->ParticleSystem();
		//pObj->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
		//Ptr<CMaterial> fireMtrl = new CMaterial(true);
		//CResMgr::GetInst()->AddRes(L"FireMtrl", fireMtrl);
		//fireMtrl->SetShader(CResMgr::GetInst()->FindRes<CGraphicsShader>(L"Std3DShader"));

		//pObj->MeshRender()->SetMaterial(fireMtrl, 0);


		PreloadGameObject(pObj, Vec3(500.f, 100.f, 1000.f), LAYER_TYPE::Enemy);
	}

	//}
	//{
	//	Ptr<CMeshData> pMeshData = nullptr;
	//	CGameObject* pObj = nullptr;
	//	
	//	pMeshData = CResMgr::GetInst()->LoadFBX(L"fbx\\fire_breath.fbx");
	//	pObj = pMeshData->Instantiate(Vec3(10000.f, 10000.f, 10000.f));
	//	pObj->AddComponent(new CCollider3D(false));
	//	pObj->AddComponent(new CGizmo);
	//	pObj->Collider3D()->SetScale(Vec3(1500.f, 1500.f, 13000.f));
	//	//pObj->Collider3D()->SetCenter(true);

	//	pObj->SetName(L"fbx fire breath");
	//	pObj->MeshRender()->GetMaterial(0)->SetTexFlow(true);
	//	pObj->MeshRender()->GetMaterial(0)->SetFlowSpeed(2.f);
	//	pObj->MeshRender()->GetMaterial(0)->SetFlowDir(Vec2(-0.5f, 0.0f));
	//	PreloadGameObject(pObj, Vec3(200.f, 100.f, 15000.f), LAYER_TYPE::Environment);
	//}
	{
		Ptr<CMeshData> pMeshData = nullptr;
		CGameObject* pObj = nullptr;

		pMeshData = CResMgr::GetInst()->LoadFBX(L"fbx\\warrior_2.fbx");
		pObj = pMeshData->Instantiate(Vec3(100.f, 100.f, 100.f));
		pObj->SetName(L"fbx warrior");
		pObj->AddComponent(new CCollider3D(false));
		pObj->SetIsWarrior(true);
		pObj->AddComponent(new CWarriorScript);

		PreloadGameObject(pObj, Vec3(10000.f, 100.f, 500.f), LAYER_TYPE::Enemy);
	}
	//{
	//	
	//	Ptr<CMeshData> pMeshData = nullptr;
	//	CGameObject* pAxe = nullptr;
	//	pMeshData = CResMgr::GetInst()->LoadFBX(L"fbx\\buzz_axe.fbx");
	//	pAxe = pMeshData->Instantiate(Vec3(fDefaultScale, fDefaultScale, fDefaultScale));
	//	pAxe->SetName(L"enemy_axe");
	//	//pAxe->Transform()->SetRelativePosOffset()

	//	pAxe->AddComponent(new CCollider3D(false));
	//	pAxe->Collider3D()->SetRaycastable(false);
	//	pAxe->Collider3D()->SetScale(Vec3(fDefaultScale / 2.f, fDefaultScale * 2.f, fDefaultScale));
	//	pAxe->AddComponent(new CWeaponScript);
	//	pAxe->AddComponent(new CAttackNormalScript);
	//	pAxe->GetScript<CAttackNormalScript>()->SetManual(true);
	//	pAxe->AddComponent(new CGizmo);
	//	pAxe->SetIsItem(true);
	//	

	//	PreloadGameObject(pAxe, Vec3(0.f, 0.f, 0.f), LAYER_TYPE::NoRaycastingCollider);
	//	
	//	CGameObject* pObj = nullptr;

	//	pMeshData = CResMgr::GetInst()->LoadFBX(L"fbx\\psycho.fbx");
	//	pObj = pMeshData->Instantiate(Vec3(100.f, 100.f, 100.f));
	//	pObj->SetName(L"psycho");
	//	pObj->AddComponent(new CEnemyScript(CEnemyScript::ENEMY_TYPE::PSYCHO));
	//	pObj->AddComponent(new CRigidBody(RIGID_BODY_SHAPE_TYPE::BOX, RIGID_BODY_TYPE::DYNAMIC));
	//	pObj->AddComponent(new CCollider3D);
	//	pObj->AddComponent(new CGizmo);
	//	pObj->RigidBody()->SetCreature(true);
	//	pObj->AddWeapon(pAxe, true);

	//	PreloadGameObject(pObj, Vec3(400.f, 100.f, 500.f), LAYER_TYPE::Enemy);

	//}
	//{
	//	Ptr<CMeshData> pMeshData = nullptr;
	//	CGameObject* pObj = nullptr;

	//	pMeshData = CResMgr::GetInst()->LoadFBX(L"fbx\\robot.fbx");
	//	pObj = pMeshData->Instantiate(Vec3(100.f, 100.f, 100.f));
	//	pObj->SetName(L"robot");
	//	pObj->AddComponent(new CEnemyScript(CEnemyScript::ENEMY_TYPE::GUN_LOADER));
	//	pObj->AddComponent(new CRigidBody(RIGID_BODY_SHAPE_TYPE::BOX, RIGID_BODY_TYPE::DYNAMIC));
	//	pObj->AddComponent(new CCollider3D);
	//	pObj->AddComponent(new CGizmo);

	//	PreloadGameObject(pObj, Vec3(100.f, 100.f, 500.f), LAYER_TYPE::Enemy);
	//	{
	//		Ptr<CMeshData> pMeshData = nullptr;
	//		CGameObject* pGun = nullptr;
	//		pMeshData = CResMgr::GetInst()->LoadFBX(L"fbx\\dahl.fbx");
	//		pGun = pMeshData->Instantiate(Vec3(fDefaultScale, fDefaultScale, fDefaultScale));
	//		pGun->SetName(L"enemy_gun");

	//		pGun->AddComponent(new CWeaponScript);
	//		pGun->AddComponent(new CGizmo);
	//		pGun->SetIsItem(true);
	//		pObj->AddWeapon(pGun);

	//		PreloadGameObject(pGun, Vec3(500.f, 100.f, 50.f), LAYER_TYPE::Item);
	//	}
	//}

	pCurLevel = CLevelMgr::GetInst()->CreateLevel(L"main menu level");
	CLevelMgr::GetInst()->ChangeLevel(pCurLevel);
	pCurLevel->ChangeState(LEVEL_STATE::STOP);

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
		pMainCam->Camera()->SetLayerMask((int)LAYER_TYPE::ViewPortUI, false);
		PreloadGameObject(pMainCam, Vec3(0.f, 0.f, 0.f), LAYER_TYPE::Camera);
	}


	pUICam = new CGameObject;
	pUICam->SetName(L"UICamera");

	pUICam->AddComponent(new CTransform);
	pUICam->AddComponent(new CCamera);

	pUICam->Camera()->SetFarZ(1000000.f);
	pUICam->Camera()->SetProjType(PROJ_TYPE::ORTHOGRAPHIC);
	pUICam->Camera()->SetCameraIndex(1);
	pUICam->Camera()->SetLayerMaskAll(false);
	pUICam->Camera()->SetLayerMask((int)LAYER_TYPE::ViewPortUI, true);
	PreloadGameObject(pUICam, Vec3(0.f, 0.f, 0.f), LAYER_TYPE::Camera);

	{
		CGameObject* pSkyBox = new CGameObject;
		pSkyBox->SetName(L"SkyBox");

		pSkyBox->AddComponent(new CTransform);
		pSkyBox->AddComponent(new CSkyBox);

		pSkyBox->Transform()->SetRelativeScale(Vec3(100.f, 100.f, 100));
		pSkyBox->SkyBox()->SetSkyBoxType(SKYBOX_TYPE::SPHERE);
		pSkyBox->SkyBox()->SetSkyBoxTexture(CResMgr::GetInst()->FindRes<CTexture>(L"texture\\skybox\\Sky01.png"));

		//SpawnGameObject(pSkyBox, Vec3(0.f, 0.f, 0.f), 0);
		PreloadGameObject(pSkyBox, Vec3(0.f, 0.f, 0.f), LAYER_TYPE::Default);
	}

	//btn_hover.png
	{
		CUI* pGameStart = new CUI();
		pGameStart->SetName(L"UI Game Start");
		pGameStart->AddComponent(new CTransform);
		pGameStart->AddComponent(new CMeshRender);
		pGameStart->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
		pGameStart->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"AdjustUI2DShaderMtrl"), 0);
		pGameStart->Transform()->SetRelativeScale(Vec3(100.f, 100.f, 1.f));
		pGameStart->SetUseHover(true);
		pGameStart->SetOutline(true);
		pGameStart->SetText(L"Game Start");
		pGameStart->SetClickFunc() = std::make_shared<std::function<void()>>([=]()
			{
				ChangeCurLevel(CLevelMgr::GetInst()->GetLevel(L"main level"));
			});
		pGameStart->SetTextHoverColor(Vec4(255.f / 255.f, 208.f / 255.f, 19.f / 255.f, 1.f));
		PreloadGameObject(pGameStart, Vec3(-440.f, 250.f, 0.f), LAYER_TYPE::ViewPortUI);
	}
	{
		CUI* pGameEnd = new CUI();
		pGameEnd->SetName(L"UI Game End");
		pGameEnd->AddComponent(new CTransform);
		pGameEnd->AddComponent(new CMeshRender);
		pGameEnd->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
		pGameEnd->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"AdjustUI2DShaderMtrl"), 0);
		pGameEnd->Transform()->SetRelativeScale(Vec3(100.f, 100.f, 1.f));
		pGameEnd->SetUseHover(true);
		pGameEnd->SetOutline(true);
		pGameEnd->SetText(L"Exit");
		pGameEnd->SetTextHoverColor(Vec4(255.f / 255.f, 208.f / 255.f, 19.f / 255.f, 1.f));
		pGameEnd->SetClickFunc() = std::make_shared<std::function<void()>>([=]()
			{
				PostQuitMessage(0);
			});
		PreloadGameObject(pGameEnd, Vec3(-495.f, 190.f, 0.f), LAYER_TYPE::ViewPortUI);
	}
	{
		CGameObject* pPlane = new CGameObject;
		pPlane->SetName(L"Plane");
		pPlane->AddComponent(new CTransform);
		pPlane->AddComponent(new CMeshRender);
		pPlane->AddComponent(new CRigidBody(RIGID_BODY_SHAPE_TYPE::BOX));
		pPlane->AddComponent(new CCollider3D);

		pPlane->Transform()->SetRelativeScale(Vec3(100000.f, 100000.f, 1.f));
		pPlane->Transform()->SetRelativeRot(Vec3(XM_PI / 2.f, 0.f, 0.f));

		pPlane->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
		pPlane->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Std3D_DeferredMtrl"), 0);

		pPlane->MeshRender()->GetMaterial(0)->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"texture\\tile\\TILE_01.tga"));
		//pPlane->MeshRender()->GetMaterial(0)->SetTexParam(TEX_1, CResMgr::GetInst()->FindRes<CTexture>(L"texture\\tile\\TILE_01_N.tga"));
		PreloadGameObject(pPlane, Vec3(0.f, -10.f, 0.f), LAYER_TYPE::Environment);
	}
	{
		CGameObject* pObj = new CGameObject;
		pObj->SetName(L"Fire Light");

		pObj->AddComponent(new CTransform);
		pObj->AddComponent(new CLight3D);

		//pObj->Transform()->SetRelativeScale(Vec3(100.f, 100.f, 1.f));

		pObj->Light3D()->SetLightType(LIGHT_TYPE::POINT);
		pObj->Light3D()->SetRadius(2000.f);
		pObj->Light3D()->SetLightColor(Vec3(1.f, 1.f, 0.f));
		pObj->Light3D()->SetLightAmbient(Vec3(1.0f, 1.0, 0.0));

		//pObj->AddComponent(new CGizmo);

		SpawnGameObject(pObj, Vec3(500.f, 100.f, 1000.f), LAYER_TYPE::Default);
	}

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

	CollisionMgr::GetInst()->SetLayerIntersect(LAYER_TYPE::Enemy, LAYER_TYPE::Player, true);
	CollisionMgr::GetInst()->SetLayerIntersect(LAYER_TYPE::Enemy, LAYER_TYPE::PlayerBullet, true);
	//CollisionMgr::GetInst()->SetLayerIntersect(LAYER_TYPE::Enemy, LAYER_TYPE::Environment, true);
	//CollisionMgr::GetInst()->SetLayerIntersect(L"Default", L"Bullet", true);
	CollisionMgr::GetInst()->SetLayerIntersect(LAYER_TYPE::Player, LAYER_TYPE::EnemyBullet, true);
	CollisionMgr::GetInst()->SetLayerIntersect(LAYER_TYPE::Player, LAYER_TYPE::NoRaycastingCollider, true);
	CollisionMgr::GetInst()->SetLayerIntersect(LAYER_TYPE::Player, LAYER_TYPE::Environment, true);
	CollisionMgr::GetInst()->SetLayerIntersect(LAYER_TYPE::Player, LAYER_TYPE::Terrain, true);
	//CollisionMgr::GetInst()->SetLayerIntersect(LAYER_TYPE::Player,LAYER_TYPE::Environment, true);

	//CollisionMgr::GetInst()->SetLayerIntersect(LAYER_TYPE::EnemyBullet, LAYER_TYPE::Environment, true);
	//CollisionMgr::GetInst()->SetLayerIntersect(LAYER_TYPE::PlayerBullet, LAYER_TYPE::Environment, true);
}
