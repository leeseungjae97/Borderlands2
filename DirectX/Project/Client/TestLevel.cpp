#include "pch.h"
#include "TestLevel.h"

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

#include "CLevelSaveLoad.h"


#include <Engine/CSetColorShader.h>

//#include <Engine/PhysXT.h>


void CreateTestLevel()
{
	//return;

	// 컴퓨트 쉐이더 테스트
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

	// Layer 이름설정
	pCurLevel->GetLayer(0)->SetName(L"Default");
	pCurLevel->GetLayer(1)->SetName(L"Objects");
	pCurLevel->GetLayer(10)->SetName(L"Light");
	pCurLevel->GetLayer(31)->SetName(L"ViewPort UI");

	// Main Camera Object 생성
	CGameObject* pMainCam = new CGameObject;
	pMainCam->SetName(L"MainCamera");

	pMainCam->AddComponent(new CTransform);
	pMainCam->AddComponent(new CCamera);
	//pMainCam->AddComponent(new CCameraMoveScript);

	pMainCam->Camera()->SetFarZ(1000000.f);
	pMainCam->Camera()->SetProjType(PROJ_TYPE::PERSPECTIVE);
	pMainCam->Camera()->SetCameraIndex(0);		// MainCamera 로 설정
	pMainCam->Camera()->SetLayerMaskAll(true);	// 모든 레이어 체크
	pMainCam->Camera()->SetLayerMask(31, false);// UI Layer 는 렌더링하지 않는다.

	//SpawnGameObject(pMainCam, Vec3(0.f, 0.f, 0.f), 0);
	PreloadGameObject(pMainCam, Vec3(0.f, 0.f, 0.f), 0);

	// UI cameara
	//CGameObject* pUICam = new CGameObject;
	//pUICam->SetName(L"UICamera");

	//pUICam->AddComponent(new CTransform);
	//pUICam->AddComponent(new CCamera);

	//pUICam->Camera()->SetProjType(PROJ_TYPE::ORTHOGRAPHIC);
	//pUICam->Camera()->SetCameraIndex(1);		// Sub 카메라로 지정
	//pUICam->Camera()->SetLayerMask(31, true);	// 31번 레이어만 체크

	//SpawnGameObject(pUICam, Vec3(0.f, 0.f, 0.f), 0);

	CGameObject* pSunLight = new CGameObject;
	pSunLight->SetName(L"SunLight");
	pSunLight->AddComponent(new CTransform);
	pSunLight->AddComponent(new CLight3D);

	pSunLight->Transform()->SetRelativeRot(Vec3(XM_PI / 4.f, XM_PI / 4.f, 0.f));
	pSunLight->Light3D()->SetRadius(500.f);
	//pSunLight->Light3D()->SetShadow(true);
	//pSunLight->Light3D()->SetFloatConstant(0, 5);
	//pSunLight->Light3D()->SetFloatConstant(1, 0.005);
	//pSunLight->Light3D()->SetLightDepthCoeff(0.005);
	pSunLight->Light3D()->SetLightType(LIGHT_TYPE::DIRECTIONAL);
	pSunLight->Light3D()->SetLightColor(Vec3(1.f, 1.f, 1.f));
	pSunLight->Light3D()->SetLightAmbient(Vec3(0.15f, 0.15f, 0.15f));

	//SpawnGameObject(pSunLight, Vec3(-2000.f, 3500.f, -2000.f), 0);
	PreloadGameObject(pSunLight, Vec3(-2000.f, 3500.f, -2000.f), 0);

	//CGameObject* pSphere = new CGameObject;
	//pSphere->SetName(L"Sphere1");
	//pSphere->AddComponent(new CTransform);
	//pSphere->AddComponent(new CMeshRender);

	//pSphere->Transform()->SetRelativeScale(Vec3(1000.f, 1000.f, 1000.f));

	//pSphere->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"SphereMesh"));
	//pSphere->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Std3D_DeferredMtrl"), 0);

	//pSphere->MeshRender()->GetMaterial(0)->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"texture\\tile\\TILE_01.tga"));
	//
	////SpawnGameObject(pSphere, Vec3(500.f, 500.f, 0.f), L"Planet");
	//PreloadGameObject(pSphere, Vec3(500.f, 500.f, 0.f), L"Objects");

	//CGameObject* pSphere = new CGameObject;
	//pSphere->SetName(L"Sphere");
	//pSphere->AddComponent(new CTransform);
	//pSphere->AddComponent(new CMeshRender);

	//pSphere->Transform()->SetRelativeScale(Vec3(1000.f, 1000.f, 1000.f));

	//pSphere->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"SphereMesh"));
	//pSphere->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Std3D_DeferredMtrl"), 0);

	//pSphere->MeshRender()->GetMaterial(0)->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"texture\\tile\\TILE_01.tga"));
	//
	////SpawnGameObject(pSphere, Vec3(0.f, 0.f, 0.f), L"Planet");
	//PreloadGameObject(pSphere, Vec3(0.f, 0.f, 0.f), L"Objects");


	CGameObject* pRectMesh = new CGameObject;
	pRectMesh->SetName(L"Rect");
	pRectMesh->AddComponent(new CTransform);
	pRectMesh->AddComponent(new CMeshRender);

	pRectMesh->Transform()->SetRelativeScale(Vec3(8000.f, 8000.f, 8000.f));
	pRectMesh->Transform()->SetRelativeRot(Vec3(XM_PI / 2.f, 0.f, 0.f));

	pRectMesh->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	pRectMesh->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Std3D_DeferredMtrl"), 0);

	pRectMesh->MeshRender()->GetMaterial(0)->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"texture\\tile\\TILE_01.tga"));
	
	//SpawnGameObject(pRectMesh, Vec3(0.f, -1000.f, 0.f), L"Planet");
	PreloadGameObject(pRectMesh, Vec3(0.f, -1000.f, 0.f), L"Objects");

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
	//pObject->Decal()->SetEmissiveDecal(true);

	//pObject->Transform()->SetRelativeScale(Vec3(100.f, 100.f, 100.f));
	//pObject->Transform()->SetRelativeRot(Vec3(0.f, 0.f, 0.f));
	////CCamera* cam = CRenderMgr::GetInst()->GetEditorCam();
	////pObject->SetFollowObj(cam->GetOwner());

	////SpawnGameObject(pObject, Vec3(0.f, 0.f, 0.f), L"Default");
	//PreloadGameObject(pObject, Vec3(0.f, 0.f, 0.f), L"Default");

	// LandScape Object
	CGameObject* pLandScape = new CGameObject;
	pLandScape->SetName(L"LandScape");

	pLandScape->AddComponent(new CTransform);

	pLandScape->Transform()->SetRelativeScale(Vec3(200.f, 1000.f, 200.f));

	pLandScape->AddComponent(new CLandScape);

	//pLandScape->LandScape()->SetBrushGuideLine(pObject);
	pLandScape->LandScape()->SetHeightMapName(L"HeightMap2");
	pLandScape->LandScape()->SetColorMapName(L"ColorMap2");
	//pLandScape->LandScape()->SetFace(32, 32);
	pLandScape->LandScape()->SetFrustumCheck(false);
	//pLandScape->LandScape()->SetHeightMap(CResMgr::GetInst()->FindRes<CTexture>(L"texture\\HeightMap_01.jpg"));

	PreloadGameObject(pLandScape, Vec3(-100.f, 0.f, 0.f), L"Objects");

	// ============
	// FBX Loading
	// ============	
	{
		Ptr<CMeshData> pMeshData = nullptr;
		CGameObject* pObj = nullptr;

		//pMeshData = CResMgr::GetInst()->LoadFBX(L"fbx\\Monster.fbx");
		//pMeshData = CResMgr::GetInst()->LoadFBX(L"fbx\\nomad.fbx");
		pMeshData = CResMgr::GetInst()->LoadFBX(L"fbx\\soldier_fps.fbx");
		//pMeshData = CResMgr::GetInst()->LoadFBX(L"fbx\\anim_NLA.fbx");
		//pMeshData = CResMgr::GetInst()->FindRes<CMeshData>(L"meshdata\\house.mdat");
		//pObj = pMeshData->Instantiate();
		//pObj->SetName(L"House");

		//pMeshData = CResMgr::GetInst()->LoadFBX(L"fbx\\monster.fbx");
		//pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"meshdata\\monster.mdat", L"meshdata\\monster.mdat");
		//pMeshData = CResMgr::GetInst()->FindRes<CMeshData>(L"meshdata\\anim_NLA.mdat");
		pObj = pMeshData->Instantiate();
		//pObj->AddComponent(new CPathFind);
		//pObj->AddComponent(new CMonsterScript);
		pObj->AddComponent(new CPlayerScript);
		pObj->SetFollowObj(pMainCam);

		//pObj->PathFind()->SetMVecEnd()
		pObj->SetName(L"soldier hands");

		//SpawnGameObject(pObj, Vec3(0.f, 0.f, 100.f), L"Default");
		PreloadGameObject(pObj, Vec3(20.f, 0.f, 100.f), L"Default");
	}
	{
		Ptr<CMeshData> pMeshData = nullptr;
		CGameObject* pObj = nullptr;

		//pMeshData = CResMgr::GetInst()->LoadFBX(L"fbx\\Monster.fbx");
		//pMeshData = CResMgr::GetInst()->LoadFBX(L"fbx\\nomad.fbx");
		pMeshData = CResMgr::GetInst()->LoadFBX(L"fbx\\terrain.fbx");
		//pMeshData = CResMgr::GetInst()->LoadFBX(L"fbx\\anim_NLA.fbx");
		//pMeshData = CResMgr::GetInst()->FindRes<CMeshData>(L"meshdata\\house.mdat");
		//pObj = pMeshData->Instantiate();
		//pObj->SetName(L"House");

		//pMeshData = CResMgr::GetInst()->LoadFBX(L"fbx\\monster.fbx");
		//pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"meshdata\\monster.mdat", L"meshdata\\monster.mdat");
		//pMeshData = CResMgr::GetInst()->FindRes<CMeshData>(L"meshdata\\anim_NLA.mdat");
		pObj = pMeshData->Instantiate();
		//pObj->AddComponent(new CPathFind);
		//pObj->AddComponent(new CMonsterScript);
		//pObj->AddComponent(new CPlayerScript);
		//pObj->SetFollowObj(pMainCam);

		//pObj->PathFind()->SetMVecEnd()
		pObj->SetName(L"fbx terrain");

		//SpawnGameObject(pObj, Vec3(0.f, 0.f, 100.f), L"Default");
		PreloadGameObject(pObj, Vec3(200.f, 0.f, 200.f), L"Default");
	}
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
		//pObj->AddComponent(new CPathFind);
		//pObj->AddComponent(new CMonsterScript);
		//pObj->AddComponent(new CPlayerScript);
		//pObj->SetFollowObj(pMainCam);

		//pObj->PathFind()->SetMVecEnd()
		pObj->SetName(L"fbx nomad");

		//SpawnGameObject(pObj, Vec3(0.f, 0.f, 100.f), L"Default");
		PreloadGameObject(pObj, Vec3(0.f, 0.f, 100.f), L"Default");
	}
	
	pCurLevel = CLevelMgr::GetInst()->CreateLevel(L"switch level");
	CLevelMgr::GetInst()->ChangeLevel(pCurLevel);
	//CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurLevel();
	pCurLevel->ChangeState(LEVEL_STATE::STOP);

	pCurLevel->GetLayer(0)->SetName(L"Default");
	pCurLevel->GetLayer(1)->SetName(L"Objects");
	pCurLevel->GetLayer(10)->SetName(L"Light");
	pCurLevel->GetLayer(31)->SetName(L"ViewPort UI");

	pMainCam = new CGameObject;
	pMainCam->SetName(L"MainCamera");

	pMainCam->AddComponent(new CTransform);
	pMainCam->AddComponent(new CCamera);
	pMainCam->AddComponent(new CCameraMoveScript);

	pMainCam->Camera()->SetFarZ(1000000.f);
	pMainCam->Camera()->SetProjType(PROJ_TYPE::PERSPECTIVE);
	pMainCam->Camera()->SetCameraIndex(0);		// MainCamera 로 설정
	pMainCam->Camera()->SetLayerMaskAll(true);	// 모든 레이어 체크
	pMainCam->Camera()->SetLayerMask(31, false);// UI Layer 는 렌더링하지 않는다.
	
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

	//CGameObject* pSunLight = new CGameObject;
	//pSunLight->SetName(L"SunLight");
	//pSunLight->AddComponent(new CTransform);
	//pSunLight->AddComponent(new CLight3D);

	//pSunLight->Transform()->SetRelativeRot(Vec3(XM_PI / 4.f, XM_PI / 4.f, 0.f));
	//pSunLight->Light3D()->SetRadius(500.f);
	//pSunLight->Light3D()->SetLightType(LIGHT_TYPE::DIRECTIONAL);
	//pSunLight->Light3D()->SetLightColor(Vec3(1.f, 1.f, 1.f));
	//pSunLight->Light3D()->SetLightAmbient(Vec3(0.15f, 0.15f, 0.15f));

	////SpawnGameObject(pSunLight, Vec3(-2000.f, 3500.f, -2000.f), 0);
	//PreloadGameObject(pSunLight, Vec3(-2000.f, 3500.f, -2000.f), 1);

	//CGameObject* pSkyBox = new CGameObject;
	//pSkyBox->SetName(L"SkyBox");

	//pSkyBox->AddComponent(new CTransform);
	//pSkyBox->AddComponent(new CSkyBox);

	//pSkyBox->Transform()->SetRelativeScale(Vec3(100.f, 100.f, 100));
	//pSkyBox->SkyBox()->SetSkyBoxType(SKYBOX_TYPE::SPHERE);
	//pSkyBox->SkyBox()->SetSkyBoxTexture(CResMgr::GetInst()->FindRes<CTexture>(L"texture\\skybox\\2k_stars_milky_way.jpg"));

	//PreloadGameObject(pSkyBox, Vec3(0.f, 0.f, 0.f), 1);
	// 충돌 시킬 레이어 짝 지정
	//CCollisionMgr::GetInst()->LayerCheck(L"Player", L"Monster");	
}
