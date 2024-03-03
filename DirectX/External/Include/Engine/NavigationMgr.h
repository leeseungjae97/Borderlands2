#pragma once
#include "CSingleton.h"
class RecastNavi;

class NavigationMgr
	: public CSingleton<NavigationMgr>
{
private:
	RecastNavi* m_pRecast;
	vector<CGameObject*> m_vecMapObj;
	CGameObject* m_pMapObj;
	int m_iMap;
	wstring m_wsMapName;
public:
	void init();
	void tick();

	void AddMap(CGameObject* _MapObj) { m_vecMapObj.push_back(_MapObj); }
	CGameObject* GetMap(int _Idx);

	void MapChange(int _Idx);

	void SetMap(int _Idx) { m_iMap = _Idx; }

	void MakeMapQuery(class dtNavMeshQuery* _Query);

	bool DoQuery(Vec3 vStart, Vec3 vEnd, vector<Vec3>* vec, int* pathSize);
	Vec3 GetMapRaycastPos();

	void DrawDebug();

public:
	NavigationMgr();
	virtual ~NavigationMgr();
};

