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

void CollisionMgr::SetLayerIntersect(const wstring& _strLeftLayer, const wstring& _strRightLayer, bool _enable)
{
	CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurLevel();

	CLayer* pLeft = pCurLevel->FindLayerByName(_strLeftLayer);
	CLayer* pRight = pCurLevel->FindLayerByName(_strRightLayer);

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

bool CollisionMgr::IsLayerIntersect(const wstring& _strLeftLayer, const wstring& _strRightLayer)
{
	CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurLevel();

	CLayer* pLeft = pCurLevel->FindLayerByName(_strLeftLayer);
	CLayer* pRight = pCurLevel->FindLayerByName(_strRightLayer);

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
