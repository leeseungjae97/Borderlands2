#pragma once
#include "CSingleton.h"

class WarriorMgr
	: public CSingleton<WarriorMgr>
{
public:

private:
	CGameObject* m_pWarrior;

public:
	CGameObject* GetWarrior() { return m_pWarrior; }

public:
	void init();
	void tick();
	void finaltick();

public:
	WarriorMgr();
	virtual ~WarriorMgr();
};

