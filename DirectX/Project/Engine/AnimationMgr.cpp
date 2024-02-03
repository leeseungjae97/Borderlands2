#include "pch.h"
#include "AnimationMgr.h"

#include "CAnimator3D.h"
#include "CLayer.h"
#include "CLevel.h"
#include "CLevelMgr.h"
#include "CMeshRender.h"

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
	if(_AllLayer)
	{
		auto map = _LayerObject->Animator3D()->GetPrefDefineAnimation();
		wstring meshName = _LayerObject->MeshRender()->GetMesh()->GetName();
		int i, j;
		CMeshRender* meshRender;
		CAnimator3D* animator;
		for(i = 0; i < MAX_LAYER; ++i)
		{
			layer = curLevel->GetLayer(i);
			for (j = 0; j < layer->GetObjects().size(); ++j)
			{
				meshRender = layer->GetObjects()[j]->MeshRender();
				animator = layer->GetObjects()[j]->Animator3D();
				if(nullptr != meshRender && nullptr != animator)
				{
					wstring otherMeshName = meshRender->GetMesh()->GetName();
					if (meshName == otherMeshName)
					{
						animator->SetPreDefineAnimation(map);
					}
				}
			}
		}
	}else
	{
		int adaptLayerIdx = _LayerObject->GetLayerIndex();
		auto map =_LayerObject->Animator3D()->GetPrefDefineAnimation();
		wstring meshName = _LayerObject->MeshRender()->GetMesh()->GetName();
		wstring otherMeshName = L"";
		CMeshRender* meshRender;
		CAnimator3D* animator;
		layer =curLevel->GetLayer(adaptLayerIdx);
		int i;
		for (i = 0; i < layer->GetObjects().size() ;++i)
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
