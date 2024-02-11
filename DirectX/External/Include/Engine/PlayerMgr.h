#pragma once

#include "CSingleton.h"

class PlayerMgr
	: public CSingleton<PlayerMgr>
{
public:
	enum class PLAYER_STATE {
		RELOAD,
		FIRE,
		SPRINT,
		WALK,
		IDLE,
	};

private:
	CGameObject*	m_pPlayer;
	Vec3			m_vCameraPos;
	int				m_iCameraBoneIdx;

public:
	CGameObject* GetPlayer() { return m_pPlayer; }
	void SetPlayer(CGameObject* _Player) { m_pPlayer = _Player; }

public:
	void init();
	void tick();

public:
	Vec3 GetPlayerCameraPos();
	Vec3 GetPlayerWeaponPos();
	Vec3 GetPlayerWeaponRot();
	Vec3 GetConvertAnimationPos(Vec3 _vPos);
public:
	PlayerMgr();
	virtual ~PlayerMgr();
};

