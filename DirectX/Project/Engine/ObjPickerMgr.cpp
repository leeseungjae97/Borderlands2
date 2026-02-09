#include "pch.h"
#include "ObjPickerMgr.h"

#include "CEventMgr.h"
#include "CGameObject.h"
#include "CLevel.h"
#include "CLevelMgr.h"

ObjPickerMgr::ObjPickerMgr()
	: m_pPickObj(nullptr)
	, m_wsPickObjName(L"")
{
}

ObjPickerMgr::~ObjPickerMgr()
{
}

void ObjPickerMgr::fixedtick()
{
	
	if(CEventMgr::GetInst()->IsLevelChanged())
	{
		m_pPickObj = nullptr;
	}
	if (nullptr == m_pPickObj)
	{
		CLevel* curLevel = CLevelMgr::GetInst()->GetCurLevel();
		m_pPickObj = curLevel->FindObjectByName(m_wsPickObjName);
	}
}

void ObjPickerMgr::SetPickObj(CGameObject* _PickObj)
{
	m_pPickObj = _PickObj;
	m_wsPickObjName = m_pPickObj->GetName();
}
