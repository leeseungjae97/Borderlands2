#include "pch.h"
#include "ParticleMgr.h"

#include "CGizmo.h"
#include "CParticleUpdateShader.h"
#include "CResMgr.h"
#include "CStructuredBuffer.h"
#include "CTransform.h"

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

	moduleData.iMaxParticleCount = 3000;
	moduleData.ModuleCheck[(UINT)PARTICLE_MODULE::PARTICLE_SPAWN] = true;
	moduleData.SpawnRate = 50;
	moduleData.vSpawnColor = Vec3(1.f, 1.f, 0.0f);
	moduleData.vSpawnScaleMin = Vec3(5.f, 2.f, 1.f);
	moduleData.vSpawnScaleMax = Vec3(10.f, 2.f, 1.f);

	moduleData.SpawnShapeType = 0;
	moduleData.vBoxShapeScale = Vec3(20.f, 20.f, 20.f);
	moduleData.Space = 1; // ½Ã¹Ä·¹ÀÌ¼Ç ÁÂÇ¥°è

	moduleData.MinLifeTime = 0.0f;
	moduleData.MaxLifeTime = 1.0f;

	moduleData.ModuleCheck[(UINT)PARTICLE_MODULE::SCALE_CHANGE] = true;
	moduleData.StartScale = 1.0f;
	moduleData.EndScale = 0.0f;

	moduleData.ModuleCheck[(UINT)PARTICLE_MODULE::COLOR_CHANGE] = true;
	moduleData.vStartColor = Vec3(1.0f, 0.5f, 0.0f);
	moduleData.vEndColor = Vec3(1.0f, 1.0f, 0.0f);

	moduleData.ModuleCheck[(UINT)PARTICLE_MODULE::ADD_VELOCITY] = true;
	moduleData.AddVelocityType = 0; // From Center
	moduleData.Speed = 100.f;
	moduleData.vVelocityDir = Vec3(0.f, 1.f, 0.f);
	moduleData.OffsetAngle;

	moduleData.ModuleCheck[(UINT)PARTICLE_MODULE::DRAG] = true;
	moduleData.StartDrag = 50.f;
	moduleData.EndDrag = -50.f;

	moduleData.ModuleCheck[(UINT)PARTICLE_MODULE::NOISE_FORCE] = false;
	moduleData.fNoiseTerm = 1.f;
	moduleData.fNoiseForce = 100.f;

	moduleData.ModuleCheck[(UINT)PARTICLE_MODULE::RENDER] = true;
	moduleData.VelocityAlignment = true;
	moduleData.VelocityScale = true;
	moduleData.vMaxVelocityScale = Vec3(15.f, 1.f, 1.f);
	moduleData.vMaxSpeed = 500.f;

	m_ModuleData[(UINT)PARTICLE_SETTING_TYPE::BULLET_IMPACT] = moduleData;
}

void ParticleMgr::begin()
{
	{
		m_pParticle = new CGameObject;
		m_pParticle->AddComponent(new CTransform);
		m_pParticle->AddComponent(new CParticleSystem);
		m_pParticle->SetName(L"Particle");
		m_pParticle->AddComponent(new CGizmo);


		m_pParticle->Transform()->SetRelativeScale(Vec3(1.f, 1.f, 1.f));
		SpawnGameObject(m_pParticle, Vec3(0.f, 0.f, 0.f), LAYER_TYPE::Environment);
	}
}

void ParticleMgr::DoParticle(PARTICLE_SETTING_TYPE _Type, Vec3 _vPos)
{
	CParticleSystem* pParticleSys= m_pParticle->ParticleSystem();
	if (nullptr == pParticleSys)
		return;

	pParticleSys->SetModuleData(m_ModuleData[(UINT)_Type]);
	pParticleSys->SetParticlePos(_vPos);
	pParticleSys->SetFire(true);
}
