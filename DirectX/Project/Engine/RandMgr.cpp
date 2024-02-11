#include "pch.h"
#include "RandMgr.h"

RandMgr::RandMgr()
{
}

RandMgr::~RandMgr()
{
}

void RandMgr::init()
{
	memset(m_arrRandNums, 0, sizeof(int) * ARRAYSIZE(m_arrRandNums));
	memset(m_arrRandMuzzleX, 0, sizeof(int) * ARRAYSIZE(m_arrRandMuzzleX));
	memset(m_arrRandMuzzleY, 0, sizeof(int) * ARRAYSIZE(m_arrRandMuzzleY));

	srand(time(NULL));
}

int RandMgr::GetRandMuzzleX(int _ModuleNum)
{
	RangeCheck(m_arrRandMuzzleX, ARRAYSIZE(m_arrRandMuzzleX));

	int randNum = rand() % _ModuleNum;
	while(m_arrRandMuzzleX[randNum] != 0)
		randNum = rand() % _ModuleNum;

	++m_arrRandMuzzleX[randNum];

	return randNum;
}

int RandMgr::GetRandMuzzleY(int _ModuleNum)
{
	RangeCheck(m_arrRandMuzzleY, ARRAYSIZE(m_arrRandMuzzleY));

	int randNum = rand() % _ModuleNum;
	while (m_arrRandMuzzleY[randNum] != 0)
		randNum = rand() % _ModuleNum;

	++m_arrRandMuzzleY[randNum];

	return randNum;
}

void RandMgr::RangeCheck(int* nums, int size)
{
	++nums[size - 1];
	if (nums[size - 1] >= size - 1)
	{
		memset(nums, 0, sizeof(int) * size);
	}
}
