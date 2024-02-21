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
		FAR_BREATH,
		MID_BREATH,
		NEAR_BREATH,
		TAIL_BEAM_ENTER,
		TAIL_BEAM_IDLE,
		TAIL_BEAM_EXIT,
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

