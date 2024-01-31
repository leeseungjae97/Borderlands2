#pragma once
#include "CSingleton.h"

class BulletMgr
	: public CSingleton<BulletMgr>
{
	SINGLE(BulletMgr)
private:
	vector<CGameObject*> m_vecBullets;

public:
	void AddBullet(CGameObject* bullet) { m_vecBullets.push_back(bullet); }
	void EraseBullet(CGameObject* bullet);
	void EraseAll();

};

