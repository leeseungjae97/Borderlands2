#pragma once
#include <Engine\CScript.h>
class FireBreathScript :
    public CScript
{
public:
    void begin() override;
    void Raycast(tRayInfo _RaycastInfo) override;
    void BeginOverlap(CCollider3D* _Other) override;
    void tick() override;
public:
    FireBreathScript();
    virtual ~FireBreathScript();
};

