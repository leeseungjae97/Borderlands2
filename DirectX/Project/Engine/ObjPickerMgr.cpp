#include "pch.h"
#include "ObjPickerMgr.h"

#include "CEventMgr.h"

ObjPickerMgr::ObjPickerMgr()
	: m_pPickObj(nullptr)
{
}

ObjPickerMgr::~ObjPickerMgr()
{
}

void ObjPickerMgr::tick()
{
	if(CEventMgr::GetInst()->IsLevelChanged())
	{
		m_pPickObj = nullptr;
	}
}
