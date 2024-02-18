#pragma once

#include <vector>
#include <string>

enum SCRIPT_TYPE
{
	GUNSCRIPT,
	BULLETSCRIPT,
	CAMERAMOVESCRIPT,
	CROSSHAIRSCRIPT,
	MISSILESCRIPT,
	ENEMYSCRIPT,
	WARRIORSCRIPT,
	PLAYERSCRIPT,
	FIREBREATHSCRIPT,
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
