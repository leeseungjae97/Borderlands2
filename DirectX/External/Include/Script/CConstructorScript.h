#pragma once
#include <Engine\CScript.h>
class CConstructorScript :
    public CScript
{
public:
    enum class CONSTRUCTOR_STATE
	{
        BEAM,
        BEAM_ENTER,
        BEAM_LOOP,
        BEAM_EXIT,
        SMASH_GROUND,
        GROUND_TO_SKY,
        IDLE,
        DIE,
        HIDE,
	    APPEAR,
        END
    };

private:
    CONSTRUCTOR_STATE tState;

    float fRotateSpeed;
    float fBeamTime;
    float fActAcc;

    int   iConsHp;
    int   iConsHpCapa;

    CGameObject* pBeam;
    CGameObject* pBeamCollider;
    CGameObject* pSmashCollider;
    CGameObject* pEyeCollider;

    CGameObject* pAttackBoundCollider;
    CGameObject* pSmashEffect;

    CGameObject* pUI_ConsHP;
    CGameObject* pUI_ConsHPBack;

    class CUI* pConsText;

    wstring wsBeamIdleSound = L"sound\\constructor\\beam.ogg";

    wstring wsAppearSound = L"sound\\constructor\\fall_sound.ogg";
    wstring wsToGroundSound = L"sound\\constructor\\to_ground.ogg";

    wstring wsLegSound = L"sound\\constructor\\leg_sound.ogg";

    wstring wsStepSound = L"sound\\constructor\\step.ogg";

    wstring wsLaserVoiceSound1 = L"sound\\constructor\\VOBD_Constructor_Start_LaserEye_Attack_01.ogg";
    wstring wsLaserVoiceSound2 = L"sound\\constructor\\VOBD_Constructor_Start_LaserEye_Attack_02.ogg";

    wstring wsVoiceSound1 = L"sound\\constructor\\voice_1.ogg";
    wstring wsVoiceSound2 = L"sound\\constructor\\voice_2.ogg";
    wstring wsVoiceSound3 = L"sound\\constructor\\voice_3.ogg";

    wstring wsDieSound = L"sound\\constructor\\VOBD_Constructor_Dead.mp3";


    bool bSmash;
public:
    void DoBeam();
    void DoSmash();

    void IsDie();

private:
    void beamMove();
    void eyeMove();
    void paperBurn();
    void smashEffect();

    float GetHpRatio();
public:
    bool Rotate();

public:
    void finaltick() override;
    void tick() override;
    void begin() override;

    void Raycast(tRayInfo _RaycastInfo) override;
    void Attacked(int _Damage);

    void initAnim();
    void createUI();
    void createCollider();
    void createObject();

    CLONE(CConstructorScript);
public:
    CConstructorScript();
    virtual ~CConstructorScript();
};

