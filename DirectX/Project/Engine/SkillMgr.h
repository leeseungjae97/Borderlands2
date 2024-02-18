#pragma once
#include "CSingleton.h"

class SkillMgr
	: public CSingleton<SkillMgr>
{
public:
	void init();
	void tick();
	void finaltick();

public:
	SkillMgr();
	virtual ~SkillMgr();
};

