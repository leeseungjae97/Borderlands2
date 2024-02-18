#include "pch.h"
#include "CollisionMgr.h"

#include "CLevelMgr.h"
#include "CLevel.h"
#include "CLayer.h"
#include "CGameObject.h"

CollisionMgr::CollisionMgr()
	: m_matrix{}
{

}


CollisionMgr::~CollisionMgr()
{

}

void CollisionMgr::tick()
{

}

void CollisionMgr::SetLayerIntersect(LAYER_TYPE _LeftType, LAYER_TYPE _RightType, bool _enable)
{
	CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurLevel();

	CLayer* pLeft = pCurLevel->FindLayerByType(_LeftType);
	CLayer* pRight = pCurLevel->FindLayerByType(_RightType);

	UINT iRow = (UINT)pLeft->GetLayerIndex();
	UINT iCol = (UINT)pRight->GetLayerIndex();

	if (iRow > iCol)
	{
		UINT iTmp = iRow;
		iRow = iCol;
		iCol = iTmp;
	}

	m_matrix[iCol][iRow] = _enable;
}
void CollisionMgr::SetLayerIntersect(UINT _intLeftLayerIdx, UINT _intRightLayerIdx, bool _enable)
{
	CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurLevel();

	CLayer* pLeft = pCurLevel->GetLayer(_intLeftLayerIdx);
	CLayer* pRight = pCurLevel->GetLayer(_intRightLayerIdx);

	UINT iRow = (UINT)pLeft->GetLayerIndex();
	UINT iCol = (UINT)pRight->GetLayerIndex();

	if (iRow > iCol)
	{
		UINT iTmp = iRow;
		iRow = iCol;
		iCol = iTmp;
	}

	m_matrix[iCol][iRow] = _enable;
}

bool CollisionMgr::IsLayerIntersect(LAYER_TYPE _LeftType, LAYER_TYPE _RightType)
{
	CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurLevel();

	CLayer* pLeft = pCurLevel->FindLayerByType(_LeftType);
	CLayer* pRight = pCurLevel->FindLayerByType(_RightType);

	UINT iRow = (UINT)pLeft->GetLayerIndex();
	UINT iCol = (UINT)pRight->GetLayerIndex();

	if (iRow > iCol)
	{
		UINT iTmp = iRow;
		iRow = iCol;
		iCol = iTmp;
	}

	return m_matrix[iCol][iRow];
}

bool CollisionMgr::IsLayerIntersect(UINT _intLeftLayerIdx, UINT _intRightLayerIdx)
{
	if (_intLeftLayerIdx == _intRightLayerIdx) return false;

	UINT iRow = _intLeftLayerIdx;
	UINT iCol = _intRightLayerIdx;

	if (iRow > iCol)
	{
		UINT iTmp = iRow;
		iRow = iCol;
		iCol = iTmp;
	}

	return m_matrix[iCol][iRow];
}
