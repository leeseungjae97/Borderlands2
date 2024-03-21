#include "pch.h"
#include "CEventMgr.h"

#include "CLevelMgr.h"
#include "CLevel.h"
#include "CGameObject.h"
#include "CResMgr.h"
#include "CRenderMgr.h"
#include "CRigidBody.h"


CEventMgr::CEventMgr()
	: m_LevelChanged(false)
{

}

CEventMgr::~CEventMgr()
{

}

void CEventMgr::tick()
{
	m_LevelChanged = false;
	m_LevelLoad = false;
	GC_Clear();

	for (size_t i = 0; i < m_vecEvent.size(); ++i)
	{
		switch (m_vecEvent[i].Type)
		{
			// wParam : GameObject, lParam : Layer Index
		case EVENT_TYPE::CREATE_OBJECT:
		{
			CGameObject* NewObject = (CGameObject*)m_vecEvent[i].wParam;
			int iLayerIdx = (int)m_vecEvent[i].lParam;

			CLevelMgr::GetInst()->GetCurLevel()->AddGameObject(NewObject, iLayerIdx, false);
			if (CLevelMgr::GetInst()->GetCurLevel()->GetState() == LEVEL_STATE::PLAY)
			{
				NewObject->begin();
			}

			m_LevelChanged = true;
		}
		break;
		case EVENT_TYPE::DELETE_OBJECT:
		{
			CGameObject* DeleteObject = (CGameObject*)m_vecEvent[i].wParam;

			if (false == DeleteObject->m_bDead)
			{
				DeleteObject->m_bDead = true;
				m_vecGC.push_back(DeleteObject);
			}
		}
		break;

		case EVENT_TYPE::ADD_CHILD:
			// wParam : ParentObject, lParam : ChildObject
		{
			CGameObject* pDestObj = (CGameObject*)m_vecEvent[i].wParam;
			CGameObject* pSrcObj = (CGameObject*)m_vecEvent[i].lParam;

			// 부모로 지정된 오브젝트가 없으면, Child 오브젝트가 최상위 부모 오브젝트가 된다.
			if (nullptr == pDestObj)
			{
				if (pSrcObj->GetParent())
				{
					// 기존 부모와의 연결 해제
					pSrcObj->DisconnectFromParent();
					// 최상위 부모 오브젝트로, 소속 레이어에 등록
					pSrcObj->AddParentList();
				}
			}
			else
			{
				pDestObj->AddChild(pSrcObj);
			}

			m_LevelChanged = true;
		}



		break;
		case EVENT_TYPE::DELETE_RESOURCE:
		{
			RES_TYPE type = (RES_TYPE)m_vecEvent[i].wParam;
			CRes* pRes = (CRes*)m_vecEvent[i].lParam;
			CResMgr::GetInst()->DeleteRes(type, pRes->GetKey());
			m_LevelChanged = true;
		}

		break;
		case EVENT_TYPE::LEVEL_CHANGE:
		{
			CLevel* Level = (CLevel*)m_vecEvent[i].wParam;
			CLevelMgr::GetInst()->ChangeLevel(Level);
			CRenderMgr::GetInst()->ClearCamera();
			m_LevelLoad = true;
			m_LevelChanged = true;
		}
		break;
		case EVENT_TYPE::LEVEL_LOAD:
		{
			CLevel* Level = (CLevel*)m_vecEvent[i].wParam;
			CLevelMgr::GetInst()->LoadLevel(Level);
			CRenderMgr::GetInst()->ClearCamera();
			m_LevelLoad = true;
			m_LevelChanged = true;
		}
		break;
		case EVENT_TYPE::LEVEL_RESET:
		{
			CLevel* Level = (CLevel*)m_vecEvent[i].wParam;
			CLevelMgr::GetInst()->ResetLevel(Level);
			CRenderMgr::GetInst()->ClearCamera();
			m_LevelLoad = true;
			m_LevelChanged = true;
		}
		break;
		case EVENT_TYPE::LEVEL_RECOG:
		{
			m_LevelChanged = true;
		}
		break;
		}
	}

	m_vecEvent.clear();
}


void CEventMgr::GC_Clear()
{
	for (size_t i = 0; i < m_vecGC.size(); ++i)
	{
		if (nullptr != m_vecGC[i])
		{
			// 자식 타입 오브젝트인 경우
			if (m_vecGC[i]->GetParent())
				m_vecGC[i]->DisconnectFromParent();
			if (m_vecGC[i]->Collider3D())
			{
				m_vecGC[i]->Collider3D()->Release();
			}
			if (m_vecGC[i]->RigidBody())
			{
				m_vecGC[i]->RigidBody()->Release();
			}

			delete m_vecGC[i];

			m_LevelChanged = true;
		}
	}
	m_vecGC.clear();
}