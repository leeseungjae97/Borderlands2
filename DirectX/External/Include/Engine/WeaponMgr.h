#pragma once
#include "CSingleton.h"

class WeaponMgr
	: public CSingleton<WeaponMgr>
{
private:
	int			iCurWeaponIdx;
	wstring		iCurWeaponName;

	CGameObject* m_arrWeapons[3];

	wstring wsWeaponFireSound[6] = {
		L"sound\\effect\\gun_fire1.ogg",
		L"sound\\effect\\gun_fire2.ogg",
		L"sound\\effect\\gun_fire3.ogg",
		L"sound\\effect\\gun_fire4.ogg",
		L"sound\\effect\\gun_fire5.ogg",
		L"sound\\effect\\gun_fire6.ogg",
	};
	Vec3 vWeaponScopeOffset[3] =
	{
		Vec3(0.f, 0.f, 0.f),
		Vec3(0.f, 0.f, 0.f),
		Vec3(0.f, 0.f, 0.f)
	};
	enum class WEAPON_TYPE {
		SMG,
		SNIPER,
		PISTOL,
		END
	};
	int iAmmo[(UINT)WEAPON_TYPE::END] =
	{
		30,
		10,
		10,
	};
	int iAmmoCapa[(UINT)WEAPON_TYPE::END] =
	{
		30,
		10,
		10,
	};
	int iAmmoRemain[(UINT)WEAPON_TYPE::END] =
	{
		2250,
		150,
		1250,
	};
	int iDamage[(UINT)WEAPON_TYPE::END] =
	{
		10,
		50,
		5,
	};


	float fRateOfFire[(UINT)WEAPON_TYPE::END] =
	{
		0.05f,
		0.1f,
		0.09f
	};
	WEAPON_TYPE m_tType;

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

	void SetType(WEAPON_TYPE _tType) { m_tType = _tType; }

	Vec3 GetWeaponMuzzlePos(CGameObject* _Gun);
	Vec3 GetOwnerWeaponRot(CGameObject* _Owner, bool bRight = true);
	Vec3 GetOwnerWeaponPos(CGameObject* _Owner, bool bRight = true);
	Vec3 GetCurWeaponScopePos(CGameObject* _Owner);

	Vec3 GetCurWeaponOffset() { return vWeaponScopeOffset[iCurWeaponIdx]; }
	void MuzzleFlash(Vec3 _vPos, Vec3 _vRot, CGameObject* _pp = nullptr);

	void ShootBullet();

	int GetCurWeaponAmmo() { return iAmmo[iCurWeaponIdx]; }
	int GetCurWeaponAmmoCapa() { return iAmmoCapa[iCurWeaponIdx]; }
	int GetCurWeaponAmmoRemain() { return iAmmoRemain[iCurWeaponIdx]; }
	float GetCurWeaponFireRate() { return fRateOfFire[iCurWeaponIdx]; }
	int GetCurWeaponDamage() { return iDamage[iCurWeaponIdx]; }

	int SetCurWeaponAmmo(int _iAmmo) { return iAmmo[iCurWeaponIdx] = _iAmmo; }
	int SetCurWeaponAmmoRemain(int _iAmmoRemain) { return iAmmoRemain[iCurWeaponIdx] = _iAmmoRemain; }
public:
	void tick();
	void begin();

public:
	WeaponMgr();
	virtual ~WeaponMgr();
};

