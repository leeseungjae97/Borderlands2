#pragma once
#include "ComponentUI.h"
#include <Engine/ptr.h>
class MeshRenderUI :
    public ComponentUI
{

public:
    virtual int render_update() override;
    void GetPaperburn();
    virtual void SetTarget(CGameObject* targetObject) override;

public:
    void SelectMesh(DWORD_PTR _Key);
    void SelectMaterial(DWORD_PTR _Key);

    void SelectTexture(DWORD_PTR _Key);
    void SelectShader(DWORD_PTR _Key);

private:
    Ptr<class CMesh> pMesh;
    class CMeshRender* pMeshRender;
    const map<wstring, Ptr<CRes>>* mapMesh;
    const map<wstring, Ptr<CRes>>* pTexture;
    const map<wstring, Ptr<CRes>>* mapShader;
    const map<wstring, Ptr<CRes>>* mapMtrl;
public:
    MeshRenderUI();
    ~MeshRenderUI();
};

