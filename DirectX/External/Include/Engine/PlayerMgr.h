#pragma once

#include "CSingleton.h"

class PlayerMgr
	: public CSingleton<PlayerMgr>
{
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
public:
	PlayerMgr();
	virtual ~PlayerMgr();
};

