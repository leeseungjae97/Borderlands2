#include "pch.h"
#include "WeaponMgr.h"

#include "CAnimator2D.h"
#include "CAnimator3D.h"
#include "CEventMgr.h"
#include "CGameObject.h"
#include "CLayer.h"
#include "CLevel.h"
#include "CLevelMgr.h"
#include "CLight3D.h"
#include "CMeshRender.h"
#include "CResMgr.h"
#include "CRigidBody.h"
#include "CTransform.h"
#include "RandMgr.h"

WeaponMgr::WeaponMgr()
	: iCurWeaponIdx(0)
	, iCurWeaponName(L"smg")
{
}

WeaponMgr::~WeaponMgr()
{
}

bool WeaponMgr::ChangeWeapon(int _Idx, bool _Force)
{
	if (!_Force && iCurWeaponIdx == _Idx)
		return false;

	iCurWeaponIdx = _Idx;
	iCurWeaponName = m_arrWeapons[iCurWeaponIdx]->GetName();
	SetEquis(false);
	SetEqui(iCurWeaponIdx, true);
	Play(GUN_ANIMATION_TYPE::IDLE, false);
	return true;
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

void WeaponMgr::SetEqui(int _Idx, bool _Equi)
{
	m_arrWeapons[_Idx]->SetIsEqui(_Equi);
	if (!_Equi)
		m_arrWeapons[_Idx]->SetObjectState(CGameObject::OBJECT_STATE::INVISIBLE);
	else
		m_arrWeapons[_Idx]->SetObjectState(CGameObject::OBJECT_STATE::VISIBLE);
}

void WeaponMgr::SetEquis(bool _Equi)
{
	for(int i = 0 ; i < 3; ++i)
	{
		SetEqui(i, _Equi);
	}
}

CGameObject* WeaponMgr::GetCurWeapon()
{
	return m_arrWeapons[iCurWeaponIdx];
}



Vec3 WeaponMgr::GetCurWeaponMuzzlePos()
{
	Vec3 vPos = m_arrWeapons[iCurWeaponIdx]->Animator3D()->GetMuzzlePos();
	vPos = XMVector3TransformCoord(vPos, m_arrWeapons[iCurWeaponIdx]->Transform()->GetWorldMat());
	return vPos;
}

Vec3 WeaponMgr::GetWeaponMuzzlePos(CGameObject* _Gun)
{
	Vec3 vPos = _Gun->Animator3D()->GetMuzzlePos();
	vPos = XMVector3TransformCoord(vPos, _Gun->Transform()->GetWorldMat());

	return vPos;
}

Vec3 WeaponMgr::GetOwnerWeaponRot(CGameObject* _Owner)
{
	if (nullptr == _Owner)
		return Vec3::Zero;

	int iWeaponHandRotIdx = _Owner->Animator3D()->GetWeaponHandIdx();

	Vec3 vRot = _Owner->MeshRender()->GetMesh()->BoneRotSkinning(iWeaponHandRotIdx, _Owner->Animator3D());

	return vRot;
}

Vec3 WeaponMgr::GetOwnerWeaponPos(CGameObject* _Owner)
{
	if (nullptr == _Owner)
		return Vec3::Zero;

	int iWeaponHandIdx = _Owner->Animator3D()->GetWeaponHandIdx();
	Vec3 vPos = _Owner->MeshRender()->GetMesh()->BonePosSkinning(iWeaponHandIdx, _Owner->Animator3D());

	CRigidBody* rb = _Owner->RigidBody();
	Vec3 vOffset = Vec3::Zero;
	if (_Owner->Animator3D())
	{
		vOffset += _Owner->Transform()->GetRelativePosOffset();
	}
	vPos = XMVector3TransformCoord(vPos, rb->GetRigidBodyMatrix(vOffset));

	return vPos;
}


void WeaponMgr::Play(GUN_ANIMATION_TYPE _Type, bool _Loop)
{
	if (nullptr == m_arrWeapons[iCurWeaponIdx])
		return;

	m_arrWeapons[iCurWeaponIdx]->Animator3D()->Play((UINT)_Type, _Loop);
}

void WeaponMgr::MuzzleFlash(Vec3 _vPos, Vec3 _vRot)
{
	int randX = RandMgr::GetInst()->GetRandMuzzleX(2);
	int randY = RandMgr::GetInst()->GetRandMuzzleY(2);
	Vec2 muzzleSize = Vec2(512.f / 2.f, 512.f / 2.f);

	
	Ptr<CMaterial> pMtrl = new CMaterial(true);
	pMtrl->SetShader(CResMgr::GetInst()->FindRes<CGraphicsShader>(L"Std3DShader"));
	CResMgr::GetInst()->AddRes(L"GunFireMtrl", pMtrl);

	CGameObject* Light = new CGameObject;
	Light->SetName(L"Muzzle Flash");
	Light->AddComponent(new CMeshRender);
	Light->AddComponent(new CTransform);
	Light->AddComponent(new CLight3D);
	Light->AddComponent(new CAnimator2D);

	Light->Animator2D()->Create(L"muzzle"
		, CResMgr::GetInst()->FindRes<CTexture>(L"texture\\effect\\Tex_Assault_Muzzle_Flash_Front_.tga")
		, Vec2(muzzleSize.x * randX, muzzleSize.y * randY)
		, muzzleSize
		, 1
		, 1
		, Vec2::Zero
		, Vec2::Zero
	);
	Light->Animator2D()->Play(L"muzzle", false);

	Light->Light3D()->SetRadius(100.f);
	Light->Light3D()->SetLightType(LIGHT_TYPE::POINT);
	Light->Light3D()->SetLightColor(Vec3(1.f, 1.f, 0.f));
	Light->Light3D()->SetLightAmbient(Vec3(0.65f, 0.65f, 0.65f));
	Light->Light3D()->SetLifeSpan(0.01f);

	Light->Transform()->SetRelativeRot(_vRot);
	Light->Transform()->SetRelativeScale(Vec3(100.f, 100.f, 100.f));
	Light->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	Light->MeshRender()->SetMaterial(pMtrl, 0);

	SpawnGameObject(Light, _vPos, LAYER_TYPE::Light);
}

void WeaponMgr::tick()
{
	if(CEventMgr::GetInst()->IsLevelChanged())
	{
		
	}
}

void WeaponMgr::begin()
{
	//for(int weaponIdx = 0 ; weaponIdx < 3; ++weaponIdx)
	//{
	//	for (int i = 0; i < (int)GUN_ANIMATION_TYPE::END; ++i)
	//	{
	//		m_arrWeapons[weaponIdx]->Animator3D()->StartEvent(i)
	//			= std::make_shared<std::function<void()>>([=]()
	//				{
	//					m_arrWeapons[weaponIdx]->Animator3D()->Proceed();
	//				});
	//	}
	//	m_arrWeapons[weaponIdx]->Animator3D()->EndEvent((UINT)GUN_ANIMATION_TYPE::RELOAD)
	//	= std::make_shared<std::function<void()>>([=]()
	//		{
	//			m_arrWeapons[weaponIdx]->Animator3D()->StopPlay();
	//		});
	//}
	CLevel* curLevel = CLevelMgr::GetInst()->GetCurLevel();
	CLayer* layer = curLevel->GetLayer((int)LAYER_TYPE::Item);
	for (int i = 0; i < layer->GetObjects().size(); ++i)
	{
		if(!layer->GetObjects()[i]->IsOwned())
			AddWeapon(layer->GetObjects()[i]);
	}
	ChangeWeapon(SMG_IDX, true);
}


