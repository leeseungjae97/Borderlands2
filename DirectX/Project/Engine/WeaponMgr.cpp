#include "pch.h"
#include "WeaponMgr.h"

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
		m_arrWeaponMuzzleIdx[SMG_IDX] = SMG_MUZZLE_IDX;
	}
	if (weaponName == L"sniper")
	{
		m_arrWeapons[SNIPER_IDX] = _Weapon;
		m_arrWeaponMuzzleIdx[SNIPER_IDX] = SNIPER_MUZZLE_IDX;
	}
	if (weaponName == L"pistol")
	{
		m_arrWeapons[PISTOL_IDX] = _Weapon;
		m_arrWeaponMuzzleIdx[PISTOL_IDX] = PISTOL_MUZZLE_IDX;
	}
}

CGameObject* WeaponMgr::GetCurWeapon()
{
	return m_arrWeapons[iCurWeaponIdx];
}

Vec3 WeaponMgr::GetCurWeaponMuzzlePos()
{
	Ptr<CMesh> mesh = m_arrWeapons[iCurWeaponIdx]->MeshRender()->GetMesh();
	Matrix matWorld = m_arrWeapons[iCurWeaponIdx]->Transform()->GetWorldMat();
	int muzzleIdx = m_arrWeaponMuzzleIdx[iCurWeaponIdx];
	int vertexIdx = 0;

	tIndexInfo indexInfo = mesh->GetIndexInfo()[0];
	vector<UINT> inds;
	inds.clear();
	vector<UINT>().swap(inds);
	inds.resize(indexInfo.iIdxCount);
	memcpy(inds.data(), indexInfo.pIdxSysMem, indexInfo.iIdxCount * sizeof(UINT));

	if (inds.size() < muzzleIdx)
	{
		OutputDebugStringW(L"Muzzle Index Error");
		vertexIdx = 0;
	}
	else
		vertexIdx = inds[muzzleIdx];

	if (mesh->GetVtxCount() < vertexIdx)
	{
		OutputDebugStringW(L"Muzzle Vertex Error");
		return Vec3::Zero;
		
	}
	Vec3 vPos = XMVector3TransformCoord(mesh->GetVtxSysMem()[vertexIdx].vPos, matWorld);
	vPos.z += m_arrWeapons[iCurWeaponIdx]->Transform()->GetRelativeScale().z / 2.f;
	return vPos;
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
