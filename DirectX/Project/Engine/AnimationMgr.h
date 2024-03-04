#pragma once
#include "CSingleton.h"

class CAnimator3D;
class AnimationMgr
	: public CSingleton<AnimationMgr>
{

public:
	void AdaptAnimation(CGameObject* _LayerObject, bool _AllLayer = false);
	double GetCurAnimationTime(CAnimator3D* _Animator);
	void AnimationSync(CAnimator3D* _Animator1, CAnimator3D* _Animator2);
	Vec3 BonePos(int _BoneIdx, CGameObject* _BoneOwner);
	Vec3 BoneRot(int _BoneIdx, CGameObject* _BoneOwner);
public:
	AnimationMgr();
	virtual ~AnimationMgr();
};

