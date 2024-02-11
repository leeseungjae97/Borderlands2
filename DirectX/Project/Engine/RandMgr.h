#pragma once
#include "CSingleton.h"

class RandMgr
	: public CSingleton<RandMgr>
{
private:
	int m_arrRandNums[1000];
	int m_arrRandMuzzleX[3];
	int m_arrRandMuzzleY[3];

public:
	int GetRandMuzzleX(int _ModuleNum);
	int GetRandMuzzleY(int _ModuleNum);
	void RangeCheck(int* nums, int size);

	void init();

public:
	RandMgr();
	virtual ~RandMgr();
};

