#include "pch.h"
#include "PlayerMgr.h"

#include "CameraMgr.h"
#include "CAnimator3D.h"
#include "CEventMgr.h"
#include "CLayer.h"
#include "CLevel.h"
#include "CLevelMgr.h"
#include "CMeshRender.h"
#include "CRigidBody.h"
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
	if (CEventMgr::GetInst()->IsLevelChanged())
	{
		if(CLevelMgr::GetInst()->GetCurLevel()->GetName() != L"main menu level")
		{
			vector<CGameObject*> objects = CLevelMgr::GetInst()->GetCurLevel()->FindLayerByType(LAYER_TYPE::Player)->GetObjects();
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
}

Vec3 PlayerMgr::GetPlayerCameraPos()
{
	if (nullptr == m_pPlayer)
		return Vec3::Zero;

	int iCameraIdx = m_pPlayer->Animator3D()->GetCameraIdx();
	CRigidBody* rb = m_pPlayer->RigidBody();

	Vec3 vPos = m_pPlayer->MeshRender()->GetMesh()->BonePosSkinning(iCameraIdx, m_pPlayer->Animator3D());
	Vec3 vOffset = Vec3::Zero;
	if(m_pPlayer->Animator3D())
	{
		vOffset.y -= ( m_pPlayer->Transform()->GetRelativeScale().y / 2.f);
		vOffset += m_pPlayer->Transform()->GetRelativePosOffset();
	}
	
	vPos = XMVector3TransformCoord(vPos, rb->GetRigidBodyMatrix(vOffset));
	return vPos;
}

Vec3 PlayerMgr::GetPlayerWeaponRot()
{
	if (nullptr == m_pPlayer)
		return Vec3::Zero;

	int iWeaponHandRotIdx = m_pPlayer->Animator3D()->GetWeaponRHandIdx();

	Vec3 vRot = m_pPlayer->MeshRender()->GetMesh()->BoneRotSkinning(iWeaponHandRotIdx, m_pPlayer->Animator3D());

	return vRot;
}

Vec3 PlayerMgr::GetPlayerWeaponPos()
{
	if (nullptr == m_pPlayer)
		return Vec3::Zero;

	int iWeaponHandIdx = m_pPlayer->Animator3D()->GetWeaponRHandIdx();
	Vec3 vPos = m_pPlayer->MeshRender()->GetMesh()->BonePosSkinning(iWeaponHandIdx, m_pPlayer->Animator3D());

	vPos = XMVector3TransformCoord(vPos, m_pPlayer->Transform()->GetWorldMat());

	return vPos;
}
