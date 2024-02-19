#include "pch.h"
#include "CLevel.h"

#include "CLayer.h"
#include "CGameObject.h"

#include "CRenderMgr.h"
#include "WeaponMgr.h"

CLevel::CLevel()
	: m_arrLayer{}
	, m_State(LEVEL_STATE::STOP)
{
	createScene();

	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		m_arrLayer[i] = new CLayer;
		m_arrLayer[i]->m_iLayerIdx = i;
	}

	m_arrLayer[(int)LAYER_TYPE::Default]->SetName(L"Default");
	m_arrLayer[(int)LAYER_TYPE::Environment]->SetName(L"Environment");
	m_arrLayer[(int)LAYER_TYPE::Enemy]->SetName(L"Enemy");
	m_arrLayer[(int)LAYER_TYPE::Player]->SetName(L"Player");
	m_arrLayer[(int)LAYER_TYPE::Item]->SetName(L"Item");
	m_arrLayer[(int)LAYER_TYPE::PlayerBullet]->SetName(L"PlayerBullet");
	m_arrLayer[(int)LAYER_TYPE::EnemyBullet]->SetName(L"EnemyBullet");
	m_arrLayer[(int)LAYER_TYPE::Missile]->SetName(L"Missile");
	m_arrLayer[(int)LAYER_TYPE::Light]->SetName(L"Light");
	m_arrLayer[(int)LAYER_TYPE::ViewPortUI]->SetName(L"ViewPort UI");
}

CLevel::~CLevel()
{
	Safe_Del_Array(m_arrLayer);

	if(m_PxCollisionCallBack)
		delete m_PxCollisionCallBack;
}


void CLevel::begin()
{
	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		m_arrLayer[i]->begin();
	}
}

void CLevel::tick()
{
	if (m_State == LEVEL_STATE::NO_UPDATE_RENDER) return;

	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		m_arrLayer[i]->tick();
	}
}

void CLevel::finaltick()
{
	if (m_State == LEVEL_STATE::NO_UPDATE_RENDER) return;

	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		m_arrLayer[i]->finaltick();
	}
}

void CLevel::createScene()
{
	m_PxCollisionCallBack = new PxCollisionCallBack;
	PxSceneDesc sceneDesc(PhysXMgr::GetInst()->GPhysics()->getTolerancesScale());
	// 300, 300, 300이 1,1,1같은 물리연산이 되게
	sceneDesc.gravity = PxVec3(0.0f, -2943, 0.0f);
	sceneDesc.cpuDispatcher = PhysXMgr::GetInst()->GDispatcher();
	sceneDesc.filterShader = SimulationFilterShader;
	sceneDesc.simulationEventCallback = m_PxCollisionCallBack;
	//sceneDesc.filterCallback = m_PxTriggerCallBack;
	//sceneDesc.flags |= PxSceneFlag::eENABLE_CCD;

	m_PxScene = PhysXMgr::GetInst()->GPhysics()->createScene(sceneDesc);

	PxPvdSceneClient* pvdClient = m_PxScene->getScenePvdClient();
	if (pvdClient)
	{
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}
	PhysXMgr::GetInst()->InsertScene(this);
	PhysXMgr::GetInst()->SetCurScene(m_PxScene);
}

//CLayer* CLevel::FindLayerByName(const wstring& _strName)
//{
//	for (int i = 0; i < MAX_LAYER; ++i)
//	{
//		if (m_arrLayer[i]->GetName() == _strName)
//			return m_arrLayer[i];
//	}
//
//	return nullptr;
//}

CLayer* CLevel::FindLayerByType(LAYER_TYPE _Type)
{
	return m_arrLayer[(int)_Type];
}


void CLevel::AddGameObject(CGameObject* _Object, int _iLayerIdx, bool _bMove)
{
	m_arrLayer[_iLayerIdx]->AddGameObject(_Object, _bMove);
	//AddCollider3D(_Object);
}

void CLevel::AddGameObject(CGameObject* _Object, LAYER_TYPE _Type, bool _Move)
{
	CLayer* pLayer = m_arrLayer[(int)_Type];
	assert(pLayer);

	pLayer->AddGameObject(_Object, _Move);
	//AddCollider3D(_Object);
}

void CLevel::AddCollider3D(CGameObject* _Object)
{
	//if(_Object->Collider3D())
		//m_PxCollisionCallBack->Collisions.insert(make_pair(_Object->Collider3D()->GetShape(), _Object->Collider3D()));
}

void CLevel::ChangeState(LEVEL_STATE _State)
{	
	m_State = _State;

	if (LEVEL_STATE::PLAY == m_State)
	{
		CRenderMgr::GetInst()->SetRenderFunc(true);
		begin();
		WeaponMgr::GetInst()->begin();
	}
	else
	{
		CRenderMgr::GetInst()->SetRenderFunc(false);
	}
}


void CLevel::clear()
{
	for (int i = 0; i < MAX_LAYER; ++i)
	{
		m_arrLayer[i]->m_vecObject.clear();
	}	
}


CGameObject* CLevel::FindObjectByName(const wstring& _Name)
{
	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		const vector<CGameObject*>& vecObjects = m_arrLayer[i]->GetObjects();
		for (size_t j = 0; j < vecObjects.size(); ++j)
		{
			if (_Name == vecObjects[j]->GetName())
				return vecObjects[j];
		}
	}

	return nullptr;
}

void CLevel::FindObjectByName(const wstring& _Name, vector<CGameObject*>& _Out)
{
	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		const vector<CGameObject*>& vecObjects = m_arrLayer[i]->GetObjects();
		for (size_t j = 0; j < vecObjects.size(); ++j)
		{
			if (_Name == vecObjects[j]->GetName())
				_Out.push_back(vecObjects[j]);
		}
	}
}

