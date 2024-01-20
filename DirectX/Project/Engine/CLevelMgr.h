#pragma once
#include "CSingleton.h"

class CLevel;
class CGameObject;

class CLevelMgr :
    public CSingleton<CLevelMgr>
{   
    SINGLE(CLevelMgr);
private:
    map<wstring, CLevel*> m_mapLevels;

    CLevel*     m_pCurLevel;

public:
    CLevel* GetCurLevel() { return m_pCurLevel; }
    CGameObject* FindObjectByName(const wstring& _strName);
    void FindObjectByName(const wstring& _strName, vector<CGameObject*>& _Out);
    CLevel* CreateLevel(const wstring& name);
    CLevel* GetLevel(const wstring& name);
    void ChangeLevel(CLevel* _NextLevel);

public:
    void init();
    void tick();
};

