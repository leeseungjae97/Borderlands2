#pragma once
#include "CComponent.h"

using namespace physx;

enum class RIGID_BODY_SHAPE_TYPE
{
    BOX,
    SPHERE,
    RECT,
    MESH,
    END,
};

enum class RIGID_BODY_TYPE
{
	STATIC,
    DYNAMIC,
    END,
};

static const char* RIGID_BODY_STR[(UINT)RIGID_BODY_TYPE::END]
{
    "STATIC",
    "DYNAMIC"
};

static const char* RIGID_BODY_SHAPE_STR[(UINT)RIGID_BODY_SHAPE_TYPE::END]
{
    "BOX",
    "SPHERE",
    "RECT",
    "MESH",
};

class CRigidBody :
    public CComponent
{
private:
	PxRigidDynamic*         m_pDynamicBody;
	PxRigidStatic*          m_pStaticBody;

    PxShape*                m_pShape;
	PxMaterial*             m_pMaterial;

    vector<PxVec3>          m_vecVerts;
    vector<PxU32>           m_vecIndis;

    wstring                 m_debugMeshName;

    RIGID_BODY_SHAPE_TYPE   m_tRigidShapeType;
    RIGID_BODY_TYPE         m_tRigidType;
    Vec3                    m_vRigidScale;

    bool                    m_bCreature;

    bool                    m_bInit;
    bool                    m_bInitData;

public:
	PxRigidDynamic*         GetDynamicBody()        const { return m_pDynamicBody; }
	PxRigidStatic*          GetStaticBody()         const { return m_pStaticBody; }

	PxShape*                GetShape()              const { return m_pShape; }
	PxMaterial*             GetMaterial()           const { return m_pMaterial; }

	vector<PxVec3>          GetVecVerts()           const { return m_vecVerts; }
	vector<PxU32>           GetVecIndis()           const { return m_vecIndis; }

	wstring                 GetebugMeshName()       const { return m_debugMeshName; }

	RIGID_BODY_SHAPE_TYPE   GetRigidShapeType()     const { return m_tRigidShapeType; }
	RIGID_BODY_TYPE         GetRigidType()          const { return m_tRigidType; }
	Vec3                    GetRigidScale()         const { return m_vRigidScale; }

    bool                    IsCreature()            const { return m_bCreature; }

    PxTransform             GetRigidBodyPos();

	void SetDynamicBody     (PxRigidDynamic* m_p_dynamic_body)              { m_pDynamicBody = m_p_dynamic_body; }
	void SetStaticBody      (PxRigidStatic* m_p_static_body)                { m_pStaticBody = m_p_static_body; }

	void SetShape           (PxShape* m_p_shape)                            { m_pShape = m_p_shape; }
	void SetMaterial        (PxMaterial* m_p_material)                      { m_pMaterial = m_p_material; }

	void SetVecVerts        (const vector<PxVec3>& m_vec_verts)             { m_vecVerts = m_vec_verts; }
	void SetVecIndis        (const vector<PxU32>& m_vec_indis)              { m_vecIndis = m_vec_indis; }

	void SetDebugMeshName   (const wstring& m_debug_mesh_name)              { m_debugMeshName = m_debug_mesh_name; }

	void SetRigidShapeType  (RIGID_BODY_SHAPE_TYPE m_t_rigid_shape_type)    { m_tRigidShapeType = m_t_rigid_shape_type; }
	void SetRigidType       (RIGID_BODY_TYPE m_t_rigid_type)                { m_tRigidType = m_t_rigid_type; }
	void SetRigidScale      (const Vec3& m_v_rigid_scale)                   { m_vRigidScale = m_v_rigid_scale; }

    void SetCreature        (bool _bCreature)                               { m_bCreature = _bCreature; }

    void AttachShape(PxShape* _Attach);
	void SetRigidBodyTrans(const PxTransform& trans);

    void SetLinearVelocity(Vec3 _Velocity);
    void SetLinearVelocityZero();

    void SetAngularVelocity(Vec3 _Angular);
    void SetAngularVelocityZero();

	bool IsRigidBodyCreate();

    Matrix GetRigidBodyMatrix();

private:
    void convertMeshToGeom();
    void setRigidPos();
    void createShape();
    void createTriangleMesh();
    void addToScene();
    void setUserData();
    void drawDebugRigid();

public:
    void finaltick() override;

    void LoadFromLevelFile(FILE* _FILE) override;
    void SaveToLevelFile(FILE* _File) override;

    CLONE(CRigidBody)
public:
    // CLONE ABLE용 생성자, ImGui사용할 때 기본 생성자
    //CRigidBody();
    CRigidBody(RIGID_BODY_SHAPE_TYPE _Type = RIGID_BODY_SHAPE_TYPE::BOX
        , RIGID_BODY_TYPE _Type2 = RIGID_BODY_TYPE::STATIC);
    virtual ~CRigidBody();
};

