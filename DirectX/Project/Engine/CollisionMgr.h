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

public:

	void SetLayerIntersect(LAYER_TYPE _LeftType, LAYER_TYPE _RightType, bool _enable);
	void SetLayerIntersect(UINT _intLeftLayerIdx, UINT _intRightLayerIdx, bool _enable);
	bool IsLayerIntersect(LAYER_TYPE _LeftType, LAYER_TYPE _RightType);
	bool IsLayerIntersect(UINT _intLeftLayerIdx, UINT _intRightLayerIdx);

	void Clear()
	{
		m_matrix->reset();
	}

public:
	void tick();

};
