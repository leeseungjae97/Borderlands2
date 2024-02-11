#include "pch.h"
#include "PlayerMgr.h"

#include "CameraMgr.h"
#include "CAnimator3D.h"
#include "CEventMgr.h"
#include "CLayer.h"
#include "CLevel.h"
#include "CLevelMgr.h"
#include "CMeshRender.h"
#include "CTransform.h"
//#include <Script/CPlayerScript.h>

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
	if (CEventMgr::GetInst()->IsLevelChanged())
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

	int iCameraIdx = m_pPlayer->Animator3D()->GetCameraIdx();
	Vec3 vPos = m_pPlayer->MeshRender()->GetMesh()->BonePosSkinning(iCameraIdx, m_pPlayer->Animator3D());

	vPos = XMVector3TransformCoord(vPos, m_pPlayer->Transform()->GetWorldMat());
	//Vec3 vFront = m_pPlayer->Transform()->GetRelativeDir(DIR_TYPE::FRONT);
	//vPos += vFront * 100.f;
	return vPos;
}

Vec3 PlayerMgr::GetPlayerWeaponRot()
{
	if (nullptr == m_pPlayer)
		return Vec3::Zero;

	int iWeaponHandRotIdx = m_pPlayer->Animator3D()->GetWeaponHandIdx();

	Vec3 vRot = m_pPlayer->MeshRender()->GetMesh()->BoneRotSkinning(iWeaponHandRotIdx, m_pPlayer->Animator3D());

	return vRot;
}

Vec3 PlayerMgr::GetConvertAnimationPos(Vec3 _vPos)
{
	if (nullptr == m_pPlayer)
		return Vec3::Zero;

	int iWeaponHandIdx = m_pPlayer->Animator3D()->GetWeaponHandIdx();
	Vec3 vPos = m_pPlayer->MeshRender()->GetMesh()->VertexPosSkinning(_vPos, iWeaponHandIdx, m_pPlayer->Animator3D());
	//Vec3 vPos = m_pPlayer->MeshRender()->GetMesh()->VertexPosSkinning(_vPos, m_pPlayer->Animator3D());

	vPos = XMVector3TransformCoord(vPos, m_pPlayer->Transform()->GetWorldMat());

	return vPos;
}

Vec3 PlayerMgr::GetPlayerWeaponPos()
{
	if (nullptr == m_pPlayer)
		return Vec3::Zero;

	int iWeaponHandIdx = m_pPlayer->Animator3D()->GetWeaponHandIdx();
	Vec3 vPos = m_pPlayer->MeshRender()->GetMesh()->BonePosSkinning(iWeaponHandIdx, m_pPlayer->Animator3D());

	vPos = XMVector3TransformCoord(vPos, m_pPlayer->Transform()->GetWorldMat());

	return vPos;
}
