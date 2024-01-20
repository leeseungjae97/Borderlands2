#pragma once
#include "CEntity.h"

class CLevel;
class Scene :
    public CEntity
{
private:
    map<wstring, CLevel*>   m_vecLevels;
    wstring                 m_stringMainLevelName;
public:
    virtual void init();
    virtual void tick();
    virtual void finaltick();

    virtual void exit();
    virtual void enter();

public:
    CLevel* CreateLevel(const wstring& name, bool IsMain = false);
    CLevel* GetLevel(const wstring& name);
    void MainLvToCurLv();

    void ChangeLevel(const wstring& name);
    bool FindLevel(const wstring& name);
    int GetLevelSize() { return m_vecLevels.size(); }

public:
    CLONE_DISABLE(Scene);
public:
    Scene();
    ~Scene();
};

