#pragma once

#include "CSingleton.h"

class CLandScape;
class CLandScapeMgr
	: public CSingleton<CLandScapeMgr>
{
	SINGLE(CLandScapeMgr)
private:
	vector<CLandScape*> m_vecLandScape;

public:
	void ClearLand();
	void AddLandScape(CLandScape* _LS) { m_vecLandScape.push_back(_LS); }
	void DeleteLandScape(CLandScape* _LS);
	Vec3 GetLandScapeCrossPos();

};