#pragma once
#include "CComponent.h"

using namespace physx;

class CCollider3D :
    public CComponent
{
private:
    // PhysX 연산에 사용될 충돌체 모양
    PxShape*        m_PxColliderShape;
    PxRigidStatic*  m_PxColliderRigid;
    // PhysX 연산에 사용될 정보
    // 마찰 결합 방식, 탄성 결합 방식 결정
    // https://docs.unrealengine.com/4.26/ko/InteractiveExperiences/Physics/PhysicalMaterials/Reference/
    PxMaterial*     m_PxMaterial;

    //PxRigidStatic*   m_RigidCollider;

    Vec3            vScale;
    bool            m_bFirstInit;
    bool            m_bAttachToRigidBody;
    //PxBoxGeometry   m_PxGeometry;

private:
    void setShapeToRigidBody();
    void createColliderShape();
    void colliderDebugDraw();

public:
    PxShape* MPxColliderShape() { return m_PxColliderShape; }
    PxMaterial* MPxMaterial() const { return m_PxMaterial; }
    PxRigidStatic* MPxColliderRigid() const { return m_PxColliderRigid; }

    void MPxColliderShape(PxShape* m_px_collider_shape) { m_PxColliderShape = m_px_collider_shape; }
    void MPxMaterial(PxMaterial* m_px_material) { m_PxMaterial = m_px_material; }
    //void MRigidCollider(PxRigidStatic* m_rigid_collider) { m_RigidCollider = m_rigid_collider; }

public:
    void EndOverlap(CCollider3D* _OhterCol);
    void OnOverlap(CCollider3D* _OhterCol);
    void BeginOverlap(CCollider3D* _OhterCol);


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

