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
#include <Engine\SoundMgr.h>

#include <Script\CPlayerScript.h>
#include <Script\CEnemyScript.h>
#include <Script\CCameraMoveScript.h>
#include <Script\CWeaponScript.h>
#include <Script\CConstructorScript.h>
#include <Script\CWarriorScript.h>
#include <Script\CAttackBurnScript.h>
#include <Script\CAttackNormalScript.h>
#include <Script\CPathFindScript.h>
#include <Script\CMoveScript.h>

void CreateLoading()
{
	CLevel* pCurLevel = CLevelMgr::GetInst()->CreateLevel(L"loading");
	CLevelMgr::GetInst()->ChangeLevel(pCurLevel);
	pCurLevel->ChangeState(LEVEL_STATE::STOP);

	Vec2 vResol = CEngine::GetInst()->GetWindowResolution();

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
		pMainCam->Camera()->SetLayerMask((int)LAYER_TYPE::Camera, false);

		PreloadGameObject(pMainCam, Vec3(0.f,0.f,-1100.f), LAYER_TYPE::Camera);
	}
	{
		CGameObject* pUICam = new CGameObject;
		pUICam->SetName(L"UICamera");
		pUICam->AddComponent(new CTransform);
		pUICam->AddComponent(new CCamera);
		pUICam->Camera()->SetFarZ(1000000.f);
		pUICam->Camera()->SetProjType(PROJ_TYPE::ORTHOGRAPHIC);
		pUICam->Camera()->SetCameraIndex(3);
		pUICam->Camera()->SetLayerMaskAll(false);
		pUICam->Camera()->SetLayerMask((int)LAYER_TYPE::ViewPortUI, true);
		PreloadGameObject(pUICam, Vec3(0.f, 0.f, 0.f), LAYER_TYPE::Camera);
	}
	{
		CGameObject* pSunLight = new CGameObject;
		pSunLight->SetName(L"Sun Light");
		pSunLight->AddComponent(new CTransform);
		pSunLight->AddComponent(new CLight3D);
		pSunLight->AddComponent(new CGizmo);

		pSunLight->Transform()->SetRelativeRot(Vec3(150.f * DegToRad(), 90.f * DegToRad(), 0.f));
		pSunLight->Light3D()->SetRadius(500.f);
		pSunLight->Light3D()->SetShadow(true);
		pSunLight->Light3D()->SetLightDepthCoeff(0.003f);
		pSunLight->Light3D()->SetFloatConstant(0, 1000.f);
		pSunLight->Light3D()->SetFloatConstant(1, 8.0f);
		pSunLight->Light3D()->SetLightType(LIGHT_TYPE::DIRECTIONAL);
		pSunLight->Light3D()->SetLightColor(Vec3(1.f, 1.f, 1.f));
		pSunLight->Light3D()->SetLightAmbient(Vec3(1.f, 1.f, 1.f));

		PreloadGameObject(pSunLight, Vec3(-2000.f, 60000.f, -2000.f), LAYER_TYPE::Default);
	}

	{
		CUI* pGameStart = new CUI();
		pGameStart->SetName(L"UI Loading");
		pGameStart->AddComponent(new CTransform);
		pGameStart->Transform()->SetRelativeScale(Vec3(100.f, 100.f, 1.f));
		pGameStart->SetText(L"Loading");
		PreloadGameObject(pGameStart, Vec3(0.f, 250.f, 0.f), LAYER_TYPE::ViewPortUI);
	}
	{
		Ptr<CMaterial> pMtrl = new CMaterial(true);
		pMtrl->SetShader(CResMgr::GetInst()->FindRes<CGraphicsShader>(L"Std3D_DeferredShader").Get());
		CResMgr::GetInst()->AddRes(L"UILoadingBack", pMtrl);

		CGameObject* LoadingBack = new CGameObject();
		LoadingBack->SetName(L"UI Loading Back");
		LoadingBack->AddComponent(new CTransform);
		LoadingBack->AddComponent(new CMeshRender);
		LoadingBack->Transform()->SetRelativeScale(Vec3(vResol.x, vResol.y, 1.f));

		LoadingBack->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
		LoadingBack->MeshRender()->SetMaterial(pMtrl, 0);
		LoadingBack->MeshRender()->GetMaterial(0)->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"texture\\UI\\loading_opp.png"));
		PreloadGameObject(LoadingBack, Vec3(0.f, 0.f, 0.f), LAYER_TYPE::Default);
	}
	{
		Ptr<CMeshData> pMeshData = nullptr;
		CGameObject* pObj = nullptr;
		pMeshData = CResMgr::GetInst()->LoadFBX(L"fbx\\smg_dahl.fbx");
		pObj = pMeshData->Instantiate(Vec3(100.f, 100.f, 100.f));
		pObj->SetName(L"smg_loading");
		pObj->AddComponent(new CWeaponScript);
		pObj->AddComponent(new CMoveScript);
		pObj->AddComponent(new CGizmo);
		pObj->GetScript<CWeaponScript>()->SetStopFollow(true);

		pObj->SetObjectState(CGameObject::OBJECT_STATE::INVISIBLE);
		PreloadGameObject(pObj, Vec3(150.f, -50.f, -730.f), LAYER_TYPE::Default);
	}
	{
		Ptr<CMeshData> pMeshData = nullptr;
		CGameObject* pObj = nullptr;
		pMeshData = CResMgr::GetInst()->LoadFBX(L"fbx\\sniper_dahl.fbx");
		pObj = pMeshData->Instantiate(Vec3(100.f, 100.f, 100.f));
		pObj->SetName(L"sniper_loading");
		pObj->AddComponent(new CWeaponScript);
		pObj->AddComponent(new CMoveScript);
		pObj->AddComponent(new CGizmo);
		pObj->GetScript<CWeaponScript>()->SetStopFollow(true);

		pObj->SetObjectState(CGameObject::OBJECT_STATE::INVISIBLE);
		PreloadGameObject(pObj, Vec3(150.f, -50.f, -730.f), LAYER_TYPE::Default);

	}
}
void CreatePlayer(float fDefaultScale)
{
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
}

