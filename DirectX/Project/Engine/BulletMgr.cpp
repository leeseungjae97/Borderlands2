#include "pch.h"
#include "BulletMgr.h"

void BulletMgr::EraseBullet(CGameObject* bullet)
{
	vector<CGameObject*>::iterator iter = m_vecBullets.begin();
	while(iter != m_vecBullets.end())
	{
		if (*iter != bullet)
			++iter;
		else
		{
			DestroyObject(bullet);
			iter = m_vecBullets.erase(iter);
			return;
		}
	}
}

void BulletMgr::EraseAll()
{
	for(auto obj : m_vecBullets)
	{
		DestroyObject(obj);
	}

	m_vecBullets.clear();
	vector<CGameObject*>().swap(m_vecBullets);
}
