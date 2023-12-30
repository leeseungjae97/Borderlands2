#pragma once
#include "CEntity.h"
enum FACE_TYPE
{
	FT_NEAR,
	FT_FAR,
	FT_DOWN,
	FT_UP,
	FT_LEFT,
	FT_RIGHT,

	FT_END,
};

class CCamera;
class CFrustum :
    public CEntity
{
private:
	CCamera*	m_pCam;
	Vec3		m_arrProj[8];
	Vec4		m_arrFace[FT_END];

public:
	void finaltick();

public:
    bool FrustumCheck(Vec3 _vPos);
    bool FrustumCheck(Vec3 _vPos, float _fRadius);
	
public:
    CLONE(CFrustum);

public:
    CFrustum() {}
    CFrustum(CCamera* _Camera);
    ~CFrustum();
};

