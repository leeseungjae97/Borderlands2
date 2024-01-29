#pragma once

#include <Engine\CGameObject.h>




class CGameObjectEx :
    public CGameObject
{
private:
    wstring wcustomMeshName;

public:
    void SetCustomMeshName(const wstring& name) { wcustomMeshName = name; }
    const wstring& GetCustomMeshName() { return wcustomMeshName; }

public:
    virtual void finaltick() override;
};

