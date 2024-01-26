#include "pch.h"
#include "CLevel.h"

#include "CLayer.h"
#include "CGameObject.h"

#include "CRenderMgr.h"

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
}

CLevel::~CLevel()
{
	Safe_Del_Array(m_arrLayer);

	if(m_PxCollisionCallBack)
		delete m_PxCollisionCallBack;
	
	PX_RELEASE(m_PxScene);
}


void CLevel::begin()
{
	if (m_State == LEVEL_STATE::NO_UPDATE_RENDER) return;

	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		m_arrLayer[i]->begin();
	}
}

void CLevel::tick()
{
	Matrix worldMat = physx::Util::WorldMatFromGlobalPose(box->getGlobalPose(), Vec3(20000.f, 10.f, 20000.f));
	DrawDebugCube(worldMat, Vec4(0.f, 1.f, 0.f, 1.f), 0.f, true);

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
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
	sceneDesc.cpuDispatcher = PhysXMgr::GetInst()->GDispatcher();
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;
	sceneDesc.simulationEventCallback = m_PxCollisionCallBack;

	m_PxScene = PhysXMgr::GetInst()->GPhysics()->createScene(sceneDesc);

	PxPvdSceneClient* pvdClient = m_PxScene->getScenePvdClient();
	if (pvdClient)
	{
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}

	PxMaterial* m_PxMaterial = PhysXMgr::GetInst()->GPhysics()->createMaterial(0.5f, 0.5f, 0.6f);

	m_PxMaterial->setFrictionCombineMode(static_cast<physx::PxCombineMode::Enum>(physx::PxCombineMode::eAVERAGE));
	m_PxMaterial->setRestitutionCombineMode(static_cast<physx::PxCombineMode::Enum>(physx::PxCombineMode::eAVERAGE));

	box = PhysXMgr::GetInst()->GPhysics()->createRigidStatic(PxTransform(PxVec3(-200, 0, -200)));
	PxShape* shape = createTriggerShape(physx::PxBoxGeometry(20000.f, 10.f, 20000.f), *m_PxMaterial, Util::FILTER_SHADER, true);
	//PxShape* shape = gPhysics->createShape(physx::PxBoxGeometry(20000.f, 10.f, 20000.f), *m_PxMaterial, true);
	//TriggerImpl 
	box->attachShape(*shape);

	m_PxScene->addActor(*box);
}

CLayer* CLevel::FindLayerByName(const wstring& _strName)
{
	for (int i = 0; i < MAX_LAYER; ++i)
	{
		if (m_arrLayer[i]->GetName() == _strName)
			return m_arrLayer[i];
	}

	return nullptr;
}


void CLevel::AddGameObject(CGameObject* _Object, int _iLayerIdx, bool _bMove)
{
	m_arrLayer[_iLayerIdx]->AddGameObject(_Object, _bMove);
}

void CLevel::AddGameObject(CGameObject* _Object, const wstring& _LayerName, bool _Move)
{
	CLayer* pLayer = FindLayerByName(_LayerName);
	assert(pLayer);

	pLayer->AddGameObject(_Object, _Move);
}

void CLevel::ChangeState(LEVEL_STATE _State)
{	
	m_State = _State;

	if (LEVEL_STATE::PLAY == m_State)
	{
		CRenderMgr::GetInst()->SetRenderFunc(true);
		begin();
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

