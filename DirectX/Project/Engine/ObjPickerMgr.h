#pragma once

#include "CSingleton.h"

class CGameObject;
class ObjPickerMgr
	: public CSingleton<ObjPickerMgr>
{
	SINGLE(ObjPickerMgr)
private:
	CGameObject* m_pPickObj;
	wstring		m_wsPickObjName;
public:
	void fixedtick();

public:
	CGameObject* GetPickObj() { return m_pPickObj; }
	void SetPickObj(CGameObject* _PickObj);
};

