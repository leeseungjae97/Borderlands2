#include "pch.h"
#include "WarriorMgr.h"

#include "CEventMgr.h"
#include "CLayer.h"
#include "CLevel.h"
#include "CLevelMgr.h"

WarriorMgr::WarriorMgr()
	: m_pWarrior(nullptr)
{
}

WarriorMgr::~WarriorMgr()
{
}

void WarriorMgr::init()
{
}

void WarriorMgr::tick()
{
	if(CEventMgr::GetInst()->IsLevelChanged())
	{
		CLevel* level = CLevelMgr::GetInst()->GetCurLevel();
		level->GetLayer((int)LAYER_TYPE::Enemy)->GetObjects();
		//m_pWarrior;
	}
}

void WarriorMgr::finaltick()
{
}