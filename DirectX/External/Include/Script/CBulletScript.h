#pragma once
#include "E:\directx11_3d\DirectX\External\Include\Engine\CScript.h"
class CBulletScript :
    public CScript
{
private:
    float   m_fBulletSpeed;
    Vec3    m_vDir;
    float   m_fAlive;
    float   m_fAliveAcc;

public:
    void BeginOverlap(CCollider3D* _Other) override;
    void tick() override;

    CLONE(CBulletScript);
public:
    CBulletScript();
    ~CBulletScript();
};

