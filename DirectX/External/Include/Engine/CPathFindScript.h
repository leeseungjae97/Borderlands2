#pragma once
#include "CScript.h"
class CPathFindScript :
    public CScript
{
private:
    dtNavMeshQuery* m_navQuery;

public:
    void begin() override;
    void SaveToLevelFile(FILE* _File) override;
    void LoadFromLevelFile(FILE* _FILE) override;

public:
    CLONE(CPathFindScript);
public:
    CPathFindScript();
    virtual ~CPathFindScript();
};

