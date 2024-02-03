#pragma once
class CameraMgr
	: public CSingleton<CameraMgr>
{
private:
	map<wstring, CGameObject*> m_mapCams;

public:
	void AddCamObj(wstring _Name,CGameObject* _Cam);
	CGameObject* GetCamObj(wstring _Name);

public:
	CameraMgr();
	virtual ~CameraMgr();
};

