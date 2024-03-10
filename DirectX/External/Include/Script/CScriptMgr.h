#pragma once

#include <vector>
#include <string>

enum SCRIPT_TYPE
{
	WEAPON_SCRIPT,
	BULLET_SCRIPT,
	CAMERA_MOVE_SCRIPT,
	CROSSHAIR_SCRIPT,
	MISSILE_SCRIPT,
	ENEMY_SCRIPT,
	WARRIOR_SCRIPT,
	PLAYER_SCRIPT,
	ATTACK_NORMAL_SCRIPT,
	ATTACK_BURN_SCRIPT,
	PATH_FIND_SCRIPT,
	MOVE_SCRIPT,

	END,
};

using namespace std;

class CScript;

class CScriptMgr
{
public:
	static void GetScriptInfo(vector<wstring>& _vec);
	static CScript * GetScript(const wstring& _strScriptName);
	static CScript * GetScript(UINT _iScriptType);
	static const wchar_t * GetScriptName(CScript * _pScript);
};
