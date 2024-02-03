#include "pch.h"
#include "PlayerMgr.h"

#include "CEventMgr.h"
#include "CLayer.h"
#include "CLevel.h"
#include "CLevelMgr.h"

PlayerMgr::PlayerMgr()
	: m_pPlayer(nullptr)
{
}

PlayerMgr::~PlayerMgr()
{
}

void PlayerMgr::init()
{
}

void PlayerMgr::tick()
{
	if(CEventMgr::GetInst()->IsLevelChanged())
	{
		vector<CGameObject*> objects = CLevelMgr::GetInst()->GetCurLevel()->FindLayerByName(L"Player")->GetObjects();
		for (int i = 0; i < objects.size(); ++i)
		{
			if (objects[i]->GetName() == L"player")
			{
				m_pPlayer = objects[i];
				break;
			}
		}
	}
}
