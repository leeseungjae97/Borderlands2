#pragma once
#include <Engine\CScript.h>

class CCameraMoveScript :
    public CScript
{
private:
    float       m_fCamSpeed;
    bool        m_bMove;
    Vec4        m_vTargetPos;

    bool        m_bShake;
    int         m_iShakeLoop;
    int         m_iShake;
public:
    void mainMenuCameraMove();
    void scopeCameraMove();

    void warriorCameraMove();
    void shakeCameraMove();

    void SetShake(bool _bShake, int _iLoopCnt) { m_bShake = _bShake; m_iShakeLoop = _iLoopCnt; }
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

