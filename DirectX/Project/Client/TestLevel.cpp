#include "pch.h"
#include "TestLevel.h"

#include <Engine/CollisionMgr.h>
#include <Engine\CLevelMgr.h>
#include <Engine\CLevel.h>
#include <Engine\CLayer.h>
#include <Engine\CGameObject.h>
#include <Engine\components.h>

#include <Engine\CResMgr.h>
#include <Engine\PhysXMgr.h>
#include <Engine\CRenderMgr.h>
#include <Engine\ObjPickerMgr.h>

#include <Script\CPlayerScript.h>
#include <Script\CMonsterScript.h>
#include <Script\CCameraMoveScript.h>

#include "CLevelSaveLoad.h"


#include <Engine/CSetColorShader.h>

//#include <Engine/PhysXT.h>


void CreateTestLevel()
{
	//return;

	// ��ǻƮ ���̴� �׽�Ʈ
	//Ptr<CTexture> pTestTexture = 
	//	CResMgr::GetInst()->CreateTexture(L"ComputeTestTex"
	//		, 200, 200, DXGI_FORMAT_R8G8B8A8_UNORM
	//		, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS
	//		, D3D11_USAGE_DEFAULT );

	//Ptr<CSetColorShader> pCS = (CSetColorShader*)CResMgr::GetInst()->FindRes<CComputeShader>(L"SetColorCS").Get();
	//pCS->SetTargetTexture(pTestTexture);
	//pCS->SetColor(Vec3(1.f, 0.f, 1.f));
	//pCS->Execute();

	//Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"sound\\BGM_Stage1.wav");
	//pSound->Play(1, 0.5f, false);
	CLevel* pCurLevel = CLevelMgr::GetInst()->CreateLevel(L"main level");
	CLevelMgr::GetInst()->ChangeLevel(pCurLevel);
	//CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurLevel();
	pCurLevel->ChangeState(LEVEL_STATE::STOP);

	// Layer �̸�����
	pCurLevel->GetLayer(0)->SetName(L"Default");
	pCurLevel->GetLayer(1)->SetName(L"Monster");
	pCurLevel->GetLayer(2)->SetName(L"Player");
	pCurLevel->GetLayer(3)->SetName(L"Bullet");
	pCurLevel->GetLayer(10)->SetName(L"Light");
	pCurLevel->GetLayer(31)->SetName(L"ViewPort UI");

	// Main Camera Object ����
	CGameObject* pMainCam = new CGameObject;
	pMainCam->SetName(L"MainCamera");

	pMainCam->AddComponent(new CTransform);
	pMainCam->AddComponent(new CCamera);
	pMainCam->AddComponent(new CCameraMoveScript);

	pMainCam->Camera()->SetFarZ(1000000.f);
	pMainCam->Camera()->SetProjType(PROJ_TYPE::PERSPECTIVE);
	pMainCam->Camera()->SetCameraIndex(0);		// MainCamera �� ����
	pMainCam->Camera()->SetLayerMaskAll(true);	// ��� ���̾� üũ
	pMainCam->Camera()->SetLayerMask(31, false);// UI Layer �� ���������� �ʴ´�.

	//SpawnGameObject(pMainCam, Vec3(0.f, 0.f, 0.f), 0);
	PreloadGameObject(pMainCam, Vec3(10.f, 10.f, 0.f), 0);

	// UI cameara
	//CGameObject* pUICam = new CGameObject;
	//pUICam->SetName(L"UICamera");

	//pUICam->AddComponent(new CTransform);
	//pUICam->AddComponent(new CCamera);

	//pUICam->Camera()->SetProjType(PROJ_TYPE::ORTHOGRAPHIC);
	//pUICam->Camera()->SetCameraIndex(1);		// Sub ī�޶�� ����
	//pUICam->Camera()->SetLayerMask(31, true);	// 31�� ���̾ üũ

	//SpawnGameObject(pUICam, Vec3(0.f, 0.f, 0.f), 0);

	CGameObject* pSunLight = new CGameObject;
	pSunLight->SetName(L"SunLight");
	pSunLight->AddComponent(new CTransform);
	pSunLight->AddComponent(new CLight3D);

	pSunLight->Transform()->SetRelativeRot(Vec3(XM_PI / 4.f, XM_PI / 4.f, 0.f));
	pSunLight->Light3D()->SetRadius(500.f);
	pSunLight->Light3D()->SetShadow(true);
	pSunLight->Light3D()->SetFloatConstant(0, 5);
	pSunLight->Light3D()->SetFloatConstant(1, 0.005);
	pSunLight->Light3D()->SetLightDepthCoeff(0.005);
	pSunLight->Light3D()->SetLightType(LIGHT_TYPE::DIRECTIONAL);
	pSunLight->Light3D()->SetLightColor(Vec3(1.f, 1.f, 1.f));
	pSunLight->Light3D()->SetLightAmbient(Vec3(0.15f, 0.15f, 0.15f));

	PreloadGameObject(pSunLight, Vec3(-2000.f, 20000.f, -2000.f), 0);

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
		pCube->SetName(L"Sphere");
		pCube->AddComponent(new CTransform);
		pCube->AddComponent(new CMeshRender);
		pCube->AddComponent(new CRigidBody(RIGID_BODY_SHAPE_TYPE::BOX, RIGID_BODY_TYPE::DYNAMIC));
		pCube->AddComponent(new CCollider3D);
		pCube->Transform()->SetRelativeScale(Vec3(100.f, 100.f, 100.f));

		pCube->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"SphereMesh"));
		pCube->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Std3D_DeferredMtrl"), 0);

		pCube->MeshRender()->GetMaterial(0)->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"texture\\tile\\TILE_01.tga"));
		pCube->MeshRender()->GetMaterial(0)->SetTexParam(TEX_1, CResMgr::GetInst()->FindRes<CTexture>(L"texture\\tile\\TILE_01_N.tga"));
		ObjPickerMgr::GetInst()->SetPickObj(pCube);
		PreloadGameObject(pCube, Vec3(-500.f, 90.f, -500.f), L"Monster");
	}

	CGameObject* pPlane = new CGameObject;
	pPlane->SetName(L"Plane");
	pPlane->AddComponent(new CTransform);
	pPlane->AddComponent(new CMeshRender);
	pPlane->AddComponent(new CRigidBody(RIGID_BODY_SHAPE_TYPE::BOX));
	pPlane->AddComponent(new CCollider3D);

	pPlane->Transform()->SetRelativeScale(Vec3(10000.f, 10000.f, 1.f));
	pPlane->Transform()->SetRelativeRot(Vec3(XM_PI / 2.f, 0.f, 0.f));

	pPlane->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	pPlane->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Std3D_DeferredMtrl"), 0);

	pPlane->MeshRender()->GetMaterial(0)->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"texture\\tile\\TILE_01.tga"));
	pPlane->MeshRender()->GetMaterial(0)->SetTexParam(TEX_1, CResMgr::GetInst()->FindRes<CTexture>(L"texture\\tile\\TILE_01_N.tga"));
	ObjPickerMgr::GetInst()->SetPickObj(pPlane);
	PreloadGameObject(pPlane, Vec3(0.f, -10.f, 0.f), 0);

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
	pSkyBox->SkyBox()->SetSkyBoxTexture(CResMgr::GetInst()->FindRes<CTexture>(L"texture\\skybox\\2k_stars_milky_way.jpg"));

	//SpawnGameObject(pSkyBox, Vec3(0.f, 0.f, 0.f), 0);
	PreloadGameObject(pSkyBox, Vec3(0.f, 0.f, 0.f), 0);

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
	CGameObject* pLandScape = new CGameObject;
	pLandScape->SetName(L"LandScape");

	pLandScape->AddComponent(new CTransform);
	PhysXMgr::GetInst()->CreateScene(Vec3(0.f, 0.f, 0.f));
	pLandScape->Transform()->SetRelativeScale(Vec3(200.f, 1000.f, 200.f));

	pLandScape->AddComponent(new CLandScape);

	//pLandScape->LandScape()->SetBrushGuideLine(pObject);
	pLandScape->LandScape()->SetHeightMapName(L"HeightMap2");
	pLandScape->LandScape()->SetColorMapName(L"ColorMap2");
	//pLandScape->LandScape()->SetFace(32, 32);
	pLandScape->LandScape()->SetFrustumCheck(false);
	//pLandScape->LandScape()->SetHeightMap(CResMgr::GetInst()->FindRes<CTexture>(L"texture\\HeightMap_01.jpg"));

	PreloadGameObject(pLandScape, Vec3(-100.f, 0.f, 0.f), 0);

	// ============
	// FBX Loading
	// ============	
	{
		Ptr<CMeshData> pMeshData = nullptr;
		CGameObject* pObj = nullptr;

		//pMeshData = CResMgr::GetInst()->LoadFBX(L"fbx\\Monster.fbx");
		//pMeshData = CResMgr::GetInst()->LoadFBX(L"fbx\\nomad.fbx");
		pMeshData = CResMgr::GetInst()->LoadFBX(L"fbx\\nomad.fbx");
		//pMeshData = CResMgr::GetInst()->LoadFBX(L"fbx\\anim_NLA.fbx");
		//pMeshData = CResMgr::GetInst()->FindRes<CMeshData>(L"meshdata\\house.mdat");
		//pObj = pMeshData->Instantiate();
		//pObj->SetName(L"House");

		//pMeshData = CResMgr::GetInst()->LoadFBX(L"fbx\\monster.fbx");
		//pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"meshdata\\monster.mdat", L"meshdata\\monster.mdat");
		//pMeshData = CResMgr::GetInst()->FindRes<CMeshData>(L"meshdata\\anim_NLA.mdat");
		pObj = pMeshData->Instantiate();
		pObj->SetName(L"player");
		//pObj->AddComponent(new CPathFind);
		//pObj->AddComponent(new CMonsterScript);
		pObj->AddComponent(new CPlayerScript);
		pObj->AddComponent(new CRigidBody(RIGID_BODY_SHAPE_TYPE::BOX, RIGID_BODY_TYPE::DYNAMIC));
		pObj->RigidBody()->SetCreature(true);
		pObj->AddComponent(new CCollider3D);

		//pObj->AddChild(pMainCam);
		pObj->SetFollowObj(pMainCam);
		//pMainCam->SetFollowObj(pObj);
		pMainCam->Transform()->SetFollowOffset(Vec3(0.f, 90.f, 0.f));

		//SpawnGameObject(pObj, Vec3(0.f, 0.f, 100.f), L"Default");
		PreloadGameObject(pObj, Vec3(50.f, 50.f, 50.f), L"Player");
	}
	{
		//Ptr<CMeshData> pMeshData = nullptr;
		//CGameObject* pObj = nullptr;

		//pMeshData = CResMgr::GetInst()->LoadFBX(L"fbx\\terrain.fbx");
		//pObj = pMeshData->Instantiate();
		//pObj->AddComponent(new CRigidBody(RIGID_BODY_SHAPE_TYPE::MESH));
		//pObj->AddComponent(new CCollider3D);

		//pObj->SetName(L"fbx terrain");
		//PreloadGameObject(pObj, Vec3(200.f, 100.f, 200.f), L"Default");
	}
	//{
	//	Ptr<CMeshData> pMeshData = nullptr;
	//	CGameObject* pObj = nullptr;

	//	pMeshData = CResMgr::GetInst()->LoadFBX(L"fbx\\nomad.fbx");
	//	pObj = pMeshData->Instantiate();
	//	pObj->SetName(L"fbx nomad");
	//	
	//	PreloadGameObject(pObj, Vec3(500.f, 100.f, 500.f), L"Default");
	//}
	
	pCurLevel = CLevelMgr::GetInst()->CreateLevel(L"switch level");
	CLevelMgr::GetInst()->ChangeLevel(pCurLevel);
	//CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurLevel();
	pCurLevel->ChangeState(LEVEL_STATE::STOP);

	pCurLevel->GetLayer(0)->SetName(L"Default");
	pCurLevel->GetLayer(1)->SetName(L"Monster");
	pCurLevel->GetLayer(2)->SetName(L"Player");
	pCurLevel->GetLayer(3)->SetName(L"Bullet");
	pCurLevel->GetLayer(10)->SetName(L"Light");
	pCurLevel->GetLayer(31)->SetName(L"ViewPort UI");

	pMainCam = new CGameObject;
	pMainCam->SetName(L"MainCamera");

	pMainCam->AddComponent(new CTransform);
	pMainCam->AddComponent(new CCamera);
	pMainCam->AddComponent(new CCameraMoveScript);

	pMainCam->Camera()->SetFarZ(1000000.f);
	pMainCam->Camera()->SetProjType(PROJ_TYPE::PERSPECTIVE);
	pMainCam->Camera()->SetCameraIndex(0);		// MainCamera �� ����
	pMainCam->Camera()->SetLayerMaskAll(true);	// ��� ���̾� üũ
	pMainCam->Camera()->SetLayerMask(31, false);// UI Layer �� ���������� �ʴ´�.
	
	PreloadGameObject(pMainCam, Vec3(0.f, 0.f, 0.f), 0);

	/*CGameObject**/ pLandScape = new CGameObject;
	pLandScape->SetName(L"LandScape");

	pLandScape->AddComponent(new CTransform);
	pLandScape->AddComponent(new CLandScape);

	pLandScape->Transform()->SetRelativeScale(Vec3(200.f, 1000.f, 200.f));
	//pLandScape->LandScape()->SetBrushGuideLine(pObject);
	pLandScape->LandScape()->SetHeightMapName(L"HeightMap1");
	pLandScape->LandScape()->SetColorMapName(L"ColorMap1");
	pLandScape->LandScape()->SetFace(32, 32);
	pLandScape->LandScape()->SetFrustumCheck(false);
	//pLandScape->LandScape()->SetHeightMap(CResMgr::GetInst()->FindRes<CTexture>(L"texture\\HeightMap_01.jpg"));
	PreloadGameObject(pLandScape, Vec3(0.f, 0.f, 0.f), 1);

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

	/*CGameObject* */pSkyBox = new CGameObject;
	pSkyBox->SetName(L"SkyBox");

	pSkyBox->AddComponent(new CTransform);
	pSkyBox->AddComponent(new CSkyBox);

	pSkyBox->Transform()->SetRelativeScale(Vec3(100.f, 100.f, 100));
	pSkyBox->SkyBox()->SetSkyBoxType(SKYBOX_TYPE::SPHERE);
	pSkyBox->SkyBox()->SetSkyBoxTexture(CResMgr::GetInst()->FindRes<CTexture>(L"texture\\skybox\\2k_stars_milky_way.jpg"));

	PreloadGameObject(pSkyBox, Vec3(0.f, 0.f, 0.f), 1);

	CollisionMgr::GetInst()->SetLayerIntersect(L"Monster", L"Bullet", true);
	CollisionMgr::GetInst()->SetLayerIntersect(L"Default", L"Bullet", true);
	CollisionMgr::GetInst()->SetLayerIntersect(L"Monster", L"Player", true);
	CollisionMgr::GetInst()->SetLayerIntersect(L"Monster", L"Default", true);
	CollisionMgr::GetInst()->SetLayerIntersect(L"Player", L"Default", true);
}
