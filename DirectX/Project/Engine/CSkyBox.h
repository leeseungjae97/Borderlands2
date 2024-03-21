#pragma once
#include "CRenderComponent.h"

enum class SKYBOX_TYPE
{
    SPHERE,
    CUBE,
    HEMI_SPHERE,
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
    virtual void render(UINT _iSubset, bool _Deferred) override;
    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _FILE) override;

    SKYBOX_TYPE GetSkyBoxType() { return m_Type; }
    Ptr<CTexture> GetSkyBoxTex() { return m_SkyBoxTex; }
    Ptr<CMaterial> GetSkyBoxMtrl() { return m_SkyBoxMtrl; }

    CLONE(CSkyBox)
private:
    SKYBOX_TYPE     m_Type;
    Ptr<CTexture>   m_SkyBoxTex;
    Ptr<CMaterial>  m_SkyBoxMtrl;

    bool            m_bFbxSkyBox;

public:
    CSkyBox();
    CSkyBox(bool _FromMeshData);
    ~CSkyBox();
};

