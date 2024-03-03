#pragma once
#include "CSingleton.h"
#include "ptr.h"

class CParticleUpdateShader;
class CStructuredBuffer;
class CMaterial;
class CMesh;

class ParticleMgr
	: public CSingleton<ParticleMgr>
{
public:
	enum class PARTICLE_SETTING_TYPE
	{
		BULLET_IMPACT,
		END,
	};
private:
	tParticleModule m_ModuleData[(UINT)PARTICLE_SETTING_TYPE::END];

public:
	void DoParticle(CGameObject* _Obj, PARTICLE_SETTING_TYPE _Type, Vec3 _vPos);

public:
	void init();

public:
	ParticleMgr();
	virtual ~ParticleMgr();
};

