#pragma once

#include "CSingleton.h"

class MonsterMgr
	: public CSingleton<MonsterMgr>
{
private:
	vector<CGameObject*> m_vecMonsters;

public:
	void init();
	void tick();

public:
	MonsterMgr();
	virtual ~MonsterMgr();
};

