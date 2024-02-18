#pragma once

#include "CSingleton.h"

class EnemyMgr
	: public CSingleton<EnemyMgr>
{
public:
	enum class ENEMY_TYPE
	{
		NOMAD,
		PSYCHO,
	};
	enum class ENEMY_STATE {
		RELOAD,
		FIRE,
		WALK,
		DIE,
		IDLE,
	};
private:
	vector<CGameObject*> m_vecEnemies;

public:
	void init();
	void tick();
	void EnemyGen();
public:
	EnemyMgr();
	virtual ~EnemyMgr();
};

