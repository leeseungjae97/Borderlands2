#pragma once
#include "CRes.h"

#include "ptr.h"

class CMesh;
class CMaterial;
class CMeshData :
    public CRes
{
private:
    Ptr<CMesh>              m_pMesh;
    vector<Ptr<CMaterial>>  m_vecMtrl;

public:
    static CMeshData* LoadFromFBX(const wstring& _strFilePath);

    virtual int Save(const wstring&) override;
    virtual int Load(const wstring& _strFilePath) override;

    CGameObject* Instantiate();

    CLONE_DISABLE(CMeshData);

public:
    CMeshData(bool _bEngine);
    ~CMeshData();
};

