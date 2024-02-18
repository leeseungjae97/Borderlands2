#pragma once
#include "CRenderComponent.h"


class CMeshRender :
    public CRenderComponent
{
public:
    virtual void finaltick() override;
    virtual void render() override;

    void render(UINT _iSubset, bool _Deferred = true) override;

    CLONE(CMeshRender)
public:
    CMeshRender();
    ~CMeshRender();
};

