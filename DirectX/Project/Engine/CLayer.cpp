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
		AddCollider3D(m_vecParentObj[i]);
	}
}

void CLayer::tick()
{
	for (size_t i = 0; i < m_vecParentObj.size(); ++i)
	{
		m_vecParentObj[i]->tick();
	}
}

void CLayer::finaltick()
{
	vector<CGameObject*>::iterator iter = m_vecParentObj.begin();
	for (; iter != m_vecParentObj.end(); )
	{
		(*iter)->finaltick();

		if ((*iter)->IsDead())
		{
			iter = m_vecParentObj.erase(iter);
		}
		else
		{
			++iter;
		}
	}	
}

void CLayer::AddGameObject(CGameObject* _Object, bool _bMove)
{
	m_vecParentObj.push_back(_Object);
	
	// �����ϰ� �ִ� ��� �ڽĿ�����Ʈ�� �ִ��� �˻�
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

		// �θ�Ÿ�� or �Ҽ� ���̾ ���°�� or �θ�� ���� �̵��ϴ� ���
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

void CLayer::AddCollider3D(CGameObject* _Obj)
{
	if (nullptr == _Obj) return;

	CCollider3D* col = _Obj->Collider3D();
	if(col)
	{
		m_PxCollisionCallBack->Collisions.insert(make_pair(col->MPxColliderShape(), col));
	}
}


