#include "pch.h"
#include "CScriptMgr.h"

#include "CBulletScript.h"
#include "CCameraMoveScript.h"
#include "CCrossHairScript.h"
#include "CGunScript.h"
#include "CMissileScript.h"
#include "CEnemyScript.h"
#include "CPlayerScript.h"
#include "CWarriorScript.h"

void CScriptMgr::GetScriptInfo(vector<wstring>& _vec)
{
	_vec.push_back(L"CBulletScript");
	_vec.push_back(L"CCameraMoveScript");
	_vec.push_back(L"CCrossHairScript");
	_vec.push_back(L"CMissileScript");
	_vec.push_back(L"CEnemyScript");
	_vec.push_back(L"CWarriorScript");
	_vec.push_back(L"CPlayerScript");
	_vec.push_back(L"CGunScript");
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
	if (L"CGunScript" == _strScriptName)
		return new CGunScript;
	return nullptr;
}

CScript * CScriptMgr::GetScript(UINT _iScriptType)
{
	switch (_iScriptType)
	{
	case (UINT)SCRIPT_TYPE::BULLETSCRIPT:
		return new CBulletScript;
		break;
	case (UINT)SCRIPT_TYPE::CAMERAMOVESCRIPT:
		return new CCameraMoveScript;
		break;
	case (UINT)SCRIPT_TYPE::CROSSHAIRSCRIPT:
		return new CCrossHairScript;
		break;
	case (UINT)SCRIPT_TYPE::MISSILESCRIPT:
		return new CMissileScript;
		break;
	case (UINT)SCRIPT_TYPE::ENEMYSCRIPT:
		return new CEnemyScript;
		break;
	case (UINT)SCRIPT_TYPE::WARRIORSCRIPT:
		return new CWarriorScript;
		break;
	case (UINT)SCRIPT_TYPE::PLAYERSCRIPT:
		return new CPlayerScript;
		break;
	case (UINT)SCRIPT_TYPE::GUNSCRIPT:
		return new CGunScript;
		break;

		//FIREBREATHSCRIPT
	}
	return nullptr;
}

const wchar_t * CScriptMgr::GetScriptName(CScript * _pScript)
{
	switch ((SCRIPT_TYPE)_pScript->GetScriptType())
	{
	case SCRIPT_TYPE::BULLETSCRIPT:
		return L"CBulletScript";
		break;

	case SCRIPT_TYPE::CAMERAMOVESCRIPT:
		return L"CCameraMoveScript";
		break;

	case SCRIPT_TYPE::CROSSHAIRSCRIPT:
		return L"CCrossHairScript";
		break;

	case SCRIPT_TYPE::MISSILESCRIPT:
		return L"CMissileScript";
		break;

	case SCRIPT_TYPE::ENEMYSCRIPT:
		return L"CEnemyScript";
		break;

	case SCRIPT_TYPE::WARRIORSCRIPT:
		return L"CWarriorScript";
		break;

	case SCRIPT_TYPE::PLAYERSCRIPT:
		return L"CPlayerScript";
		break;

	case SCRIPT_TYPE::GUNSCRIPT:
		return L"CGunScript";
		break;

	}
	return nullptr;
}