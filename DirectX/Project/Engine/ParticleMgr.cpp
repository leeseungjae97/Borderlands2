#include "pch.h"
#include "ParticleMgr.h"

#include "CParticleUpdateShader.h"
#include "CResMgr.h"
#include "CStructuredBuffer.h"

ParticleMgr::ParticleMgr()
{

}

ParticleMgr::~ParticleMgr()
{
}

void ParticleMgr::init()
{
	tParticleModule moduleData = m_ModuleData[(UINT)PARTICLE_SETTING_TYPE::BULLET_IMPACT];
	moduleData.ModuleCheck[(UINT)PARTICLE_MODULE::PARTICLE_SPAWN] = true;
	moduleData.ModuleCheck[(UINT)PARTICLE_MODULE::SCALE_CHANGE] = true;
	moduleData.ModuleCheck[(UINT)PARTICLE_MODULE::COLOR_CHANGE] = true;
	moduleData.ModuleCheck[(UINT)PARTICLE_MODULE::ADD_VELOCITY] = true;
	moduleData.ModuleCheck[(UINT)PARTICLE_MODULE::DRAG] = true;
	moduleData.ModuleCheck[(UINT)PARTICLE_MODULE::NOISE_FORCE] = false;
	moduleData.ModuleCheck[(UINT)PARTICLE_MODULE::RENDER] = true;

	moduleData.iMaxParticleCount = 10;

	moduleData.SpawnRate = 50;
	moduleData.vSpawnColor = Vec3(1.f, 1.f, 0.0f);
	moduleData.vSpawnScaleMin = Vec3(3.f, 1.f, 1.f);
	moduleData.vSpawnScaleMax = Vec3(5.f, 1.f, 1.f);
	moduleData.SpawnShapeType = 0;
	moduleData.vBoxShapeScale = Vec3(20.f, 20.f, 20.f);
	moduleData.Space = 0;
	moduleData.MinLifeTime = 1.f;
	moduleData.MaxLifeTime = 1.f;

	moduleData.StartScale = 1.0f;
	moduleData.EndScale = 0.0f;

	moduleData.vStartColor = Vec3(1.0f, 0.5f, 0.0f);
	moduleData.vEndColor = Vec3(1.0f, 1.0f, 0.0f);

	moduleData.AddVelocityType = 0;
	moduleData.Speed = 1000.f;
	moduleData.vVelocityDir = Vec3(0.f, 1.f, 0.f);

	moduleData.StartDrag = 50.f;
	moduleData.EndDrag = -50.f;

	moduleData.fNoiseTerm = 1.f;
	moduleData.fNoiseForce = 100.f;

	moduleData.VelocityAlignment = true;
	moduleData.VelocityScale = true;
	moduleData.vMaxVelocityScale = Vec3(15.f, 1.f, 1.f);
	moduleData.vMaxSpeed = 10000.f;

	m_ModuleData[(UINT)PARTICLE_SETTING_TYPE::BULLET_IMPACT] = moduleData;
}

void ParticleMgr::DoParticle(CGameObject* _Obj, PARTICLE_SETTING_TYPE _Type, Vec3 _vPos)
{
	CParticleSystem* pParticleSys= _Obj->ParticleSystem();
	if (nullptr == pParticleSys)
		return;

	pParticleSys->SetModuleData(m_ModuleData[(UINT)_Type]);
	pParticleSys->SetParticlePos(_vPos);
}
