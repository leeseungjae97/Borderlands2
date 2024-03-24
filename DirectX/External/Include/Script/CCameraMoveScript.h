#pragma once
#include <Engine\CScript.h>

class CCameraMoveScript :
    public CScript
{
private:
    float       m_fCamSpeed;
    bool        m_bMove;
    Vec4        m_vTargetPos;

public:
    void mainMenuCameraMove();
    void scopeCameraMove();

    void warriorCameraMove();
public:
    virtual void tick() override;
    virtual void finaltick() override;

private:
    void cameraDebugMove();
    void cameraFollowMove();
    CLONE(CCameraMoveScript);
public:
    CCameraMoveScript();
    ~CCameraMoveScript();
};

