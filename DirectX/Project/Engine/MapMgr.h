#pragma once
#include "CSingleton.h"
class MapMgr
	: public CSingleton<MapMgr>
{
public:
	void tick();
	void init();

public:
	MapMgr();
	virtual ~MapMgr();
};

