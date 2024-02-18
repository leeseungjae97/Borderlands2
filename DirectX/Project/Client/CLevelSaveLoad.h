#pragma once

class CLevel;
class CLayer;
class CGameObject;


class CLevelSaveLoad
{
public:
	static int SaveLevel(const wstring& _LevelPath, CLevel* _Level);
	static int SaveGameObject(CGameObject* _Object, FILE* _File, bool _Root);

	static CLevel* LoadLevel(const wstring& _LevelPath);
	static CGameObject* LoadGameObject(FILE* _File, CLevel* _NewLevel, int _LayerIdx, bool _Root);
};

