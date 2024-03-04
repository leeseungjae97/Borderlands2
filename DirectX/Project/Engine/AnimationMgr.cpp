#include "pch.h"
#include "AnimationMgr.h"

#include "CAnimator3D.h"
#include "CLayer.h"
#include "CLevel.h"
#include "CLevelMgr.h"
#include "CMeshRender.h"
#include "CTransform.h"

AnimationMgr::AnimationMgr()
{
}

AnimationMgr::~AnimationMgr()
{
}

void AnimationMgr::AdaptAnimation(CGameObject* _LayerObject, bool _AllLayer)
{
	CLevel* curLevel = CLevelMgr::GetInst()->GetCurLevel();
	CLayer* layer = nullptr;
	if (_AllLayer)
	{
		auto map = _LayerObject->Animator3D()->GetPrefDefineAnimation();
		wstring meshName = _LayerObject->MeshRender()->GetMesh()->GetName();
		int i, j;
		CMeshRender* meshRender;
		CAnimator3D* animator;
		for (i = 0; i < MAX_LAYER; ++i)
		{
			layer = curLevel->GetLayer(i);
			for (j = 0; j < layer->GetObjects().size(); ++j)
			{
				meshRender = layer->GetObjects()[j]->MeshRender();
				animator = layer->GetObjects()[j]->Animator3D();
				if (nullptr != meshRender && nullptr != animator)
				{
					wstring otherMeshName = meshRender->GetMesh()->GetName();
					if (meshName == otherMeshName)
					{
						animator->SetPreDefineAnimation(map);
					}
				}
			}
		}
	}
	else
	{
		int adaptLayerIdx = _LayerObject->GetLayerIndex();
		auto map = _LayerObject->Animator3D()->GetPrefDefineAnimation();
		wstring meshName = _LayerObject->MeshRender()->GetMesh()->GetName();
		wstring otherMeshName = L"";
		CMeshRender* meshRender;
		CAnimator3D* animator;
		layer = curLevel->GetLayer(adaptLayerIdx);
		int i;
		for (i = 0; i < layer->GetObjects().size(); ++i)
		{
			meshRender = layer->GetObjects()[i]->MeshRender();
			animator = layer->GetObjects()[i]->Animator3D();
			if (nullptr != meshRender && nullptr != animator)
			{
				otherMeshName = meshRender->GetMesh()->GetName();
				if (meshName == otherMeshName)
				{
					animator->SetPreDefineAnimation(map);
				}
			}
		}
	}
}

double AnimationMgr::GetCurAnimationTime(CAnimator3D* _Animator)
{
	CAnimClip* anim_clip = _Animator->GetNextAnimClip();
	if (nullptr == anim_clip)
		anim_clip = _Animator->GetCurAnimClip();

	tMTAnimClip clip = anim_clip->GetCurClip();

	double clip_time_diff = clip.dEndTime - clip.dStartTime;

	return clip_time_diff;
}

void AnimationMgr::AnimationSync(CAnimator3D* _Animator1, CAnimator3D* _Animator2)
{
	//CAnimClip* anim_clip_1 = _Animator1->GetNextAnimClip();
	//if(nullptr == anim_clip_1)
	//	anim_clip_1 = _Animator1->GetCurAnimClip();
	//
	//CAnimClip* anim_clip_2 = _Animator2->GetNextAnimClip();
	//if (nullptr == anim_clip_2)
	//	anim_clip_2 = _Animator2->GetCurAnimClip();

	//tMTAnimClip clip_1 = anim_clip_1->GetCurClip();
	//tMTAnimClip clip_2 = anim_clip_2->GetCurClip();

	//double clip_1_time_diff= clip_1.dEndTime - clip_1.dStartTime;
	//double clip_2_time_diff= clip_2.dEndTime - clip_2.dStartTime;

	double clip_1_time_diff = GetCurAnimationTime(_Animator1);
	double clip_2_time_diff = GetCurAnimationTime(_Animator2);;

	double frame_diff = max(clip_1_time_diff, clip_2_time_diff) / min(clip_1_time_diff, clip_2_time_diff);
	if(clip_1_time_diff < clip_2_time_diff)
	{
		_Animator2->SetSpeedAdjustment(frame_diff);
	}
	else
	{
		_Animator1->SetSpeedAdjustment(frame_diff);
	}
}

Vec3 AnimationMgr::BonePos(int _BoneIdx, CGameObject* _BoneOwner)
{
	if (nullptr == _BoneOwner)
		return Vec3::Zero;

	Vec3 vPos = _BoneOwner->MeshRender()->GetMesh()->BonePosSkinning(_BoneIdx, _BoneOwner->Animator3D());

	vPos = XMVector3TransformCoord(vPos, _BoneOwner->Transform()->GetWorldMat());

	return vPos;
}

Vec3 AnimationMgr::BoneRot(int _BoneIdx, CGameObject* _BoneOwner)
{
	if (nullptr == _BoneOwner)
		return Vec3::Zero;

	Vec3 vPos = _BoneOwner->MeshRender()->GetMesh()->BoneRotSkinning(_BoneIdx, _BoneOwner->Animator3D());

	return vPos;
}
