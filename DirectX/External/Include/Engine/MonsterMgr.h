#pragma once

#include "CSingleton.h"

class MonsterMgr
	: public CSingleton<MonsterMgr>
{
public:
	enum class MONSTER_TYPE
	{
		NOMAD,
		PSYCHO,
	};
	enum class MONSTER_STATE {
		RELOAD,
		FIRE,
		WALK,
		DEAD,
		DIE,
		IDLE,
	};
private:
	vector<CGameObject*> m_vecMonsters;

public:
	void init();
	void tick();
	void MonsterGen();
public:
	MonsterMgr();
	virtual ~MonsterMgr();
};

