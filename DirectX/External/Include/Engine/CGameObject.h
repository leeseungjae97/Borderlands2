#pragma once
#include "CEntity.h"

class CComponent;
class CTransform;
class CMeshRender;
class CParticleSystem;
class CAnimator3D;
class CAnimator2D;
class CLight3D;
class CCamera;
class CRenderComponent;
class CScript;
class CSkyBox;
class CDecal;
class CCollider3D;
class CRigidBody;
class CGizmo;
class CLandScape;

#define GET_COMPONENT(Type, TYPE) C##Type* Type() const { return (C##Type*)m_arrCom[(UINT)COMPONENT_TYPE::TYPE]; }

class CGameObject :
    public CEntity
{
public:
    enum class OBJECT_STATE
    {
	    VISIBLE,
        INVISIBLE,
        NONE,
    };

private:
    CComponent*             m_arrCom[(UINT)COMPONENT_TYPE::END];
    CRenderComponent*       m_RenderCom;
    vector<CScript*>        m_vecScript;

    CGameObject*            m_Parent;
    CGameObject*            m_Follow;
    
    vector<CGameObject*>    m_vecChild;

    int                     m_iLayerIdx;
    bool                    m_bDead;
    float                   m_LifeTime;
    float                   m_CurLifeTime;
    bool                    m_bLifeSpan;

    bool                    m_bESM;
    bool                    m_bPreLoading;

    bool                    m_bItem;
    bool                    m_bEqui;

    OBJECT_STATE            m_tState;

    CGameObject*            m_pWeaponOwner;
    vector<CGameObject*>    m_vecWeapons;
    bool                    m_bMelee;
    bool                    m_bOwned;

    bool                    m_bWarrior;

public:
    virtual void begin();

    void tick();        
    virtual void finaltick();
    void finaltick_module();
    virtual void render();
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
    GET_COMPONENT(Animator2D, ANIMATOR2D);
    GET_COMPONENT(SkyBox, SKYBOX);
    GET_COMPONENT(Decal, DECAL);
    GET_COMPONENT(LandScape, LANDSCAPE);
    GET_COMPONENT(Collider3D, COLLIDER3D);
    GET_COMPONENT(RigidBody, RIGIDBODY);

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
    void SetDead(bool _Dead) { m_bDead = _Dead; }
    bool IsAncestor(CGameObject* _Target);
    void SetFollowObj(CGameObject* _Target);

    CGameObject* GetFollowObj() { return m_Follow; }

    void SetESM(bool _bESM) { m_bESM = _bESM; }

    void SetPreLoadingObject(bool _PreLoading) { m_bPreLoading = _PreLoading; }
    bool IsPreLoadingObject();

    void SetIsItem(bool _IsItem) { m_bItem = _IsItem; }
    void SetIsEqui(bool _IsEqui) { m_bEqui = _IsEqui; }
    void SetIsOwned(bool _IsOwned) { m_bOwned = _IsOwned; }
    void SetIsWarrior(bool _IsWarrior) { m_bWarrior = _IsWarrior; }

    void SetObjectState(OBJECT_STATE _State) { m_tState = _State; }

    bool IsItem() { return m_bItem; }
    bool IsEqui() { return m_bEqui; }
    bool IsOwned() { return m_bOwned; }
    bool IsWarrior() { return m_bWarrior; }

    OBJECT_STATE GetObjectState() { return m_tState; }

    void SetWeaponOwner(CGameObject* _pOwner) { m_pWeaponOwner = _pOwner; }
    CGameObject*  GetWeaponOwner() { return m_pWeaponOwner; }

    void AddWeapon(CGameObject* _pWeapon, bool _Melee = false);
    void DeleteWeapon(CGameObject* _pWeapon);
    void SetMelee(bool _Melee) { m_bMelee = _Melee; }
    bool IsMelee() { return m_bMelee; }

    vector<CGameObject*> GetWeapons() { return m_vecWeapons; }
    CGameObject* GetWeapon(int _Idx) { if (m_vecWeapons.size() <= _Idx) return nullptr; return m_vecWeapons[_Idx]; }

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
