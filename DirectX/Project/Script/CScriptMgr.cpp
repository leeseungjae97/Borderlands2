#include "pch.h"
#include "CScriptMgr.h"

#include "CAttackBurnScript.h"
#include "CAttackNormalScript.h"
#include "CBulletScript.h"
#include "CCameraMoveScript.h"
#include "CConstructorScript.h"
#include "CCrossHairScript.h"
#include "CWeaponScript.h"
#include "CMissileScript.h"
#include "CEnemyScript.h"
#include "CMoveScript.h"
#include "CPlayerScript.h"
#include "CWarriorScript.h"
#include "CPathFindScript.h"

void CScriptMgr::GetScriptInfo(vector<wstring>& _vec)
{
	_vec.push_back(L"CBulletScript");
	_vec.push_back(L"CCameraMoveScript");
	_vec.push_back(L"CCrossHairScript");
	_vec.push_back(L"CMissileScript");
	_vec.push_back(L"CEnemyScript");
	_vec.push_back(L"CWarriorScript");
	_vec.push_back(L"CPlayerScript");
	_vec.push_back(L"CWeaponScript");
	_vec.push_back(L"CAttackBurnScript");
	_vec.push_back(L"CAttackNormalScript");
	_vec.push_back(L"CPathFindScript");
	_vec.push_back(L"CMoveScript");
}

CScript * CScriptMgr::GetScript(const wstring& _strScriptName)
{
	if (L"CBulletScript" == _strScriptName)
		return new CBulletScript;
	if (L"CCameraMoveScript" == _strScriptName)
		return new CCameraMoveScript;
	if (L"CCrossHairScript" == _strScriptName)
		return new CCrossHairScript;
	if (L"CMissileScript" == _strScriptName)
		return new CMissileScript;
	if (L"CEnemyScript" == _strScriptName)
		return new CEnemyScript;
	if (L"CWarriorScript" == _strScriptName)
		return new CWarriorScript;
	if (L"CPlayerScript" == _strScriptName)
		return new CPlayerScript;
	if (L"CWeaponScript" == _strScriptName)
		return new CWeaponScript;
	if (L"CAttackBurnScript" == _strScriptName)
		return new CAttackBurnScript;
	if (L"CAttackNormalScript" == _strScriptName)
		return new CAttackNormalScript;
	if (L"CPathFindScript" == _strScriptName)
		return new CPathFindScript;
	if (L"CMoveScript" == _strScriptName)
		return new CMoveScript;
	if (L"CConstructorScript" == _strScriptName)
		return new CConstructorScript;
	return nullptr;
}

CScript * CScriptMgr::GetScript(UINT _iScriptType)
{
	switch (_iScriptType)
	{
	case (UINT)SCRIPT_TYPE::BULLET_SCRIPT:
		return new CBulletScript;
		break;
	case (UINT)SCRIPT_TYPE::CAMERA_MOVE_SCRIPT:
		return new CCameraMoveScript;
		break;
	case (UINT)SCRIPT_TYPE::CROSSHAIR_SCRIPT:
		return new CCrossHairScript;
		break;
	case (UINT)SCRIPT_TYPE::MISSILE_SCRIPT:
		return new CMissileScript;
		break;
	case (UINT)SCRIPT_TYPE::ENEMY_SCRIPT:
		return new CEnemyScript;
		break;
	case (UINT)SCRIPT_TYPE::WARRIOR_SCRIPT:
		return new CWarriorScript;
		break;
	case (UINT)SCRIPT_TYPE::PLAYER_SCRIPT:
		return new CPlayerScript;
		break;
	case (UINT)SCRIPT_TYPE::WEAPON_SCRIPT:
		return new CWeaponScript;
		break;
	case (UINT)SCRIPT_TYPE::ATTACK_BURN_SCRIPT:
		return new CAttackBurnScript;
		break;
	case (UINT)SCRIPT_TYPE::ATTACK_NORMAL_SCRIPT:
		return new CAttackNormalScript;
		break;
	case (UINT)SCRIPT_TYPE::PATH_FIND_SCRIPT:
		return new CPathFindScript;
		break;
	case (UINT)SCRIPT_TYPE::MOVE_SCRIPT:
		return new CMoveScript;
		break;
	case (UINT)SCRIPT_TYPE::CONSTRUCTOR_SCRIPT:
		return new CConstructorScript;
		break;
		
	}
	return nullptr;
}

const wchar_t * CScriptMgr::GetScriptName(CScript * _pScript)
{
	switch ((SCRIPT_TYPE)_pScript->GetScriptType())
	{
	case SCRIPT_TYPE::BULLET_SCRIPT:
		return L"CBulletScript";
		break;

	case SCRIPT_TYPE::CAMERA_MOVE_SCRIPT:
		return L"CCameraMoveScript";
		break;

	case SCRIPT_TYPE::CROSSHAIR_SCRIPT:
		return L"CCrossHairScript";
		break;

	case SCRIPT_TYPE::MISSILE_SCRIPT:
		return L"CMissileScript";
		break;

	case SCRIPT_TYPE::ENEMY_SCRIPT:
		return L"CEnemyScript";
		break;

	case SCRIPT_TYPE::WARRIOR_SCRIPT:
		return L"CWarriorScript";
		break;

	case SCRIPT_TYPE::PLAYER_SCRIPT:
		return L"CPlayerScript";
		break;

	case SCRIPT_TYPE::WEAPON_SCRIPT:
		return L"CWeaponScript";
		break;

	case SCRIPT_TYPE::ATTACK_BURN_SCRIPT:
		return L"CAttackBurnScript";
		break;

	case SCRIPT_TYPE::ATTACK_NORMAL_SCRIPT:
		return L"CAttackNormalScript";
		break;

	case SCRIPT_TYPE::PATH_FIND_SCRIPT:
		return L"CPathFindScript";
		break;

	case SCRIPT_TYPE::MOVE_SCRIPT:
		return L"CMoveScript";
		break;

	case SCRIPT_TYPE::CONSTRUCTOR_SCRIPT:
		return L"CConstructorScript";
		break;
	}
	return nullptr;
}