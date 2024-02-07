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
	Vec3 GetCurWeaponMuzzlePos();
public:
	void tick();
public:
	WeaponMgr();
	virtual ~WeaponMgr();
};

