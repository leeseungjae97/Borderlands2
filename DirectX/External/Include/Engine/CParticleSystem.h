#pragma once
#include "CRenderComponent.h"

#include "ptr.h"
#include "CParticleUpdateShader.h"

class CStructuredBuffer;

class CParticleSystem :
    public CRenderComponent
{
private:
    CStructuredBuffer*          m_ParticleBuffer;
    CStructuredBuffer*          m_RWBuffer;
    CStructuredBuffer*          m_ModuleDataBuffer;
    
    tParticleModule             m_ModuleData;
    Ptr<CParticleUpdateShader>  m_UpdateCS;
   
    float                       m_AccTime;

public:
    void ActiveModule(PARTICLE_MODULE _ModuleType) { m_ModuleData.ModuleCheck[(UINT)_ModuleType] = true; }
    void DeactivateModule(PARTICLE_MODULE _ModuleType) { m_ModuleData.ModuleCheck[(UINT)_ModuleType] = false; }

	void SetMaxSpawn(int _MaxSpawn);
    void SetSpawnRate(int _SpawnRate) { m_ModuleData.SpawnRate = _SpawnRate; }
	void SetSpawnInitialColor(Vec3 _vInitialColor) { m_ModuleData.vSpawnColor = _vInitialColor; }
	void SetSpawnMinScale(Vec3 _SpawnScale) { m_ModuleData.vSpawnScaleMin = _SpawnScale; }
	void SetSpawnMaxScale(Vec3 _SpawnScale) { m_ModuleData.vSpawnScaleMax = _SpawnScale; }

	void SetShapeType(int _Type) { m_ModuleData.SpawnShapeType = _Type; }
	void SetBoxShapeScale(Vec3 _Scale) { m_ModuleData.vBoxShapeScale = _Scale; }

	void SetSpace(int _Branch) { m_ModuleData.Space = _Branch; }
	void SetMinLifeTime(float _Time) { m_ModuleData.MinLifeTime = _Time; }
	void SetMaxLifeTime(float _Time) { m_ModuleData.MaxLifeTime = _Time; }
	void SetStartScale(float _Scale) { m_ModuleData.StartScale = _Scale; }
	void SetEndScale(float _Scale) { m_ModuleData.EndScale = _Scale; }
	void SetStartColor(Vec3 _Color) { m_ModuleData.vStartColor= _Color; }
	void SetEndColor(Vec3 _Color) { m_ModuleData.vEndColor= _Color; }
	void SetVelocityType(int _Type) { m_ModuleData.AddVelocityType = _Type; }
	void SetSpeed(float _Speed) { m_ModuleData.Speed= _Speed; }
	void SetSpeed(Vec3 _Dir) { m_ModuleData.vVelocityDir = _Dir; }
	void SetStartDrag(float _Drag) { m_ModuleData.StartDrag= _Drag; }
	void SetEndDrag(float _Drag) { m_ModuleData.EndDrag= _Drag; }
	void SetNoiseTerm(float _Term) { m_ModuleData.fNoiseTerm= _Term; }
	void SetNoiseForce(float _Force) { m_ModuleData.fNoiseForce= _Force; }

	void ActiveVelocityScale() { m_ModuleData.VelocityScale = true; }
	void DeactiveVelocityScale() { m_ModuleData.VelocityScale = false; }

	void SetMaxVelocityScale(Vec3 _VelocityScale) { m_ModuleData.vMaxVelocityScale = _VelocityScale; }
	void SetMaxSpeed(float _Speed) { m_ModuleData.vMaxSpeed = _Speed; }

	void SetMaxScaleFator(float _ScaleFator) { m_ModuleData.MaxScaleFator= _ScaleFator; }
	void SetMinScaleFator(float _ScaleFator) { m_ModuleData.MinScaleFator= _ScaleFator; }

	void SetParticleIntervalDistance(Vec2 _Distance)
    {
        m_ModuleData.vFixedScale.x = _Distance.x;
        m_ModuleData.vFixedScale.y = _Distance.y;
    }

	Vec2 GetIntervalDistance() { return m_ModuleData.vFixedScale; }
	float GetMaxScaleFator() { return m_ModuleData.MaxScaleFator; }
	float GetMinScaleFator() { return m_ModuleData.MinScaleFator; }

public:
	void SetParticleTex(const Ptr<CTexture>& _Tex);
	void SetNoiseTex(const Ptr<CTexture>& _NoiseTex);

	void SetComputeShader(const Ptr<CComputeShader>& _ComputeShader);
	//void SetMaterial(CMaterial& _Mat);
public:
    virtual void finaltick() override;
    virtual void render() override;
	void render(UINT _iSubset) override;
    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;

    CLONE(CParticleSystem);
public:
    CParticleSystem();
    ~CParticleSystem();
};

