#pragma once
#include <Engine\CScript.h>
class CPathFindScript :
    public CScript
{
private:
    vector<Vec3> m_vecQueryPath;

    Vec3 vDestPos;
    Vec3 vPrevPos;
    float fDestDist;

public:
    void DoQuery();

public:
    void LoadFromLevelFile(FILE* _FILE) override;
    void SaveToLevelFile(FILE* _File) override;
    void begin() override;
    void finaltick() override;
    void BeginOverlap(CCollider3D* _Other) override;
    void tick() override;
    void Move();
public:
    CLONE(CPathFindScript);

public:
    CPathFindScript();
    virtual ~CPathFindScript();
};

