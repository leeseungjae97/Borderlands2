#pragma once

#ifndef _LEVEL
#define _LEVEL
#include "CEntity.h"

#include "physx_util.h"

class CLayer;
class CGameObject;

using namespace physx;
using namespace physx::Util;

class CLevel :
    public CEntity
{
private:
    CLayer*                     m_arrLayer[MAX_LAYER];
    PxScene*                    m_PxScene;

    LEVEL_STATE                 m_State;

    class PxCollisionCallBack*  m_PxCollisionCallBack;
    //class TriggersFilterCallback* m_PxTriggerCallBack;
    PxRigidStatic*              box;
public:
    void begin();
    void tick();
    void finaltick(); 

    void createScene();
public:
    CLayer* FindLayerByName(const wstring& _strName);
    CLayer* GetLayer(int _iLayerIdx) { assert(!(_iLayerIdx < 0)); return m_arrLayer[_iLayerIdx]; }

    CGameObject* FindObjectByName(const wstring& _Name);
    void FindObjectByName(const wstring& _Name, vector<CGameObject*>& _Out);

    void AddGameObject(CGameObject* _Object, int _iLayerIdx, bool _Move);
    void AddGameObject(CGameObject* _Object, const wstring& _LayerName, bool _Move);
    void AddCollider3D(CGameObject* _Object);
    void ChangeState(LEVEL_STATE _State);
    LEVEL_STATE GetState() { return m_State; }
    PxScene* GetScene() { return m_PxScene; }

private:
    // 등록된 GameObject 제거
    void clear();

    //CLONE_DISABLE(CLevel);
    CLONE(CLevel);
public:
    CLevel();
    ~CLevel();

    friend class CLevelMgr;
};

#endif