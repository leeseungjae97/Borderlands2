#include "pch.h"
#include "PlayerMgr.h"

#include "CAnimator3D.h"
#include "CEventMgr.h"
#include "CLayer.h"
#include "CLevel.h"
#include "CLevelMgr.h"
#include "CMeshRender.h"
#include "CTransform.h"

PlayerMgr::PlayerMgr()
	: m_pPlayer(nullptr)
{
}

PlayerMgr::~PlayerMgr()
{
}

void PlayerMgr::init()
{
}

void PlayerMgr::tick()
{
	if(CEventMgr::GetInst()->IsLevelChanged())
	{
		vector<CGameObject*> objects = CLevelMgr::GetInst()->GetCurLevel()->FindLayerByName(L"Player")->GetObjects();
		for (int i = 0; i < objects.size(); ++i)
		{
			if (objects[i]->GetName() == L"player")
			{
				m_pPlayer = objects[i];
				break;
			}
		}
	}
}

Vec3 PlayerMgr::GetPlayerCameraPos()
{
	if (nullptr == m_pPlayer) 
		return Vec3::Zero;

	Matrix matWorld = m_pPlayer->Transform()->GetWorldMat();
	int iCameraIdx = m_pPlayer->Animator3D()->GetCameraIdx();
	Vec4 v = m_pPlayer->Animator3D()->GetCameraPos();
	Vec3 vPos = m_pPlayer->MeshRender()->GetMesh()->BonePosSkinning(iCameraIdx, v);

	vPos = XMVector3TransformCoord(vPos, matWorld);

	return vPos;
}

Vec3 PlayerMgr::GetPlayerWeaponPos()
{
	if (nullptr == m_pPlayer)
		return Vec3::Zero;

	Matrix matWorld = m_pPlayer->Transform()->GetWorldMat();

	int iWeaponHandIdx = m_pPlayer->Animator3D()->GetWeaponHandIdx();
	Vec4 v = m_pPlayer->Animator3D()->GetWeaponHandPos();
	Vec3 vPos = m_pPlayer->MeshRender()->GetMesh()->BonePosSkinning(iWeaponHandIdx, v);

	vPos = XMVector3TransformCoord(vPos, matWorld);

	return vPos;
}
