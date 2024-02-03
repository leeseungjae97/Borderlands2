#pragma once
#include "CSingleton.h"

class AnimationMgr
	: public CSingleton<AnimationMgr>
{

public:
	void AdaptAnimation(CGameObject* _LayerObject, bool _AllLayer = false);

public:
	AnimationMgr();
	virtual ~AnimationMgr();
};

