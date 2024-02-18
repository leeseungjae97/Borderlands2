#pragma once
#include "CComponent.h"

using namespace physx;

enum class COLLIDER_SHAPE_TYPE
{
	BOX,
    SPHERE,
};

class CCollider3D :
    public CComponent
{
private:
    PxShape*        m_PxColliderShape;
    //PxShape*        m_PxRangeShape;

    PxRigidStatic*  m_PxColliderRigid;

    PxMaterial*     m_PxMaterial;

    COLLIDER_SHAPE_TYPE  m_tColliderShapeType;

    Vec3            m_vScale;
    Vec3            m_vOffset;
    bool            m_bFirstInit;
    bool            m_bAttachToRigidBody;
    bool            m_bCenter;

private:
    void setShapeToRigidBody();
    void createColliderShape();
    void colliderDebugDraw();

public:
    PxShape* GetShape() { return m_PxColliderShape; }
    //PxShape* GetRange() { return m_PxColliderShape; }
    PxMaterial* GetMaterial() const { return m_PxMaterial; }
    PxRigidStatic* GetColliderRigid() const { return m_PxColliderRigid; }
    COLLIDER_SHAPE_TYPE GetColliderShape() { return m_tColliderShapeType; }

    //PxTransform GetColliderPos() { return m_PxColliderShape->Get}
    void SetColliderPos(Vec3 _vPos);
    void SetColliderRot(Vec3 _vRot);

    void SetShape(PxShape* m_px_collider_shape) { m_PxColliderShape = m_px_collider_shape; }
    void SetMaterial(PxMaterial* m_px_material) { m_PxMaterial = m_px_material; }
    void SetColliderShape(COLLIDER_SHAPE_TYPE _type) { m_tColliderShapeType = _type; }

    void SetScale(Vec3 _Scale) { m_vScale = _Scale; }
    Vec3 GetScale() { return m_vScale; }

    void SetOffset(Vec3 _Offset) { m_vOffset = _Offset; }
    Vec3 GetOffset() { return m_vOffset; }

    void SetAttachToRigidBody(bool _Attach) { m_bAttachToRigidBody = true; }
    bool GetAttachToRigidBody() { return m_bAttachToRigidBody; }

    void SetCenter(bool _Center) { m_bCenter = _Center; }
    bool GetCenter() { return m_bCenter; }

    PxTransform GetColliderPos() { return m_PxColliderRigid->getGlobalPose(); }

    bool IsColliderRigidCreate() { return m_PxColliderRigid; }
public:
    void EndOverlap(CCollider3D* _OhterCol);
    void OnOverlap(CCollider3D* _OhterCol);
    void BeginOverlap(CCollider3D* _OhterCol);
    void Raycast(tRayInfo _RaycastInfo);

public:
    virtual void finaltick() override;
    virtual void LoadFromLevelFile(FILE* _FILE) override;
    virtual void SaveToLevelFile(FILE* _File) override;

public:
    CLONE(CCollider3D);

public:
    CCollider3D(bool _AttachRigid = true);
    virtual ~CCollider3D();
};

