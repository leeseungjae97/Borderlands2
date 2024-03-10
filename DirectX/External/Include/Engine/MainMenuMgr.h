#pragma once
#include "CSingleton.h"

class CUI;
class MainMenuMgr
	: public CSingleton<MainMenuMgr>
{
private:
	vector<CUI*> m_vecMainMenuCUI;
	CGameObject* m_pHoverImg;
public:
	
	void tick();
	void AddCUI(CUI* _Cui) { m_vecMainMenuCUI.push_back(_Cui); }

private:
	void makeHoverImg();
	void reload();
public:
	MainMenuMgr();
	virtual ~MainMenuMgr();
};

