#pragma once

#include "global.h"

using namespace physx;

class PhysXMgr
	: public CSingleton<PhysXMgr>
{
	
private:
	PxDefaultAllocator		gAllocator;
	PxDefaultErrorCallback	gErrorCallback;
	PxFoundation*			gFoundation;
	PxPhysics*				gPhysics;
	PxDefaultCpuDispatcher* gDispatcher;
	PxScene*				gScene;
	PxMaterial*				gMaterial;
	PxPvd*					gPvd;
	PxFoundation*			mFoundation;

	PxReal stackZ = 10.0f;

private:
	void initPhysics();

public:
	void init();
	void cleanupPhysics();
	void TestPhysX();

public:
	PhysXMgr();
	~PhysXMgr();
};

