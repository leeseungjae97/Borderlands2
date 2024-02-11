#pragma once
#include "CSingleton.h"

class WeaponMgr
	: public CSingleton<WeaponMgr>
{
private:
	int			iCurWeaponIdx;
	wstring		iCurWeaponName;

	CGameObject* m_arrWeapons[3];
	int m_arrWeaponMuzzleIdx[3];
public:
	void ChangeWeapon(int _Idx);
	void AddWeapon(CGameObject* _Weapon);
	CGameObject* GetCurWeapon();
	int GetCurWeaponMuzzleIdx() { return iCurWeaponIdx; }
	Vec3 GetCurWeaponMuzzlePos();
	void Play(GUN_ANIMATION_TYPE _Type, bool _Loop);
	

public:
	void tick();
	void begin();

public:
	WeaponMgr();
	virtual ~WeaponMgr();
};

