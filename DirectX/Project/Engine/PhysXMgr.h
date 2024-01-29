#pragma once

#include "CSingleton.h"

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
	map<CLevel*, PxScene*>	gScenes;
	PxScene*				gCurScene;
	PxPvd*					gPvd;
	PxFoundation*			mFoundation;
	PxCudaContextManager*	gCudaContextManager;
	PxSceneDesc				gSceneDesc;
	PxCookingParams			gCookingParams;
public:
	void init();
	void tick();
	void render();

private:
	void cleanupPhysics();

public:
	void CreateScene(Vec3 vPos);
	void SetCurScene(PxScene* px_scene) { gCurScene = px_scene; }

public:
	PxDefaultAllocator GAllocator() const { return gAllocator; }
	PxDefaultErrorCallback GErrorCallback() const { return gErrorCallback; }
	PxFoundation* GFoundation() const { return gFoundation; }
	PxPhysics* GPhysics() const { return gPhysics; }
	PxDefaultCpuDispatcher* GDispatcher() const { return gDispatcher; }
	map<CLevel*, PxScene*> GScenes() const { return gScenes; }
	PxPvd* GPvd() const { return gPvd; }
	PxFoundation* MFoundation() const { return mFoundation; }
	PxScene* GCurScene();
	PxSceneDesc& GSceneDesc() { return gSceneDesc; }
	PxCookingParams& GCookingParams() { return gCookingParams; }

	void GAllocator(const PxDefaultAllocator& g_allocator) { gAllocator = g_allocator; }
	void GErrorCallback(const PxDefaultErrorCallback& g_error_callback) { gErrorCallback = g_error_callback; }
	void GFoundation(PxFoundation* g_foundation) { gFoundation = g_foundation; }
	void GPhysics(PxPhysics* g_physics) { gPhysics = g_physics; }
	void GDispatcher(PxDefaultCpuDispatcher* g_dispatcher) { gDispatcher = g_dispatcher; }
	void GScenes(const map<CLevel*, PxScene*>& g_scenes) { gScenes = g_scenes; }
	void GPvd(PxPvd* g_pvd) { gPvd = g_pvd; }
	void MFoundation(PxFoundation* m_foundation) { mFoundation = m_foundation; }

	void InsertScene(CLevel* _Level);

public:
	PhysXMgr();
	~PhysXMgr();
};

