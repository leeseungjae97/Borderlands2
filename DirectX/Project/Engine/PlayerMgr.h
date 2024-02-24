#pragma once

#include "CSingleton.h"

class PlayerMgr
	: public CSingleton<PlayerMgr>
{
public:
	enum class PLAYER_STATE {
		RELOAD,
		FIRE,
		DRAW,
		SPRINT,
		WALK,
		IDLE,
	};

private:
	CGameObject*	m_pPlayer;
	CGameObject*	m_pPlayerBody;
	Vec3			m_vCameraPos;
	int				m_iCameraBoneIdx;

public:
	CGameObject* GetPlayer() { return m_pPlayer; }
	CGameObject* GetPlayerBody() { return m_pPlayerBody; }

	void SetPlayer(CGameObject* _Player) { m_pPlayer = _Player; }
	void SetPlayerBody(CGameObject* _PlayerBody) { m_pPlayerBody = _PlayerBody; }

public:
	void init();
	void tick();

public:
	Vec3 GetPlayerCameraPos();
	Vec3 GetPlayerWeaponPos();
	Vec3 GetPlayerWeaponRot();
	
public:
	PlayerMgr();
	virtual ~PlayerMgr();
};

