#pragma once
#include "CRenderComponent.h"

enum class SKYBOX_TYPE
{
    SPHERE,
    CUBE,
};
class CSkyBox :
    public CRenderComponent
{
public:
    void SetSkyBoxType(SKYBOX_TYPE _Type);
    void SetSkyBoxTexture(Ptr<CTexture> _Tex);

public:
    virtual void render() override;
    virtual void finaltick() override;

    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _FILE) override;
    CLONE(CSkyBox)
private:
    SKYBOX_TYPE     m_Type;
    Ptr<CTexture>   m_SkyBoxTex;

public:
    CSkyBox();
    ~CSkyBox();
};

