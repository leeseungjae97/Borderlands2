#pragma once
#include "CSingleton.h"
#include "CUI.h"

class FieldUIMgr
	: public CSingleton<FieldUIMgr>
{
public:
	struct DamageText
	{
		Vec3 vPos;
		Vec3 vDir;
		float fAcc;
		float fAlpha;
		int iDamage;
	};
public:
	Matrix BilBoardMat;
	float  fTheta;
	vector<DamageText> m_vecText;
public:
	void init();
	void tick();
	void render();

	void AddDamage(int _Damage, Vec3 _vPos);
public:
	FieldUIMgr();
	virtual ~FieldUIMgr();
};

