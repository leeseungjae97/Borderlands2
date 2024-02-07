#pragma once
#include "CSingleton.h"
#include <bitset>

class CLayer;
class CCollider3D;

class CollisionMgr :
	public CSingleton<CollisionMgr>
{
	SINGLE(CollisionMgr);
private:
	std::bitset<MAX_LAYER> m_matrix[MAX_LAYER];

	int m_iBulletLayerIdx;

public:
	void SetBulletLayerIdx(int idx) { m_iBulletLayerIdx = idx; }
	int GetBulletLayerIdx() { return m_iBulletLayerIdx; }

	void SetLayerIntersect(const wstring& _strLeftLayer, const wstring& _strRightLayer, bool _enable);
	bool IsLayerIntersect(const wstring& _strLeftLayer, const wstring& _strRightLayer);
	bool IsLayerIntersect(UINT _intLeftLayerIdx, UINT _intRightLayerIdx);

	void Clear()
	{
		m_matrix->reset();
	}

public:
	void tick();

};
