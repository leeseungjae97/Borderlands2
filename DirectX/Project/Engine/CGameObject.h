#pragma once
#include "CEntity.h"

class CComponent;
class CTransform;
class CMeshRender;
class CParticleSystem;
class CAnimator3D;
class CLight3D;
class CCamera;
class CRenderComponent;
class CScript;
class CSkyBox;
class CDecal;
class CPathFind;
class CCollider3D;
class CRigidBody;
class CGizmo;
class CLandScape;

#define GET_COMPONENT(Type, TYPE) C##Type* Type() const { return (C##Type*)m_arrCom[(UINT)COMPONENT_TYPE::TYPE]; }

class CGameObject :
    public CEntity
{
private:
    CComponent*             m_arrCom[(UINT)COMPONENT_TYPE::END];
    CRenderComponent*       m_RenderCom;
    vector<CScript*>        m_vecScript;

    CGameObject*            m_Parent;
    CGameObject*            m_Follow;

    vector<CGameObject*>    m_vecChild;

    int                     m_iLayerIdx; // 소속된 레이어 인덱스값
    bool                    m_bDead;
    float                   m_LifeTime;
    float                   m_CurLifeTime;
    bool                    m_bLifeSpan;

    bool                    m_bESM;
    bool                    m_bPreLoading;

    bool                    m_bItem;
public:
    // 레벨이 시작될 때 호출 or 시작 된 레벨에 합류할 때 호출
    // 생성자
    void begin();       

    void tick();        
    virtual void finaltick();
    void finaltick_module();
    void render();
    void render_shadowmap();

public:
    void AddComponent(CComponent* _Component);
    void AddChild(CGameObject* _Object);

    CComponent* GetComponent(COMPONENT_TYPE _ComType) { return m_arrCom[(UINT)_ComType]; }
    const vector<CGameObject*>& GetChild() { return m_vecChild; }

    CGameObject* GetParent() const { return m_Parent; }
    
    GET_COMPONENT(Transform, TRANSFORM);

    GET_COMPONENT(MeshRender, MESHRENDER);
    GET_COMPONENT(ParticleSystem, PARTICLESYSTEM);
    GET_COMPONENT(Camera, CAMERA);
    GET_COMPONENT(Gizmo, GIZMO);
    GET_COMPONENT(Light3D, LIGHT3D);
    GET_COMPONENT(Animator3D, ANIMATOR3D);
    GET_COMPONENT(SkyBox, SKYBOX);
    GET_COMPONENT(Decal, DECAL);
    GET_COMPONENT(LandScape, LANDSCAPE);
    GET_COMPONENT(Collider3D, COLLIDER3D);
    GET_COMPONENT(RigidBody, RIGIDBODY);
    GET_COMPONENT(PathFind, PATHFIND);
    //Decal* Decal()
    //{
    //    return (Decal*)m_arrCom[(UINT)COMPONENT_TYPE::TRANSFORM];
    //}

    CRenderComponent* GetRenderComponent() const {  return m_RenderCom; }


    int GetLayerIndex() { return m_iLayerIdx; }

    template<typename T>
    T* GetScript();

    const vector<CScript*>& GetScripts() { return m_vecScript; }

    void SetLifeSpan(float _fTime)
    {
        if (m_bLifeSpan)
            return;

        m_LifeTime = _fTime;
        m_bLifeSpan = true;
    }

    bool IsDead() { return m_bDead; }
    bool IsAncestor(CGameObject* _Target);
    void SetFollowObj(CGameObject* _Target);

    CGameObject* GetFollowObj() { return m_Follow; }

    void SetESM(bool _bESM) { m_bESM = _bESM; }

    void SetPreLoadingObject(bool _PreLoading) { m_bPreLoading = _PreLoading; }
    bool IsPreLoadingObject();

    void SetIsItem(bool _IsItem) { m_bItem = _IsItem; }
    bool IsItem() { return m_bItem; }
private:
    void DisconnectFromParent();
    void ChangeToChildType();
    void AddParentList();


    CLONE(CGameObject)    
public:
    CGameObject();
    CGameObject(const CGameObject& _Other);
    ~CGameObject();

    friend class CLayer;
    friend class CEventMgr;
};

template<typename T>
inline T* CGameObject::GetScript()
{
    for (size_t i = 0; i < m_vecScript.size(); ++i)
    {
        T* pScript = dynamic_cast<T*> (m_vecScript[i]);
        if (nullptr != pScript)
            return pScript;
    }

    return nullptr;
}
