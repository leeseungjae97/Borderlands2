#pragma once
#include "CSingleton.h"

class WeaponMgr
	: public CSingleton<WeaponMgr>
{
private:
	int			iCurWeaponIdx;
	wstring		iCurWeaponName;

	CGameObject* m_arrWeapons[3];

public:
	bool ChangeWeapon(int _Idx, bool _Force = false);
	void AddWeapon(CGameObject* _Weapon);

	void SetEqui(int _Idx, bool _Equi);
	void SetEquis(bool _Equi);

	CGameObject* GetCurWeapon();
	int GetCurWeaponIdx() { return iCurWeaponIdx; }
	int GetCurWeaponPlayerAnim(PLAYER_ANIMATION_TYPE type) { return iCurWeaponIdx + (int)type; }

	Vec3 GetCurWeaponMuzzlePos();
	void Play(GUN_ANIMATION_TYPE _Type, bool _Loop);

	Vec3 GetWeaponMuzzlePos(CGameObject* _Gun);
	Vec3 GetOwnerWeaponRot(CGameObject* _Owner);
	Vec3 GetOwnerWeaponPos(CGameObject* _Owner);
	void MuzzleFlash(Vec3 _vPos, Vec3 _vRot);
public:
	void tick();
	void begin();

public:
	WeaponMgr();
	virtual ~WeaponMgr();
};

