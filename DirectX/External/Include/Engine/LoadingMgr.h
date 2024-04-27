#pragma once
#include "CSingleton.h"

class LoadingMgr
	: public CSingleton<LoadingMgr>
{
private:
	int iStageNum;

public:
	void tick();
	void begin();

public:
	void SetStage(int _iStage) { iStageNum = _iStage; }
	int GetStage() { return iStageNum; }

public:
	LoadingMgr();
	virtual ~LoadingMgr();
};

