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

    wstring                 m_wsFbxName;

public:
    static CMeshData* LoadFromFBX(const wstring& _strFilePath);

    virtual int Save(const wstring& _strRelativePath) override;
    virtual int Load(const wstring& _strFilePath) override;

    CGameObject* Instantiate(Vec3 scale, bool _Sky = false);
    void Instantiate(CGameObject* _Obj);

    CLONE_DISABLE(CMeshData);

public:
    CMeshData(bool _bEngine = false);
    ~CMeshData();
};

