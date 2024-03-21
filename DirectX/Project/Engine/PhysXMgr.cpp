#include "pch.h"
#include "PhysXMgr.h"

//#include "PxPhysicsAPI.h"

#include "CLevel.h"
#include "CLevelMgr.h"
#include "CTimeMgr.h"
#include "physx_util.h"

#define PVD_HOST "127.0.0.1"

PhysXMgr::PhysXMgr()
	: gFoundation(NULL)
	, gPhysics(NULL)
	, gDispatcher(NULL)
	, gPvd(NULL)
	, gCurScene(NULL)
	, mFoundation(NULL)
	, gCudaContextManager(nullptr)
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

	PxTolerancesScale scale;
	gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, scale, true, gPvd);

	gDispatcher = PxDefaultCpuDispatcherCreate(2);
}

void PhysXMgr::fixedTick()
{
	GCurScene();
	if(CLevelMgr::GetInst()->GetCurLevel()->GetState() == LEVEL_STATE::PLAY
		&& CLevelMgr::GetInst()->GetCurLevel()->GetTickCnt() > 10)
	{
		if (gCurScene)
		{
			//gCurScene->simulate(1.0f / 60.0f);
			gCurScene->simulate(DT);
			gCurScene->fetchResults(true);
		}
	}

}

void PhysXMgr::render()
{

}

PxScene* PhysXMgr::GCurScene()
{
	CLevel* level = CLevelMgr::GetInst()->GetCurLevel();
	auto scene = gScenes.find(level);
	gCurScene = scene->second;

	return gCurScene;
}

void PhysXMgr::InsertScene(CLevel* _Level)
{
	gScenes.insert(make_pair(_Level, _Level->GetScene()));
}

void PhysXMgr::CreateScene(Vec3 vPos)
{

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
