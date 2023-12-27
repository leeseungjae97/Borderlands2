#include "pch.h"
#include "TestLevel.h"

#include <Engine\CLevelMgr.h>
#include <Engine\CLevel.h>
#include <Engine\CLayer.h>
#include <Engine\CGameObject.h>
#include <Engine\components.h>

#include <Engine\CResMgr.h>
#include <Engine\CCollisionMgr.h>

#include <Script\CPlayerScript.h>
#include <Script\CMonsterScript.h>

#include "CLevelSaveLoad.h"


#include <Engine/CSetColorShader.h>


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

	CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurLevel();
	pCurLevel->ChangeState(LEVEL_STATE::STOP);

	// Layer 이름설정
	pCurLevel->GetLayer(0)->SetName(L"Default");
	pCurLevel->GetLayer(1)->SetName(L"Planet");
	pCurLevel->GetLayer(10)->SetName(L"Light");
	pCurLevel->GetLayer(31)->SetName(L"ViewPort UI");


	// Main Camera Object 생성
	CGameObject* pMainCam = new CGameObject;
	pMainCam->SetName(L"MainCamera");

	pMainCam->AddComponent(new CTransform);
	pMainCam->AddComponent(new CCamera);

	pMainCam->Camera()->SetProjType(PROJ_TYPE::ORTHOGRAPHIC);
	pMainCam->Camera()->SetCameraIndex(0);		// MainCamera 로 설정
	pMainCam->Camera()->SetLayerMaskAll(true);	// 모든 레이어 체크
	pMainCam->Camera()->SetLayerMask(31, false);// UI Layer 는 렌더링하지 않는다.

	SpawnGameObject(pMainCam, Vec3(0.f, 0.f, 0.f), 0);

	// UI cameara
	CGameObject* pUICam = new CGameObject;
	pUICam->SetName(L"UICamera");

	pUICam->AddComponent(new CTransform);
	pUICam->AddComponent(new CCamera);

	pUICam->Camera()->SetProjType(PROJ_TYPE::ORTHOGRAPHIC);
	pUICam->Camera()->SetCameraIndex(1);		// Sub 카메라로 지정
	pUICam->Camera()->SetLayerMask(31, true);	// 31번 레이어만 체크

	SpawnGameObject(pUICam, Vec3(0.f, 0.f, 0.f), 0);

	CGameObject* pSun = new CGameObject;
	pSun->SetName(L"Sun");
	pSun->AddComponent(new CTransform);
	pSun->AddComponent(new CSolarSystem);
	pSun->AddComponent(new CPlanetMove);

	pSun->PlanetMove()->SetRotate(true);
	pSun->Transform()->SetRelativeScale(Vec3(1000.f, 1000.f, 1000.f));

	pSun->SolarSystem()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"SphereMesh"));
	pSun->SolarSystem()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Planet3D_DeferredMtrl"));

	pSun->SolarSystem()->SetPlanetTex(CResMgr::GetInst()->FindRes<CTexture>(L"texture\\solar_system\\sun\\2k_sun.jpg"));
	//pCube->SolarSystem()->GetMaterial()->SetTexParam(TEX_1, CResMgr::GetInst()->FindRes<CTexture>(L"texture\\tile\\TILE_01_N.tga"));
	pSun->SolarSystem()->SetEmissive(true);
	
	SpawnGameObject(pSun, Vec3(0.f, 0.f, 0.f), L"Planet");

	CGameObject* pSunLight = new CGameObject;
	pSunLight->SetName(L"SunLight");
	pSunLight->AddComponent(new CTransform);
	pSunLight->AddComponent(new CLight3D);

	pSunLight->Light3D()->SetLightType(LIGHT_TYPE::POINT);
	pSunLight->Light3D()->SetLightColor(Vec3(1.f, 1.f, 1.f));
	pSunLight->Light3D()->SetLightAmbient(Vec3(0.15f, 0.15f, 0.15f));
	pSunLight->Light3D()->SetRadius(100000.f);
	//pSunLight->Light3D()->SetAngle(XM_PI / 8.f);

	//pDirLight->Transform()->SetRelativeRot(Vec3(XM_PI / 2.f, 0.f, 0.f));

	SpawnGameObject(pSunLight, Vec3(0.f, 0.f, 0.f), L"Light");

	CGameObject* pMercury = new CGameObject;
	pMercury->SetName(L"Mercury");
	pMercury->AddComponent(new CTransform);
	pMercury->AddComponent(new CSolarSystem);
	pMercury->AddComponent(new CPlanetMove);

	pMercury->PlanetMove()->SetOrbit(true);
	pMercury->PlanetMove()->SetRotate(true);
	pMercury->PlanetMove()->SetOrbitSpeed(1000.f);
	pMercury->PlanetMove()->SetOrbitRadius(1000.f);

	pMercury->Transform()->SetRelativeScale(Vec3(200.f, 200.f, 200.f));
	pMercury->SolarSystem()->SetEmissive(true);
	pMercury->SolarSystem()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"SphereMesh"));
	pMercury->SolarSystem()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Planet3D_DeferredMtrl"));
	pMercury->SolarSystem()->SetPlanetTex(CResMgr::GetInst()->FindRes<CTexture>(L"texture\\solar_system\\mercury\\2k_mercury.jpg"));
	//pCube->SolarSystem()->GetMaterial()->SetTexParam(TEX_1, CResMgr::GetInst()->FindRes<CTexture>(L"texture\\tile\\TILE_01_N.tga"));
	//pMercury->SolarSystem()->SetEmissive(true);
	
	SpawnGameObject(pMercury, Vec3(1000.f, 0.f, 0.f), L"Planet");

	CGameObject* pVenus = new CGameObject;
	pVenus->SetName(L"Venus");
	pVenus->AddComponent(new CTransform);
	pVenus->AddComponent(new CSolarSystem);
	pVenus->AddComponent(new CPlanetMove);

	pVenus->PlanetMove()->SetOrbit(true);
	pVenus->PlanetMove()->SetRotate(true);
	pVenus->PlanetMove()->SetOrbitSpeed(1000.f);
	pVenus->PlanetMove()->SetOrbitRadius(1200.f);

	pVenus->Transform()->SetRelativeScale(Vec3(200.f, 200.f, 200.f));

	pVenus->SolarSystem()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"SphereMesh"));
	pVenus->SolarSystem()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Planet3D_DeferredMtrl"));
	pVenus->SolarSystem()->SetPlanetTex(CResMgr::GetInst()->FindRes<CTexture>(L"texture\\solar_system\\venus\\2k_venus_surface.jpg"));
	
	SpawnGameObject(pVenus, Vec3(1200.f, 0.f, -100.f), L"Planet");

	CGameObject* pEarth = new CGameObject;
	pEarth->SetName(L"Earth");
	pEarth->AddComponent(new CTransform);
	pEarth->AddComponent(new CSolarSystem);
	pEarth->AddComponent(new CPlanetMove);
	pEarth->PlanetMove()->SetOrbit(true);
	pEarth->PlanetMove()->SetRotate(true);
	pEarth->PlanetMove()->SetOrbitSpeed(1000.f);
	pEarth->PlanetMove()->SetOrbitRadius(1400.f);

	pEarth->Transform()->SetRelativeScale(Vec3(200.f, 200.f, 200.f));

	pEarth->SolarSystem()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"SphereMesh"));
	pEarth->SolarSystem()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Planet3D_DeferredMtrl"));
	pEarth->SolarSystem()->SetPlanetTex(CResMgr::GetInst()->FindRes<CTexture>(L"texture\\solar_system\\earth\\2k_earth_nightmap.jpg"));
	
	SpawnGameObject(pEarth, Vec3(1400, 0.f, -200.f), L"Planet");

	CGameObject* pMars = new CGameObject;
	pMars->SetName(L"Mars");
	pMars->AddComponent(new CTransform);
	pMars->AddComponent(new CSolarSystem);
	pMars->AddComponent(new CPlanetMove);
	pMars->PlanetMove()->SetOrbit(true);
	pMars->PlanetMove()->SetRotate(true);
	pMars->PlanetMove()->SetOrbitSpeed(1000.f);
	pMars->PlanetMove()->SetOrbitRadius(1600.f);

	pMars->Transform()->SetRelativeScale(Vec3(200.f, 200.f, 200.f));

	pMars->SolarSystem()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"SphereMesh"));
	pMars->SolarSystem()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Planet3D_DeferredMtrl"));
	pMars->SolarSystem()->SetPlanetTex(CResMgr::GetInst()->FindRes<CTexture>(L"texture\\solar_system\\mars\\2k_mars.jpg"));
	
	SpawnGameObject(pMars, Vec3(1600, 0.f, -300.f), L"Planet");

	CGameObject* pJupiter = new CGameObject;
	pJupiter->SetName(L"Jupiter");
	pJupiter->AddComponent(new CTransform);
	pJupiter->AddComponent(new CSolarSystem);
	pJupiter->AddComponent(new CPlanetMove);
	pJupiter->PlanetMove()->SetOrbit(true);
	pJupiter->PlanetMove()->SetRotate(true);
	pJupiter->PlanetMove()->SetOrbitSpeed(1000.f);
	pJupiter->PlanetMove()->SetOrbitRadius(1800.f);

	pJupiter->Transform()->SetRelativeScale(Vec3(200.f, 200.f, 200.f));

	pJupiter->SolarSystem()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"SphereMesh"));
	pJupiter->SolarSystem()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Planet3D_DeferredMtrl"));
	pJupiter->SolarSystem()->SetPlanetTex(CResMgr::GetInst()->FindRes<CTexture>(L"texture\\solar_system\\jupiter\\2k_jupiter.jpg"));
	
	SpawnGameObject(pJupiter, Vec3(1800, 0.f, -400.f), L"Planet");

	CGameObject* pSaturn = new CGameObject;
	pSaturn->SetName(L"Saturn");
	pSaturn->AddComponent(new CTransform);
	pSaturn->AddComponent(new CSolarSystem);
	pSaturn->AddComponent(new CPlanetMove);
	//pSaturn->PlanetMove()->SetOrbit(true);
	pSaturn->PlanetMove()->SetRotate(true);
	pSaturn->PlanetMove()->SetOrbitSpeed(1000.f);
	pSaturn->PlanetMove()->SetOrbitRadius(2000.f);

	pSaturn->Transform()->SetRelativeScale(Vec3(200.f, 200.f, 200.f));

	pSaturn->SolarSystem()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"SphereMesh"));
	pSaturn->SolarSystem()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Planet3D_DeferredMtrl"));
	pSaturn->SolarSystem()->SetPlanetTex(CResMgr::GetInst()->FindRes<CTexture>(L"texture\\solar_system\\saturn\\2k_saturn.jpg"));
	
	SpawnGameObject(pSaturn, Vec3(2000.f, 0.f, -500.f), L"Planet");

	CGameObject* pSaturnRing = new CGameObject;
	//pSaturnRing->SetParent();
	pSaturnRing->SetName(L"Saturn ring");
	pSaturnRing->AddComponent(new CTransform);
	pSaturnRing->AddComponent(new CSolarSystem);
	pSaturnRing->AddComponent(new CPlanetMove);
	//pSaturnRing->PlanetMove()->SetOrbit(true);
	//pSaturnRing->PlanetMove()->SetRotate(true);
	//pSaturnRing->PlanetMove()->SetXRotate(true);

	pSaturnRing->Transform()->SetRelativeScale(Vec3(1.f, 0.1f, 1.f));
	pSaturnRing->Transform()->SetRelativeRot(Vec3(XM_PI / 2.f, 0.f, 0.f));
	pSaturnRing->Transform()->SetRelativePos(Vec3(1.5f, 0.f, 0.f));

	pSaturnRing->SolarSystem()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	pSaturnRing->SolarSystem()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Planet3D_DeferredMtrl"));
	pSaturnRing->SolarSystem()->SetPlanetTex(CResMgr::GetInst()->FindRes<CTexture>(L"texture\\solar_system\\saturn\\2k_saturn_ring_alpha.png"));
	
	//SpawnGameObject(pSaturnRing, Vec3(2000.f, 0.f, -500.f), L"Planet");
	pSaturn->AddChild(pSaturnRing);

	CGameObject* pUranus = new CGameObject;
	pUranus->SetName(L"Uranus");
	pUranus->AddComponent(new CTransform);
	pUranus->AddComponent(new CSolarSystem);
	pUranus->AddComponent(new CPlanetMove);
	pUranus->PlanetMove()->SetOrbit(true);
	pUranus->PlanetMove()->SetRotate(true);
	pUranus->PlanetMove()->SetOrbitSpeed(1000.f);
	pUranus->PlanetMove()->SetOrbitRadius(2200.f);

	pUranus->Transform()->SetRelativeScale(Vec3(200.f, 200.f, 200.f));

	pUranus->SolarSystem()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"SphereMesh"));
	pUranus->SolarSystem()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Planet3D_DeferredMtrl"));
	pUranus->SolarSystem()->SetPlanetTex(CResMgr::GetInst()->FindRes<CTexture>(L"texture\\solar_system\\uranus\\2k_uranus.jpg"));
	
	SpawnGameObject(pUranus, Vec3(2200.f, 0.f, -600.f), L"Planet");

	CGameObject* pNeptune = new CGameObject;
	pNeptune->SetName(L"Neptune");
	pNeptune->AddComponent(new CTransform);
	pNeptune->AddComponent(new CSolarSystem);
	pNeptune->AddComponent(new CPlanetMove);
	pNeptune->PlanetMove()->SetOrbit(true);
	pNeptune->PlanetMove()->SetRotate(true);
	pNeptune->PlanetMove()->SetOrbitSpeed(1000.f);
	pNeptune->PlanetMove()->SetOrbitRadius(2400.f);

	pNeptune->Transform()->SetRelativeScale(Vec3(200.f, 200.f, 200.f));

	pNeptune->SolarSystem()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"SphereMesh"));
	pNeptune->SolarSystem()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Planet3D_DeferredMtrl"));
	pNeptune->SolarSystem()->SetPlanetTex(CResMgr::GetInst()->FindRes<CTexture>(L"texture\\solar_system\\neptune\\2k_neptune.jpg"));
	
	SpawnGameObject(pNeptune, Vec3(2400.f, 0.f, -700.f), L"Planet");

	CGameObject* pSkyBox = new CGameObject;
	pSkyBox->SetName(L"SkyBox");

	pSkyBox->AddComponent(new CTransform);
	pSkyBox->AddComponent(new CSkyBox);

	pSkyBox->Transform()->SetRelativeScale(Vec3(100.f, 100.f, 100));
	pSkyBox->SkyBox()->SetSkyBoxType(SKYBOX_TYPE::SPHERE);
	pSkyBox->SkyBox()->SetSkyBoxTexture(CResMgr::GetInst()->FindRes<CTexture>(L"texture\\skybox\\2k_stars_milky_way.jpg"));

	SpawnGameObject(pSkyBox, Vec3(0.f, 0.f, 0.f), 0);

	//CGameObject* pPLight = new CGameObject;
	//pPLight->SetName(L"PointLight1");
	//pPLight->AddComponent(new CTransform);
	//pPLight->AddComponent(new CLight3D);

	//pPLight->Light3D()->SetLightType(LIGHT_TYPE::SPOT);
	//pPLight->Light3D()->SetLightColor(Vec3(1.f, 1.f, 1.f));
	//pPLight->Light3D()->SetLightAmbient(Vec3(0.15f, 0.15f, 0.15f));
	//pPLight->Light3D()->SetRadius(1000.f);
	//pPLight->Light3D()->SetAngle(XM_PI / 8.f);

	//pPLight->Transform()->SetRelativeRot(Vec3(0.f, XM_PI / 2.f, 0.f));

	//SpawnGameObject(pPLight, Vec3(10.f, 0.f, 490.f), L"Light");

	//CGameObject* pPLight2 = new CGameObject;
	//pPLight2->SetName(L"PointLight2");
	//pPLight2->AddComponent(new CTransform);
	//pPLight2->AddComponent(new CLight3D);

	//pPLight2->Light3D()->SetLightType(LIGHT_TYPE::POINT);
	//pPLight2->Light3D()->SetLightColor(Vec3(1.f, 1.f, 0.f));
	//pPLight2->Light3D()->SetLightAmbient(Vec3(0.15f, 0.15f, 0.15f));
	//pPLight2->Light3D()->SetRadius(1000.f);
	//pPLight2->Light3D()->SetAngle(XM_PI);

	//pPLight2->Transform()->SetRelativeRot(Vec3(0.f, XM_PI, 0.f));

	//SpawnGameObject(pPLight2, Vec3(0.f, 10.f, 490.f), L"Light");

	//CGameObject* pPLight3 = new CGameObject;
	//pPLight3->SetName(L"PointLight3");
	//pPLight3->AddComponent(new CTransform);
	//pPLight3->AddComponent(new CLight3D);

	//pPLight3->Light3D()->SetLightType(LIGHT_TYPE::SPOT);
	//pPLight3->Light3D()->SetLightColor(Vec3(1.f, 0.f, 1.f));
	//pPLight3->Light3D()->SetLightAmbient(Vec3(0.15f, 0.15f, 0.15f));
	//pPLight3->Light3D()->SetRadius(2000.f);
	//pPLight3->Light3D()->SetAngle(XM_PI);

	//pPLight3->Transform()->SetRelativeRot(Vec3(0.f, XM_PI * 2.f, 0.f));

	//SpawnGameObject(pPLight3, Vec3(-10.f, 0.f, 490.f), L"Light");

	//// 오브젝트 생성
	//CGameObject* pRect = new CGameObject;
	//pRect->SetName(L"Rect");
	//pRect->AddComponent(new CTransform);
	//pRect->AddComponent(new CMeshRender);
	//pRect->AddComponent(new CPlayerScript);

	//pRect->Transform()->SetRelativeScale(Vec3(2000.f, 2000.f, 200.f));

	//pRect->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	//pRect->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Std3DShaderMtrl"));

	//SpawnGameObject(pRect, Vec3(-120.f, 200.f, 500), L"Player");

	// 오브젝트 생성
	//CGameObject* pSphere = new CGameObject;
	//pSphere->SetName(L"Sphere");
	//pSphere->AddComponent(new CTransform);
	//pSphere->AddComponent(new CMeshRender);
	//pSphere->AddComponent(new CPlayerScript);

	//pSphere->Transform()->SetRelativeScale(Vec3(200.f, 200.f, 200.f));

	//pSphere->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"SphereMesh"));
	//pSphere->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Std3DShaderMtrl"));

	//SpawnGameObject(pSphere, Vec3(0.f, 0.f, 200.f), L"Player");

	//CGameObject* pCube = new CGameObject;
	//pCube->SetName(L"Sphere");
	//pCube->AddComponent(new CTransform);
	//pCube->AddComponent(new CMeshRender);

	//pCube->Transform()->SetRelativeScale(Vec3(200.f, 200.f, 200.f));

	//pCube->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"SphereMesh"));
	//pCube->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Std3D_DeferredMtrl"));
	//pCube->MeshRender()->GetMaterial()->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"texture\\tile\\TILE_01.tga"));
	//pCube->MeshRender()->GetMaterial()->SetTexParam(TEX_1, CResMgr::GetInst()->FindRes<CTexture>(L"texture\\tile\\TILE_01_N.tga"));

	//SpawnGameObject(pCube, Vec3(0.f, 0.f, 0.f), L"Player");

	//CGameObject* pRectMesh = new CGameObject;
	//pRectMesh->SetName(L"Rect");
	//pRectMesh->AddComponent(new CTransform);
	//pRectMesh->AddComponent(new CMeshRender);

	//pRectMesh->Transform()->SetRelativeScale(Vec3(1000.f, 1000.f, 1000.f));
	//pRectMesh->Transform()->SetRelativeRot(Vec3(XM_PI / 2.f, 0.f, 0.f));

	//pRectMesh->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	//pRectMesh->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Std3D_DeferredMtrl"));
	//pRectMesh->MeshRender()->GetMaterial()->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"texture\\tile\\TILE_01.tga"));
	//pRectMesh->MeshRender()->GetMaterial()->SetTexParam(TEX_1, CResMgr::GetInst()->FindRes<CTexture>(L"texture\\tile\\TILE_01_N.tga"));

	//SpawnGameObject(pRectMesh, Vec3(0.f, -500.f, 0.f), L"Player");

	//CGameObject* pObject = new CGameObject;
	//pObject->SetName(L"Decal");
	//pObject->AddComponent(new CTransform);
	//pObject->AddComponent(new CDecal);
	//pObject->Decal()->SetDeferredDecal(true);
	////pObject->Decal()->SetEmissiveDecal(true);

	//pObject->Transform()->SetRelativeScale(Vec3(200.f, 200.f, 200.f));
	//pObject->Transform()->SetRelativeRot(Vec3(0.f, 0.f, 0.f));

	//SpawnGameObject(pObject, Vec3(0.f, 0.f, 200.f), L"Default");
	
	// 충돌 시킬 레이어 짝 지정
	//CCollisionMgr::GetInst()->LayerCheck(L"Player", L"Monster");	
}
