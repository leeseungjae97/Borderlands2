#pragma once
#include "CRenderComponent.h"

class CParticleUpdateShader;
class CStructuredBuffer;

class ParticleSystem :
    public CRenderComponent
{
private:
    CStructuredBuffer* m_ParticleBuffer;
    CStructuredBuffer* m_RWBuffer;
    CStructuredBuffer* m_ModuleDataBuffer;

    tParticleModule             m_ModuleData;
    Ptr<CParticleUpdateShader>  m_UpdateCS;

    float                       m_AccTime;

public:
    virtual void finaltick() override;
    virtual void render() override;

    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;

    CLONE(ParticleSystem);

public:
    ParticleSystem();
    virtual ~ParticleSystem();


};

