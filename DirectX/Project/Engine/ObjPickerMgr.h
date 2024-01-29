#pragma once

#include "CSingleton.h"

class CGameObject;
class ObjPickerMgr
	: public CSingleton<ObjPickerMgr>
{
	SINGLE(ObjPickerMgr)
private:
	CGameObject* m_pPickObj;

public:
	CGameObject* GetPickObj() { return m_pPickObj; }
	void SetPickObj(CGameObject* _PickObj) { m_pPickObj = _PickObj; }
};

