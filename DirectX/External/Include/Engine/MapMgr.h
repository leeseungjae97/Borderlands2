#pragma once
#include "CSingleton.h"
class MapMgr
	: public CSingleton<MapMgr>
{
private:
	CGameObject* m_pUI_Map;
	CGameObject* m_pUI_MapMarker;
	CGameObject* m_pUI_MapMarkerRange;
public:
	void tick();
	void init();
	void begin();
	
public:
	MapMgr();
	virtual ~MapMgr();
};