#include "pch.h"
#include "PhysXMgr.h"

#include "PxPhysicsAPI.h"
#include <ctype.h>

#include "CLevel.h"
#include "CLevelMgr.h"
#include "physx_util.h"

#define PVD_HOST "127.0.0.1"

PhysXMgr::PhysXMgr()
	: gFoundation(NULL)
	, gPhysics(NULL)
	, gDispatcher(NULL)
	, gPvd(NULL)
	, gCurScene(NULL)
	, mFoundation(NULL)
{
}

PhysXMgr::~PhysXMgr()
{
	cleanupPhysics();
}

void PhysXMgr::init()
{
	gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);

	gPvd = PxCreatePvd(*gFoundation);
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
	gPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

	gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), true, gPvd);
	gDispatcher = PxDefaultCpuDispatcherCreate(2);
}

void PhysXMgr::tick()
{
	CLevel* level = CLevelMgr::GetInst()->GetCurLevel();
	if(level)
	{
		gCurScene = level->GetScene();
	}

	if(gCurScene)
	{
		gCurScene->simulate(1.0f / 60.0f);
		gCurScene->fetchResults(true);
	}
}

void PhysXMgr::render()
{

}

void PhysXMgr::InsertScene(CLevel* _Level)
{
	gScenes.insert(make_pair(_Level, _Level->GetScene()));
}

void PhysXMgr::CreateScene(Vec3 vPos)
{
	//PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
	//sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
	//sceneDesc.cpuDispatcher = gDispatcher;
	//sceneDesc.filterShader = PxDefaultSimulationFilterShader;
	//sceneDesc.simulationEventCallback;
	//gScene = gPhysics->createScene(sceneDesc);
	//PxPvdSceneClient* pvdClient = gScene->getScenePvdClient();
	//if (pvdClient)
	//{
	//	pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
	//	pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
	//	pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	//}


	//PxMaterial* m_PxMaterial = PhysXMgr::GetInst()->GPhysics()->createMaterial(0.5f, 0.5f, 0.6f);

	//m_PxMaterial->setFrictionCombineMode(static_cast<physx::PxCombineMode::Enum>(physx::PxCombineMode::eAVERAGE));
	//m_PxMaterial->setRestitutionCombineMode(static_cast<physx::PxCombineMode::Enum>(physx::PxCombineMode::eAVERAGE));

	//box = gPhysics->createRigidStatic(PxTransform(PxVec3(-200, 0, -200)));
	//PxShape* shape = createTriggerShape(physx::PxBoxGeometry(20000.f, 10.f, 20000.f), *m_PxMaterial, Util::FILTER_SHADER, true);
	////PxShape* shape = gPhysics->createShape(physx::PxBoxGeometry(20000.f, 10.f, 20000.f), *m_PxMaterial, true);
	////TriggerImpl 
	//box->attachShape(*shape);
	////PxRigidStatic* groundPlane = PxCreatePlane(*gPhysics, PxPlane(0, -1, 0, 0), *gMaterial);
	//gScene->addActor(*box);

}

void PhysXMgr::cleanupPhysics()
{
	for(auto pair : gScenes)
	{
		PX_RELEASE(pair.second);
	}
	
	PX_RELEASE(gDispatcher);
	PX_RELEASE(gPhysics);
	if (gPvd)
	{
		PxPvdTransport* transport = gPvd->getTransport();
		gPvd->release();	gPvd = NULL;
		PX_RELEASE(transport);
	}
	PX_RELEASE(gFoundation);
}
