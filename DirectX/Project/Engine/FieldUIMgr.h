#pragma once
#include "CSingleton.h"

class FieldUIMgr
	: public CSingleton<FieldUIMgr>
{
public:
	void init();
	void tick();

public:
	FieldUIMgr();
	virtual ~FieldUIMgr();
};

