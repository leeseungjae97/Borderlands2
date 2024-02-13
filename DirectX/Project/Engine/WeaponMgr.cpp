#include "pch.h"
#include "WeaponMgr.h"

#include "CAnimator3D.h"
#include "CEventMgr.h"
#include "CGameObject.h"
#include "CLayer.h"
#include "CLevel.h"
#include "CLevelMgr.h"
#include "CMeshRender.h"
#include "CTransform.h"

WeaponMgr::WeaponMgr()
	: iCurWeaponIdx(0)
	, iCurWeaponName(L"smg")
{
}

WeaponMgr::~WeaponMgr()
{
}

void WeaponMgr::ChangeWeapon(int _Idx)
{
	iCurWeaponIdx = _Idx;
	iCurWeaponName = m_arrWeapons[iCurWeaponIdx]->GetName();
}

void WeaponMgr::AddWeapon(CGameObject* _Weapon)
{
	if (nullptr == _Weapon) return;

	wstring weaponName = _Weapon->GetName();
	if(weaponName == L"smg")
	{
		m_arrWeapons[SMG_IDX] = _Weapon;
	}
	if (weaponName == L"sniper")
	{
		m_arrWeapons[SNIPER_IDX] = _Weapon;
	}
	if (weaponName == L"pistol")
	{
		m_arrWeapons[PISTOL_IDX] = _Weapon;
	}
}

CGameObject* WeaponMgr::GetCurWeapon()
{
	return m_arrWeapons[iCurWeaponIdx];
}

Vec3 WeaponMgr::GetCurWeaponMuzzlePos()
{
	//int iMuzzleIdx = m_arrWeapons[iCurWeaponIdx]->Animator3D()->GetWeaponMuzzleIdx();
	Vec3 vPos = m_arrWeapons[iCurWeaponIdx]->Animator3D()->GetMuzzlePos();
	//Vec3 vPos = m_pPlayer->MeshRender()->GetMesh()->BonePosSkinning(iWeaponHandIdx, m_pPlayer->Animator3D());

	vPos = XMVector3TransformCoord(vPos, m_arrWeapons[iCurWeaponIdx]->Transform()->GetWorldMat());
	return vPos;
}

void WeaponMgr::Play(GUN_ANIMATION_TYPE _Type, bool _Loop)
{

	if (nullptr == m_arrWeapons[iCurWeaponIdx])
		return;

	m_arrWeapons[iCurWeaponIdx]->Animator3D()->Play((UINT)_Type, _Loop);
}

void WeaponMgr::tick()
{
	if(CEventMgr::GetInst()->IsLevelChanged())
	{
		CLevel* curLevel = CLevelMgr::GetInst()->GetCurLevel();
		CLayer* layer =curLevel->GetLayer(4);
		for (int i = 0; i < layer->GetObjects().size(); ++i)
		{
			AddWeapon(layer->GetObjects()[i]);
		}
	}
}

void WeaponMgr::begin()
{

	for(int i = 0 ; i < (int)GUN_ANIMATION_TYPE::END; ++i)
	{
		m_arrWeapons[SMG_IDX]->Animator3D()->StartEvent(i)
			= std::make_shared<std::function<void()>>([=]()
		{
			m_arrWeapons[SMG_IDX]->Animator3D()->Proceed();
		});
	}
	m_arrWeapons[SMG_IDX]->Animator3D()->EndEvent((UINT)GUN_ANIMATION_TYPE::RELOAD)
		= std::make_shared<std::function<void()>>([=]()
			{
				m_arrWeapons[SMG_IDX]->Animator3D()->StopPlay();
			});


}
