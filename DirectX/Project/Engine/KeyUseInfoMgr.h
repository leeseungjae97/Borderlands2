#pragma once
#include "CSingleton.h"

class KeyUseInfoMgr
	: public CSingleton<KeyUseInfoMgr>
{
	SINGLE(KeyUseInfoMgr)
public:
	void render();
};

