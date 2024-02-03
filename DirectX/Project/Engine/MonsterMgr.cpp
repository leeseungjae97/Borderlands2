#include "pch.h"
#include "MonsterMgr.h"

#include "CEventMgr.h"

MonsterMgr::MonsterMgr()
{
}

MonsterMgr::~MonsterMgr()
{
}

void MonsterMgr::init()
{
}

void MonsterMgr::tick()
{
	if (CEventMgr::GetInst()->IsLevelChanged())
	{
		//TODO: Get Monsters
	}
}
