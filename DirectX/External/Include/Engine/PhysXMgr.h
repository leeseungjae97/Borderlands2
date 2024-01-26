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
	map<wstring, PxScene*>	gScenes;
	PxScene*				gScene;
	PxMaterial*				gMaterial;
	PxPvd*					gPvd;
	PxFoundation*			mFoundation;

	PxRigidStatic* box;

public:
	void init();
	void tick();
	void render();

private:
	void cleanupPhysics();

public:
	void CreateScene(Vec3 vPos);

public:
	PxDefaultAllocator GAllocator() const { return gAllocator; }
	PxDefaultErrorCallback GErrorCallback() const { return gErrorCallback; }
	PxFoundation* GFoundation() const { return gFoundation; }
	PxPhysics* GPhysics() const { return gPhysics; }
	PxDefaultCpuDispatcher* GDispatcher() const { return gDispatcher; }
	map<wstring, PxScene*> GScenes() const { return gScenes; }
	PxScene* GScene() const { return gScene; }
	PxPvd* GPvd() const { return gPvd; }
	PxFoundation* MFoundation() const { return mFoundation; }

	void GAllocator(const PxDefaultAllocator& g_allocator) { gAllocator = g_allocator; }
	void GErrorCallback(const PxDefaultErrorCallback& g_error_callback) { gErrorCallback = g_error_callback; }
	void GFoundation(PxFoundation* g_foundation) { gFoundation = g_foundation; }
	void GPhysics(PxPhysics* g_physics) { gPhysics = g_physics; }
	void GDispatcher(PxDefaultCpuDispatcher* g_dispatcher) { gDispatcher = g_dispatcher; }
	void GScenes(const map<wstring, PxScene*>& g_scenes) { gScenes = g_scenes; }
	void GScene(PxScene* g_scene) { gScene = g_scene; }
	void GPvd(PxPvd* g_pvd) { gPvd = g_pvd; }
	void MFoundation(PxFoundation* m_foundation) { mFoundation = m_foundation; }

public:
	PhysXMgr();
	~PhysXMgr();
};

