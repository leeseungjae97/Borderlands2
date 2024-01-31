#pragma once

#include "CSingleton.h"

class PlayerMgr
	: public CSingleton<PlayerMgr>
{
private:
	CGameObject* m_pPlayer;

public:
	CGameObject* GetPlayer() { return m_pPlayer; }
	void SetPlayer(CGameObject* _Player) { m_pPlayer = _Player; }

public:
	void init();
	void tick();
	
public:
	PlayerMgr();
	virtual ~PlayerMgr();
};

