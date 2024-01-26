#pragma once
#include "CComponent.h"
using namespace physx;

class CRigidBody :
    public CComponent
{
private:
	PxShape*        m_PxColliderShape;
    PxMaterial*     m_PxMaterial;

    PxRigidActor*   m_RigidBody;

    PxReal          m_fColliderX;
    PxReal          m_fColliderY;
    PxReal          m_fColliderZ;

private:
    void addToScene();

    void setOwnerPosToPx();
    void rigidDebugDraw();

public:
    PxShape* MPxColliderShape() const { return m_PxColliderShape; }
    PxMaterial* MPxMaterial() const { return m_PxMaterial; }
    PxRigidActor* MRigidBody() const { return m_RigidBody; }
    PxReal MFColliderX() const { return m_fColliderX; }
    PxReal MFColliderY() const { return m_fColliderY; }
    PxReal MFColliderZ() const { return m_fColliderZ; }

    void MPxColliderShape(PxShape* m_px_collider_shape) { m_PxColliderShape = m_px_collider_shape; }
    void MPxMaterial(PxMaterial* m_px_material) { m_PxMaterial = m_px_material; }
    void MRigidBody(PxRigidActor* m_rigid_body) { m_RigidBody = m_rigid_body; }
    void MFColliderX(PxReal m_f_collider_x) { m_fColliderX = m_f_collider_x; }
    void MFColliderY(PxReal m_f_collider_y) { m_fColliderY = m_f_collider_y; }
    void MFColliderZ(PxReal m_f_collider_z) { m_fColliderZ = m_f_collider_z; }
public:
    void finaltick() override;

    void LoadFromLevelFile(FILE* _FILE) override;
    void SaveToLevelFile(FILE* _File) override;

public:
    CLONE(CRigidBody);
public:
    CRigidBody();
    virtual ~CRigidBody();

};