void CreateLevels()
{
	float fDefaultScale = 100.f;

	/*****************************************************************
	 **************************** LEVEL 2 ****************************
	 *****************************************************************/

	CLevel* pCurLevel = CLevelMgr::GetInst()->CreateLevel(L"main level 2");
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
		pMainCam->Camera()->SetLayerMask((int)LAYER_TYPE::Camera, false);
		PreloadGameObject(pMainCam, Vec3(0.f, 0.f, 0.f), LAYER_TYPE::Camera);
	}
	{
		CGameObject* pMapCam = new CGameObject;
		pMapCam->SetName(L"MapCamera");

		pMapCam->AddComponent(new CTransform);
		pMapCam->AddComponent(new CCamera);
		pMapCam->AddComponent(new CCameraMoveScript);

		pMapCam->Camera()->SetFarZ(1000000.f);
		pMapCam->Camera()->SetProjType(PROJ_TYPE::ORTHOGRAPHIC);
		pMapCam->Camera()->SetOrthoWidth(1000.f);
		pMapCam->Camera()->SetOrthoHeight(1000.f);
		pMapCam->Camera()->SetScale(0.15f);
		//pMapCam->Camera()->SetFOV(2.1f);
		pMapCam->Camera()->SetCameraIndex(1);
		pMapCam->Camera()->SetLayerMaskAll(false);
		pMapCam->Camera()->SetLayerMask((int)LAYER_TYPE::Terrain, true);
		//pMapCam->Camera()->SetLayerMask((int)LAYER_TYPE::Enemy, true);
		//pMapCam->Camera()->SetLayerMask((int)LAYER_TYPE::Environment, true);

		//pMapCam->Transform()->SetRelativeRot(Vec3(0.f, ));
		PreloadGameObject(pMapCam, Vec3(0.f, 0.f, 0.f), LAYER_TYPE::Camera);
	}
	{
		CGameObject* pScopeCam = new CGameObject;
		pScopeCam->SetName(L"ScopeCamera");

		pScopeCam->AddComponent(new CTransform);
		pScopeCam->AddComponent(new CCamera);
		pScopeCam->AddComponent(new CCameraMoveScript);

		pScopeCam->Camera()->SetFarZ(1000000.f);
		pScopeCam->Camera()->SetProjType(PROJ_TYPE::PERSPECTIVE);
		pScopeCam->Camera()->SetCameraIndex(2);
		pScopeCam->Camera()->SetLayerMaskAll(true);
		pScopeCam->Camera()->SetLayerMask((int)LAYER_TYPE::ViewPortUI, false);
		pScopeCam->Camera()->SetLayerMask((int)LAYER_TYPE::Camera, false);
		PreloadGameObject(pScopeCam, Vec3(0.f, 0.f, 0.f), LAYER_TYPE::Camera);
	}

	{
		CGameObject* pUICam = new CGameObject;
		pUICam->SetName(L"UICamera");

		pUICam->AddComponent(new CTransform);
		pUICam->AddComponent(new CCamera);

		pUICam->Camera()->SetFarZ(1000000.f);
		pUICam->Camera()->SetProjType(PROJ_TYPE::ORTHOGRAPHIC);
		pUICam->Camera()->SetCameraIndex(3);
		pUICam->Camera()->SetLayerMaskAll(false);
		pUICam->Camera()->SetLayerMask((int)LAYER_TYPE::ViewPortUI, true);
		PreloadGameObject(pUICam, Vec3(0.f, 0.f, 0.f), LAYER_TYPE::Camera);
	}

	CGameObject* pSunLight = new CGameObject;
	pSunLight->SetName(L"Sun Light");
	pSunLight->AddComponent(new CTransform);
	pSunLight->AddComponent(new CLight3D);
	pSunLight->AddComponent(new CGizmo);

	pSunLight->Transform()->SetRelativeRot(Vec3(XM_PI / 4.f, XM_PI / 4.f, 0.f));
	pSunLight->Light3D()->SetRadius(500.f);
	pSunLight->Light3D()->SetShadow(true);
	pSunLight->Light3D()->SetLightDepthCoeff(0.003f);
	pSunLight->Light3D()->SetFloatConstant(0, 1000.f);
	pSunLight->Light3D()->SetFloatConstant(1, 8.0f);
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
	PreloadGameObject(pPlane, Vec3(0.f, -10.f, 45000.f), LAYER_TYPE::Environment);

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

	//{
	//	Ptr<CMeshData> pMeshData = nullptr;
	//	CGameObject* pSkyBox = nullptr;
	//	pMeshData = CResMgr::GetInst()->LoadFBX(L"fbx\\warrior_skybox.fbx");
	//	pSkyBox = pMeshData->Instantiate(Vec3(100.f, 100.f, 100.f), true);
	//	pSkyBox->SetName(L"SkyBox");

	//	PreloadGameObject(pSkyBox, Vec3(0.f, 0.f, 0.f), LAYER_TYPE::Default);
	//}

	{
		CGameObject* pSkyBox = new CGameObject;
		pSkyBox->SetName(L"SkyBox");

		pSkyBox->AddComponent(new CTransform);
		pSkyBox->AddComponent(new CSkyBox);

		pSkyBox->Transform()->SetRelativeScale(Vec3(100.f, 100.f, 100));
		pSkyBox->SkyBox()->SetSkyBoxType(SKYBOX_TYPE::HEMI_SPHERE);
		pSkyBox->SkyBox()->SetSkyBoxTexture(CResMgr::GetInst()->FindRes<CTexture>(L"texture\\skybox\\Zone3BossSky_Dif.png"));

		PreloadGameObject(pSkyBox, Vec3(0.f, 0.f, 0.f), LAYER_TYPE::Default);
	}

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
		//CGameObject* pDistortion = new CGameObject;
		//pDistortion->SetName(L"Lava Distortion");
		//pDistortion->AddComponent(new CTransform);
		//pDistortion->AddComponent(new CMeshRender);
		//pDistortion->Transform()->SetRelativeScale(Vec3(100.f, 100.f, 100.f));

		//pDistortion->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"mesh\\lava.mesh"));
		//pDistortion->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"DistortionMtrl"), 0);
		//pDistortion->MeshRender()->GetMaterial(0)->SetTexParam(TEX_1, CResMgr::GetInst()->FindRes<CTexture>(L"texture\\noise\\noise_03.jpg"));

		//PreloadGameObject(pDistortion, Vec3(500.f, 100.f, 50.f), LAYER_TYPE::Default);

		Ptr<CMeshData> pMeshData = nullptr;
		CGameObject* pObj = nullptr;
		pMeshData = CResMgr::GetInst()->LoadFBX(L"fbx\\map\\lava.fbx");
		pObj = pMeshData->Instantiate(Vec3(206.f, 100.f, 326.f));
		pObj->SetName(L"lava");
		pObj->Transform()->SetRelativeRot(Vec3(0.f, -90.f * DegToRad(), 0.f));
		//pObj->AddComponent(new CWeaponScript);
		pObj->AddComponent(new CGizmo);

		PreloadGameObject(pObj, Vec3(500.f, 245.f, 25000.f), LAYER_TYPE::Environment);
	}
	{
		Ptr<CMeshData> pMeshData = nullptr;
		CGameObject* pObj = nullptr;

		pMeshData = CResMgr::GetInst()->LoadFBX(L"fbx\\volume_lava.fbx");
		pObj = pMeshData->Instantiate(Vec3(221.f, 91.f, 81.f));
		pObj->Transform()->SetRelativeRot(Vec3(0.f, 180.f * DegToRad(), 50.f * DegToRad()));

		pObj->SetName(L"fbx volume lava1");
		PreloadGameObject(pObj, Vec3(-6617.f, 1077.f, 200.f), LAYER_TYPE::Environment);
	}
	{
		Ptr<CMeshData> pMeshData = nullptr;
		CGameObject* pObj = nullptr;

		pMeshData = CResMgr::GetInst()->LoadFBX(L"fbx\\volume_lava.fbx");
		pObj = pMeshData->Instantiate(Vec3(222.f, 71.f, 790.f));
		pObj->Transform()->SetRelativeRot(Vec3(0.f, -90.f * DegToRad(), 50.f * DegToRad()));

		pObj->SetName(L"fbx volume lava2");
		PreloadGameObject(pObj, Vec3(824.f, -5560.f, -7925.f), LAYER_TYPE::Environment);
	}

	{
		Ptr<CMeshData> pMeshData = nullptr;
		CGameObject* pObj = nullptr;

		pMeshData = CResMgr::GetInst()->LoadFBX(L"fbx\\warrior_level_map.fbx");
		pObj = pMeshData->Instantiate(Vec3(fDefaultScale, fDefaultScale, fDefaultScale));
		pObj->AddComponent(new CRigidBody(RIGID_BODY_SHAPE_TYPE::MESH));
		pObj->AddComponent(new CCollider3D(true, false, COLLIDER_SHAPE_TYPE::MESH));

		pObj->SetName(L"fbx level2 map");
		PreloadGameObject(pObj, Vec3(200.f, 10.f, 200.f), LAYER_TYPE::Terrain);
	}

	{
		CGameObject* pObj = new CGameObject;
		pObj->SetName(L"Fire1");

		pObj->AddComponent(new CTransform);
		pObj->AddComponent(new CMeshRender);
		pObj->AddComponent(new CLight3D);

		pObj->Light3D()->SetLightType(LIGHT_TYPE::POINT);
		pObj->Light3D()->SetRadius(355.f);
		pObj->Light3D()->SetLightColor(Vec3(1.f, 0.f, 0.f));
		pObj->Light3D()->SetLightAmbient(Vec3(1.0f, 0.0, 0.0));

		pObj->Transform()->SetBilboard(true);
		pObj->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
		Ptr<CMaterial> fireMtrl = new CMaterial(true);

		CResMgr::GetInst()->AddRes(L"FireMtrl", fireMtrl);
		fireMtrl->SetShader(CResMgr::GetInst()->FindRes<CGraphicsShader>(L"Std3DShader"));
		fireMtrl->SetEmissiveCoeff(20.f);
		fireMtrl->SetPerlinNoise(true);
		pObj->MeshRender()->SetMaterial(fireMtrl, 0);

		pObj->AddComponent(new CGizmo);

		PreloadGameObject(pObj, Vec3(-2982.f, 1703.f, 17946.f), LAYER_TYPE::Default);
	}
	{
		CGameObject* pObj = new CGameObject;
		pObj->SetName(L"Fire2");

		pObj->AddComponent(new CTransform);
		pObj->AddComponent(new CMeshRender);
		pObj->AddComponent(new CLight3D);

		pObj->Light3D()->SetLightType(LIGHT_TYPE::POINT);
		pObj->Light3D()->SetRadius(355.f);
		pObj->Light3D()->SetLightColor(Vec3(1.f, 0.f, 0.f));
		pObj->Light3D()->SetLightAmbient(Vec3(1.0f, 0.0, 0.0));

		pObj->Transform()->SetBilboard(true);
		//pObj->Transform()->SetRelativeScale(Vec3(389.f, 717.f, 1.f));
		pObj->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
		Ptr<CMaterial> fireMtrl = new CMaterial(true);

		CResMgr::GetInst()->AddRes(L"FireMtrl", fireMtrl);
		fireMtrl->SetShader(CResMgr::GetInst()->FindRes<CGraphicsShader>(L"Std3DShader"));
		fireMtrl->SetEmissiveCoeff(20.f);
		fireMtrl->SetPerlinNoise(true);
		pObj->MeshRender()->SetMaterial(fireMtrl, 0);

		pObj->AddComponent(new CGizmo);

		PreloadGameObject(pObj, Vec3(-2982.f, 1703.f, 16917.000), LAYER_TYPE::Default);
	}

	{
		CGameObject* pObj = new CGameObject;
		pObj->SetName(L"Fire3");

		pObj->AddComponent(new CTransform);
		pObj->AddComponent(new CMeshRender);
		pObj->AddComponent(new CLight3D);

		pObj->Light3D()->SetLightType(LIGHT_TYPE::POINT);
		pObj->Light3D()->SetRadius(805.f);
		pObj->Light3D()->SetLightColor(Vec3(1.f, 0.f, 0.f));
		pObj->Light3D()->SetLightAmbient(Vec3(1.0f, 0.0, 0.0));

		pObj->Transform()->SetBilboard(true);
		pObj->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
		Ptr<CMaterial> fireMtrl = new CMaterial(true);

		CResMgr::GetInst()->AddRes(L"FireMtrl", fireMtrl);
		fireMtrl->SetShader(CResMgr::GetInst()->FindRes<CGraphicsShader>(L"Std3DShader"));
		fireMtrl->SetEmissiveCoeff(20.f);
		fireMtrl->SetPerlinNoise(true);
		pObj->MeshRender()->SetMaterial(fireMtrl, 0);

		pObj->AddComponent(new CGizmo);

		PreloadGameObject(pObj, Vec3(-3346.000, 1981.000, 14060.000), LAYER_TYPE::Default);
	}
	{
		CGameObject* pObj = new CGameObject;
		pObj->SetName(L"Lava Ambient Light");

		pObj->AddComponent(new CTransform);
		pObj->AddComponent(new CLight3D);

		//pObj->Transform()->SetRelativeScale(Vec3(100.f, 100.f, 1.f));

		pObj->Light3D()->SetLightType(LIGHT_TYPE::POINT);
		pObj->Light3D()->SetRadius(40000.f);
		pObj->Light3D()->SetLightColor(Vec3(3.0f, 1.5f, 0.f));
		pObj->Light3D()->SetLightAmbient(Vec3(2.0f, 0.9, 0.0));

		//pObj->AddComponent(new CGizmo);

		PreloadGameObject(pObj, Vec3(100.f, -32000.f, 20000.f), LAYER_TYPE::Default);
	}


	{
		CreatePlayer(fDefaultScale);
	}

	{
		Ptr<CMeshData> pMeshData = nullptr;
		CGameObject* pObj = nullptr;

		pMeshData = CResMgr::GetInst()->LoadFBX(L"fbx\\warrior_2.fbx");
		pObj = pMeshData->Instantiate(Vec3(100.f, 100.f, 100.f));
		pObj->Transform()->SetRelativeRot(Vec3(0.f, -90 * DegToRad(), 0.f));
		pObj->SetName(L"fbx warrior");

		pObj->AddComponent(new CCollider3D(false));
		pObj->AddComponent(new CWarriorScript);
		pObj->SetIsWarrior(true);

		PreloadGameObject(pObj, Vec3(-1854.156, -10154.404, 4439.329), LAYER_TYPE::Enemy);
	}

	for (int i = 0; i < 2; ++i)
	{
		Ptr<CMeshData> pMeshData = nullptr;
		CGameObject* pObj = nullptr;

		pMeshData = CResMgr::GetInst()->LoadFBX(L"fbx\\nomad.fbx");
		pObj = pMeshData->Instantiate(Vec3(fDefaultScale, fDefaultScale, fDefaultScale));
		wstring str = L"fbx nomad" + std::to_wstring(i);
		pObj->SetName(str);

		pObj->AddComponent(new CEnemyScript(CEnemyScript::ENEMY_TYPE::NOMAD));
		pObj->AddComponent(new CRigidBody(RIGID_BODY_SHAPE_TYPE::BOX, RIGID_BODY_TYPE::DYNAMIC));
		pObj->AddComponent(new CCollider3D);
		pObj->AddComponent(new CGizmo);

		pObj->RigidBody()->SetCreature(true);

		PreloadGameObject(pObj, Vec3(500.f, 100.f, 1000.f + (i * 150.f)), LAYER_TYPE::Enemy);

		{
			Ptr<CMeshData> pMeshData = nullptr;
			CGameObject* pGun = nullptr;
			pMeshData = CResMgr::GetInst()->LoadFBX(L"fbx\\dahl.fbx");
			pGun = pMeshData->Instantiate(Vec3(fDefaultScale, fDefaultScale, fDefaultScale));
			pGun->SetName(L"enemy_gun");

			pGun->AddComponent(new CWeaponScript);
			pGun->AddComponent(new CGizmo);
			pGun->SetIsItem(true);
			pObj->AddWeapon(pGun);

			PreloadGameObject(pGun, Vec3(500.f, 100.f, 50.f), LAYER_TYPE::Item);
		}
	}

	{
		
		Ptr<CMeshData> pMeshData = nullptr;
		CGameObject* pAxe = nullptr;
		pMeshData = CResMgr::GetInst()->LoadFBX(L"fbx\\buzz_axe.fbx");
		pAxe = pMeshData->Instantiate(Vec3(fDefaultScale, fDefaultScale, fDefaultScale));
		pAxe->SetName(L"enemy_axe");
		//pAxe->Transform()->SetRelativePosOffset()

		pAxe->AddComponent(new CCollider3D(false));
		pAxe->Collider3D()->SetRaycastable(false);
		pAxe->Collider3D()->SetScale(Vec3(fDefaultScale / 2.f, fDefaultScale * 2.f, fDefaultScale));
		pAxe->AddComponent(new CWeaponScript);
		pAxe->AddComponent(new CAttackNormalScript);
		pAxe->GetScript<CAttackNormalScript>()->SetManual(true);
		pAxe->AddComponent(new CGizmo);
		pAxe->SetIsItem(true);
		

		PreloadGameObject(pAxe, Vec3(0.f, 0.f, 0.f), LAYER_TYPE::NoRaycastingCollider);
		
		CGameObject* pObj = nullptr;

		pMeshData = CResMgr::GetInst()->LoadFBX(L"fbx\\psycho.fbx");
		pObj = pMeshData->Instantiate(Vec3(100.f, 100.f, 100.f));
		pObj->SetName(L"psycho");
		pObj->AddComponent(new CEnemyScript(CEnemyScript::ENEMY_TYPE::PSYCHO));
		pObj->AddComponent(new CRigidBody(RIGID_BODY_SHAPE_TYPE::BOX, RIGID_BODY_TYPE::DYNAMIC));
		pObj->AddComponent(new CCollider3D);
		pObj->AddComponent(new CGizmo);
		pObj->RigidBody()->SetCreature(true);
		pObj->AddWeapon(pAxe, true);

		PreloadGameObject(pObj, Vec3(400.f, 100.f, 500.f), LAYER_TYPE::Enemy);
	}


	/*****************************************************************
	 **************************** LEVEL 1 ****************************
	 *****************************************************************/
	pCurLevel = CLevelMgr::GetInst()->CreateLevel(L"main level 1");
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
		pMainCam->Camera()->SetLayerMask((int)LAYER_TYPE::Camera, false);
		PreloadGameObject(pMainCam, Vec3(0.f, 0.f, 0.f), LAYER_TYPE::Camera);
	}
	{
		CGameObject* pMapCam = new CGameObject;
		pMapCam->SetName(L"MapCamera");

		pMapCam->AddComponent(new CTransform);
		pMapCam->AddComponent(new CCamera);
		pMapCam->AddComponent(new CCameraMoveScript);

		pMapCam->Camera()->SetFarZ(1000000.f);
		pMapCam->Camera()->SetProjType(PROJ_TYPE::ORTHOGRAPHIC);
		pMapCam->Camera()->SetOrthoWidth(1000.f);
		pMapCam->Camera()->SetOrthoHeight(1000.f);
		pMapCam->Camera()->SetScale(0.15f);
		pMapCam->Camera()->SetCameraIndex(1);
		pMapCam->Camera()->SetLayerMaskAll(false);
		pMapCam->Camera()->SetLayerMask((int)LAYER_TYPE::Terrain, true);

		PreloadGameObject(pMapCam, Vec3(0.f, 0.f, 0.f), LAYER_TYPE::Camera);
	}
	{
		CGameObject* pScopeCam = new CGameObject;
		pScopeCam->SetName(L"ScopeCamera");

		pScopeCam->AddComponent(new CTransform);
		pScopeCam->AddComponent(new CCamera);
		pScopeCam->AddComponent(new CCameraMoveScript);

		pScopeCam->Camera()->SetFarZ(1000000.f);
		pScopeCam->Camera()->SetProjType(PROJ_TYPE::PERSPECTIVE);
		pScopeCam->Camera()->SetCameraIndex(2);
		pScopeCam->Camera()->SetLayerMaskAll(true);
		pScopeCam->Camera()->SetLayerMask((int)LAYER_TYPE::ViewPortUI, false);
		pScopeCam->Camera()->SetLayerMask((int)LAYER_TYPE::Camera, false);
		PreloadGameObject(pScopeCam, Vec3(0.f, 0.f, 0.f), LAYER_TYPE::Camera);
	}

	{
		CGameObject* pUICam = new CGameObject;
		pUICam->SetName(L"UICamera");

		pUICam->AddComponent(new CTransform);
		pUICam->AddComponent(new CCamera);

		pUICam->Camera()->SetFarZ(1000000.f);
		pUICam->Camera()->SetProjType(PROJ_TYPE::ORTHOGRAPHIC);
		pUICam->Camera()->SetCameraIndex(3);
		pUICam->Camera()->SetLayerMaskAll(false);
		pUICam->Camera()->SetLayerMask((int)LAYER_TYPE::ViewPortUI, true);
		PreloadGameObject(pUICam, Vec3(0.f, 0.f, 0.f), LAYER_TYPE::Camera);
	}

	{
		CGameObject* pSunLight = new CGameObject;
		pSunLight->SetName(L"Sun Light");
		pSunLight->AddComponent(new CTransform);
		pSunLight->AddComponent(new CLight3D);
		pSunLight->AddComponent(new CGizmo);

		pSunLight->Transform()->SetRelativeRot(Vec3(-231 * DegToRad(), 45.f * DegToRad(), 0.f));
		pSunLight->Light3D()->SetRadius(500.f);
		pSunLight->Light3D()->SetShadow(true);
		pSunLight->Light3D()->SetLightDepthCoeff(0.003f);
		pSunLight->Light3D()->SetFloatConstant(0, 1000.f);
		pSunLight->Light3D()->SetFloatConstant(1, 8.0f);
		pSunLight->Light3D()->SetLightType(LIGHT_TYPE::DIRECTIONAL);
		pSunLight->Light3D()->SetLightColor(Vec3(0.5f, 0.5f, 1.f));
		pSunLight->Light3D()->SetLightAmbient(Vec3(0.1f, 0.1f, 0.15f));

		PreloadGameObject(pSunLight, Vec3(-2000.f, 60000.f, -2000.f), LAYER_TYPE::Default);
	}

	{
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
		PreloadGameObject(pPlane, Vec3(0.f, -10.f, 45000.f), LAYER_TYPE::Environment);
	}

	{
		Ptr<CMeshData> pMeshData = nullptr;
		CGameObject* pObj = nullptr;

		pMeshData = CResMgr::GetInst()->LoadFBX(L"fbx\\level_hyperion_map.fbx");
		pObj = pMeshData->Instantiate(Vec3(fDefaultScale, fDefaultScale, fDefaultScale));
		pObj->AddComponent(new CRigidBody(RIGID_BODY_SHAPE_TYPE::MESH));
		pObj->AddComponent(new CCollider3D(true, false, COLLIDER_SHAPE_TYPE::MESH));

		pObj->SetName(L"fbx level1 map");
		PreloadGameObject(pObj, Vec3(200.f, 10.f, 200.f), LAYER_TYPE::Terrain);
	}
	{
		CreatePlayer(fDefaultScale);
	}

	for(int i = 0; i < 5; ++i)
	{
		{
			Ptr<CMeshData> pMeshData = nullptr;
			CGameObject* pObj = nullptr;

			pMeshData = CResMgr::GetInst()->LoadFBX(L"fbx\\robot.fbx");
			pObj = pMeshData->Instantiate(Vec3(100.f, 100.f, 100.f));
			pObj->SetName(L"robot");
			pObj->AddComponent(new CEnemyScript(CEnemyScript::ENEMY_TYPE::GUN_LOADER));
			pObj->AddComponent(new CRigidBody(RIGID_BODY_SHAPE_TYPE::BOX, RIGID_BODY_TYPE::DYNAMIC));
			pObj->AddComponent(new CCollider3D);
			pObj->AddComponent(new CGizmo);

			PreloadGameObject(pObj, Vec3(100.f, 100.f, 500.f), LAYER_TYPE::Enemy);
			{
				Ptr<CMeshData> pMeshData = nullptr;
				CGameObject* pGun = nullptr;
				pMeshData = CResMgr::GetInst()->LoadFBX(L"fbx\\dahl.fbx");
				pGun = pMeshData->Instantiate(Vec3(fDefaultScale, fDefaultScale, fDefaultScale));
				pGun->SetName(L"enemy_gun");

				pGun->AddComponent(new CWeaponScript);
				pGun->AddComponent(new CGizmo);
				pGun->SetIsItem(true);
				pObj->AddWeapon(pGun);

				PreloadGameObject(pGun, Vec3(500.f, 100.f, 50.f), LAYER_TYPE::Item);
			}
		}
	}

	{
		Ptr<CMeshData> pMeshData = nullptr;
		CGameObject* pObj = nullptr;

		pMeshData = CResMgr::GetInst()->LoadFBX(L"fbx\\constructor.fbx");
		pObj = pMeshData->Instantiate(Vec3(100.f, 100.f, 100.f));
		pObj->SetName(L"constructor");
		pObj->AddComponent(new CConstructorScript);
		//pObj->AddComponent(new CRigidBody(RIGID_BODY_SHAPE_TYPE::BOX, RIGID_BODY_TYPE::DYNAMIC));
		pObj->AddComponent(new CCollider3D);
		pObj->AddComponent(new CGizmo);
		pObj->Transform()->SetRelativeRot(Vec3(0.f, -90.f * DegToRad(), 0.f));
		
		PreloadGameObject(pObj, Vec3(10300.885, -950.f, 4230.077), LAYER_TYPE::Enemy);
	}

	{
		CGameObject* pObj = new CGameObject;
		pObj->SetName(L"Ramp 1");

		pObj->AddComponent(new CTransform);
		pObj->AddComponent(new CLight3D);

		pObj->Light3D()->SetLightType(LIGHT_TYPE::POINT);
		pObj->Light3D()->SetRadius(1300);
		pObj->Light3D()->SetLightColor(Vec3(1.f, 1.f, 1.f));
		pObj->Light3D()->SetLightAmbient(Vec3(1.0f, 1.0f, 1.0f));

		PreloadGameObject(pObj, Vec3(-27.634, 158.081, 3031.312), LAYER_TYPE::Default);
	}

	{
		CGameObject* pObj = new CGameObject;
		pObj->SetName(L"Ramp 2");

		pObj->AddComponent(new CTransform);
		pObj->AddComponent(new CLight3D);

		pObj->Light3D()->SetLightType(LIGHT_TYPE::POINT);
		pObj->Light3D()->SetRadius(1300);
		pObj->Light3D()->SetLightColor(Vec3(1.f, 1.f, 1.f));
		pObj->Light3D()->SetLightAmbient(Vec3(1.0f, 1.0f, 1.0f));

		PreloadGameObject(pObj, Vec3(490.477, 921.263, 3211.335), LAYER_TYPE::Default);
	}

	{
		CGameObject* pObj = new CGameObject;
		pObj->SetName(L"Ramp 3");

		pObj->AddComponent(new CTransform);
		pObj->AddComponent(new CLight3D);

		pObj->Light3D()->SetLightType(LIGHT_TYPE::POINT);
		pObj->Light3D()->SetRadius(1300);
		pObj->Light3D()->SetLightColor(Vec3(1.f, 1.f, 1.f));
		pObj->Light3D()->SetLightAmbient(Vec3(1.0f, 1.0f, 1.0f));

		PreloadGameObject(pObj, Vec3(316.719, 818.832, 4260.014), LAYER_TYPE::Default);
	}

	CreateLoading();

	/*****************************************************************
	 ************************ MAIN MENU LEVEL ************************
	 *****************************************************************/
	pCurLevel = CLevelMgr::GetInst()->CreateLevel(L"main menu level");
	CLevelMgr::GetInst()->ChangeLevel(pCurLevel);
	pCurLevel->ChangeState(LEVEL_STATE::STOP);

	{
		CGameObject* pMainCam = new CGameObject;
		pMainCam->SetName(L"MainMenuCamera");

		pMainCam->AddComponent(new CTransform);
		pMainCam->AddComponent(new CCamera);
		pMainCam->AddComponent(new CCameraMoveScript);

		pMainCam->Camera()->SetFarZ(1000000.f);
		pMainCam->Camera()->SetProjType(PROJ_TYPE::PERSPECTIVE);
		pMainCam->Camera()->SetCameraIndex(0);
		pMainCam->Camera()->SetLayerMaskAll(true);
		pMainCam->Camera()->SetLayerMask((int)LAYER_TYPE::ViewPortUI, false);
		pMainCam->Camera()->SetLayerMask((int)LAYER_TYPE::Camera, false);

		pMainCam->Transform()->SetRelativeRot(Vec3(3.95 * DegToRad(), -42.38f * DegToRad(), 0.f));
		PreloadGameObject(pMainCam, Vec3(346.f, 1135.944f, -96.944f), LAYER_TYPE::Camera);
	}

	{
		CGameObject* pUICam = new CGameObject;
		pUICam->SetName(L"UICamera");
		pUICam->AddComponent(new CTransform);
		pUICam->AddComponent(new CCamera);
		pUICam->Camera()->SetFarZ(1000000.f);
		pUICam->Camera()->SetProjType(PROJ_TYPE::ORTHOGRAPHIC);
		pUICam->Camera()->SetCameraIndex(3);
		pUICam->Camera()->SetLayerMaskAll(false);
		pUICam->Camera()->SetLayerMask((int)LAYER_TYPE::ViewPortUI, true);
		PreloadGameObject(pUICam, Vec3(0.f, 0.f, 0.f), LAYER_TYPE::Camera);
	}

	{
		CGameObject* pSkyBox = new CGameObject;
		pSkyBox->SetName(L"SkyBox");

		pSkyBox->AddComponent(new CTransform);
		pSkyBox->AddComponent(new CSkyBox);

		pSkyBox->Transform()->SetRelativeScale(Vec3(100.f, 100.f, 100));
		pSkyBox->SkyBox()->SetSkyBoxType(SKYBOX_TYPE::SPHERE);
		pSkyBox->SkyBox()->SetSkyBoxTexture(CResMgr::GetInst()->FindRes<CTexture>(L"texture\\skybox\\Sky.png"));

		PreloadGameObject(pSkyBox, Vec3(0.f, 0.f, 0.f), LAYER_TYPE::Default);
	}

	{
		CGameObject* pSunLight = new CGameObject;
		pSunLight->SetName(L"Sun Light");
		pSunLight->AddComponent(new CTransform);
		pSunLight->AddComponent(new CLight3D);
		pSunLight->AddComponent(new CGizmo);

		pSunLight->Transform()->SetRelativeRot(Vec3(-231 * DegToRad(), 45.f * DegToRad(), 0.f));
		pSunLight->Light3D()->SetRadius(500.f);
		pSunLight->Light3D()->SetShadow(true);
		pSunLight->Light3D()->SetLightDepthCoeff(0.003f);
		pSunLight->Light3D()->SetFloatConstant(0, 1000.f);
		pSunLight->Light3D()->SetFloatConstant(1, 8.0f);
		pSunLight->Light3D()->SetLightType(LIGHT_TYPE::DIRECTIONAL);
		pSunLight->Light3D()->SetLightColor(Vec3(0.5f, 0.5f, 1.f));
		pSunLight->Light3D()->SetLightAmbient(Vec3(0.1f, 0.1f, 0.15f));

		PreloadGameObject(pSunLight, Vec3(-2000.f, 60000.f, -2000.f), LAYER_TYPE::Default);
	}
	{
		Ptr<CMeshData> pMeshData = nullptr;
		CGameObject* pObj = nullptr;

		pMeshData = CResMgr::GetInst()->LoadFBX(L"fbx\\main_menu_level_map.fbx");
		pObj = pMeshData->Instantiate(Vec3(fDefaultScale, fDefaultScale, fDefaultScale));

		pObj->SetName(L"fbx main menu map");
		PreloadGameObject(pObj, Vec3(200.f, 1185.f, 200.f), LAYER_TYPE::Environment);
	}

	{
		CUI* pGameStart = new CUI();
		pGameStart->SetName(L"UI Game Start");
		pGameStart->AddComponent(new CTransform);
		pGameStart->Transform()->SetRelativeScale(Vec3(100.f, 100.f, 1.f));
		pGameStart->SetUseHover(true);
		pGameStart->SetOutline(true);
		pGameStart->SetText(L"Game Start");
		pGameStart->SetClickFunc() = std::make_shared<std::function<void()>>([=]()
			{
				//LoadingLevel(L"main level 1");
				CLevelMgr::GetInst()->GetCurLevel()->SetLevelEnd(true);
			});
		pGameStart->SetTextHoverColor(Vec4(255.f / 255.f, 208.f / 255.f, 19.f / 255.f, 1.f));
		PreloadGameObject(pGameStart, Vec3(-440.f, 250.f, 0.f), LAYER_TYPE::ViewPortUI);
	}
	{
		CUI* pGameEnd = new CUI();
		pGameEnd->SetName(L"UI Game End");
		pGameEnd->AddComponent(new CTransform);
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
		CGameObject* pObj = new CGameObject;
		pObj->SetName(L"Fire1");

		pObj->AddComponent(new CTransform);
		pObj->AddComponent(new CMeshRender);
		pObj->AddComponent(new CLight3D);

		pObj->Light3D()->SetLightType(LIGHT_TYPE::POINT);
		pObj->Light3D()->SetRadius(825.f);
		pObj->Light3D()->SetLightColor(Vec3(1.f, 0.f, 0.f));
		pObj->Light3D()->SetLightAmbient(Vec3(1.0f, 0.0, 0.0));

		pObj->Transform()->SetBilboard(true);
		//pObj->Transform()->SetRelativeScale(Vec3(389.f, 717.f, 1.f));
		pObj->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
		Ptr<CMaterial> fireMtrl = new CMaterial(true);

		CResMgr::GetInst()->AddRes(L"FireMtrl", fireMtrl);
		fireMtrl->SetShader(CResMgr::GetInst()->FindRes<CGraphicsShader>(L"Std3DShader"));
		fireMtrl->SetEmissiveCoeff(20.f);
		fireMtrl->SetPerlinNoise(true);
		pObj->MeshRender()->SetMaterial(fireMtrl, 0);

		pObj->AddComponent(new CGizmo);

		PreloadGameObject(pObj, Vec3(-2496.f, 1067.f, 941.f), LAYER_TYPE::Default);
	}
	{
		Ptr<CMaterial> pMtrl = new CMaterial(true);
		pMtrl->SetShader(CResMgr::GetInst()->FindRes<CGraphicsShader>(L"Std3DShader"));
		CResMgr::GetInst()->AddRes(L"FireMtrl2", pMtrl);

		bool flatFire = true;
		pMtrl->SetScalarParam(INT_3, &flatFire);

		CGameObject* Fire = new CGameObject;
		Fire->SetName(L"Fire2");
		Fire->AddComponent(new CTransform);
		Fire->AddComponent(new CMeshRender);
		Fire->AddComponent(new CLight3D);

		Fire->Light3D()->SetLightType(LIGHT_TYPE::POINT);
		Fire->Light3D()->SetRadius(355.f);
		Fire->Light3D()->SetLightColor(Vec3(1.f, 0.f, 0.f));
		Fire->Light3D()->SetLightAmbient(Vec3(1.0f, 0.0, 0.0));

		Fire->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
		Fire->MeshRender()->SetMaterial(pMtrl, 0);

		Fire->Transform()->SetRelativeScale(Vec3(100.f, 100.f, 1.f));
		Fire->Transform()->SetRelativeRot(Vec3(0.f, -40.f * DegToRad(), 0.f));

		PreloadGameObject(Fire, Vec3(-69.f, 1090.f, 217.f), LAYER_TYPE::Default);
	}

	CollisionMgr::GetInst()->SetLayerIntersect(LAYER_TYPE::Enemy, LAYER_TYPE::Player, true);
	CollisionMgr::GetInst()->SetLayerIntersect(LAYER_TYPE::Enemy, LAYER_TYPE::PlayerBullet, true);

	//CollisionMgr::GetInst()->SetLayerIntersect(LAYER_TYPE::EnemyBullet, LAYER_TYPE::Environment, true);
	//CollisionMgr::GetInst()->SetLayerIntersect(LAYER_TYPE::EnemyBullet, LAYER_TYPE::Terrain, true);
	CollisionMgr::GetInst()->SetLayerIntersect(LAYER_TYPE::PlayerBullet, LAYER_TYPE::Environment, true);

	CollisionMgr::GetInst()->SetLayerIntersect(LAYER_TYPE::Player, LAYER_TYPE::EnemyBullet, true);
	CollisionMgr::GetInst()->SetLayerIntersect(LAYER_TYPE::Player, LAYER_TYPE::NoRaycastingCollider, true);
	CollisionMgr::GetInst()->SetLayerIntersect(LAYER_TYPE::Player, LAYER_TYPE::Environment, true);
	CollisionMgr::GetInst()->SetLayerIntersect(LAYER_TYPE::Player, LAYER_TYPE::Terrain, true);
}
