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
			for (j = 0; j < layer->GetParentObject().size(); ++j)
			{
				meshRender = layer->GetParentObject()[j]->MeshRender();
				animator = layer->GetParentObject()[j]->Animator3D();
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
		for (i = 0; i < layer->GetParentObject().size(); ++i)
		{
			meshRender = layer->GetParentObject()[i]->MeshRender();
			animator = layer->GetParentObject()[i]->Animator3D();
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

	CTransform* OwnerTransform = _BoneOwner->Transform();
	if (nullptr == OwnerTransform)
		return Vec3::Zero;

	CAnimator3D* OwnerAnimator = _BoneOwner->Animator3D();
	if (nullptr == OwnerAnimator)
		return Vec3::Zero;

	Vec3 vPos = Vec3::Zero;
	if (CMeshRender* MeshRender = _BoneOwner->MeshRender())
	{
		if (!OwnerAnimator->IsUpdate())
			return Vec3::Zero;

		Matrix matBone = Matrix::Identity;

		matBone = OwnerAnimator->GetPosMatFrameLatency(_BoneIdx);
		//matBone = OwnerAnimator->GetPosMatNoFrameLatency(_BoneIdx);

		Quat quat; Vec3 vS, vT;

		matBone.Decompose(vS, quat, vT);
		vPos = Vec3(matBone._14, matBone._24, matBone._34);
	}

	if(vPos != Vec3::Zero)
		vPos = XMVector3TransformCoord(vPos, OwnerTransform->GetWorldMat());

	return vPos;
}

Vec3 AnimationMgr::BoneRot(int _BoneIdx, CGameObject* _BoneOwner)
{
	if (nullptr == _BoneOwner)
		return Vec3::Zero;

	CAnimator3D* OwnerAnimator = _BoneOwner->Animator3D();
	if (nullptr == OwnerAnimator)
		return Vec3::Zero;


	//return Vec3::Zero;
	if (OwnerAnimator->IsUpdate())
		return Vec3::Zero;

	Matrix matBone = Matrix::Identity;
	Vec3 vRot = Vec3::Zero;

	matBone = OwnerAnimator->GetRotMatFrameLatency(_BoneIdx);
	//matBone = OwnerAnimator->GetRotMatNoFrameLatency(_BoneIdx);

	Quat quat;
	Vec3 vS, vT;

	matBone.Decompose(vS, quat, vT);

	vRot = physx::Util::QuaternionToVector3(quat);
	vRot.x *= -1.f;
	vRot.y *= -1.f;
	vRot.z *= -1.f;
	vRot.y += XM_PI / 2.f;

	return vRot;
}
