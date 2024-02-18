#pragma once
#include "CSingleton.h"

class WarriorMgr
	: public CSingleton<WarriorMgr>
{
public:
	enum class WARRIOR_STATE
	{
		ENTER,
		IDLE,
		DIE,
		BREATH,
		TAIL_BEAM,
		TAIL_ATTACK,
		ROCK_THROW,
	};
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

