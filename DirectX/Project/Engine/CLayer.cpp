#include "pch.h"
#include "CLayer.h"

#include "CGameObject.h"


CLayer::CLayer()
{
}

CLayer::~CLayer()
{
	Safe_Del_Vec(m_vecParentObj);
	//Safe_Del_Vec(m_vecObject);
}

void CLayer::begin()
{
	for (size_t i = 0; i < m_vecParentObj.size(); ++i)
	{
		m_vecParentObj[i]->begin();
	}
}

void CLayer::tick()
{
	for (size_t i = 0; i < m_vecParentObj.size(); ++i)
	{
		if (m_vecParentObj[i]->GetObjectState() == CGameObject::OBJECT_STATE::INVISIBLE)
			continue;
		m_vecParentObj[i]->tick();
	}
}

void CLayer::finaltick()
{
	vector<CGameObject*>::iterator iter = m_vecParentObj.begin();
	vector<CGameObject*> vecDead;
	for (; iter != m_vecParentObj.end(); )
	{
		if ((*iter)->GetObjectState() != CGameObject::OBJECT_STATE::INVISIBLE)
		{
			(*iter)->finaltick();
		}

		if ((*iter)->IsDead())
		{
			vecDead.push_back(*iter);
			iter = m_vecParentObj.erase(iter);
		}
		else
		{
			++iter;
		}
	}



	//for(int i =0  ; i < vecDead.size(); ++i)
	//{
	//	DestroyObject(vecDead[i]);	
	//}
	//vecDead.clear();
	//
}

void CLayer::AddGameObject(CGameObject* _Object, bool _bMove)
{
	m_vecParentObj.push_back(_Object);
	
	// 소유하고 있는 모든 자식오브젝트가 있는지 검사
	static list<CGameObject*> queue;
	queue.clear();

	queue.push_back(_Object);

	while (!queue.empty())
	{
		CGameObject* pObject = queue.front();
		queue.pop_front();

		for (size_t i = 0; i < pObject->m_vecChild.size(); ++i)
		{
			queue.push_back(pObject->m_vecChild[i]);			
		}

		// 부모타입 or 소속 레이어가 없는경우 or 부모와 같이 이동하는 경우
		if(nullptr == pObject->m_Parent || -1 == pObject->m_iLayerIdx || _bMove)
		{
			pObject->m_iLayerIdx = m_iLayerIdx;
		}
			
	}	
}


void CLayer::RemoveFromParentList(CGameObject* _Obj)
{
	vector<CGameObject*>::iterator iter = m_vecParentObj.begin();

	for (; iter != m_vecParentObj.end(); ++iter)
	{
		if ((*iter) == _Obj)
		{
			m_vecParentObj.erase(iter);
			return;
		}
	}

	assert(nullptr);
}

void CLayer::AddParentList(CGameObject* _Obj)
{
	assert(!_Obj->m_Parent);
	m_vecParentObj.push_back(_Obj);
}


