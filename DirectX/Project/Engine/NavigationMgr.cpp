#include "pch.h"
#include "NavigationMgr.h"

#include "CCamera.h"
#include "CEventMgr.h"
#include "CLayer.h"
#include "CLevel.h"
#include "CLevelMgr.h"
#include "CRenderMgr.h"
#include "CTransform.h"
#include "MouseMgr.h"
#include "RecastNavi.h"

NavigationMgr::NavigationMgr()
	: m_iMap(0)
	, m_pRecast(nullptr)
	, m_pMapObj(nullptr)
{
	m_pRecast = new RecastNavi;
}

NavigationMgr::~NavigationMgr()
{
	if(m_pRecast)
	{
		delete m_pRecast;
		m_pRecast = nullptr;
	}
}

void NavigationMgr::init()
{
	
}

void NavigationMgr::tick()
{
	if(CEventMgr::GetInst()->IsLevelLoad())
	{
		m_vecMapObj.clear();
		vector<CGameObject*>().swap(m_vecMapObj);

		CLevel* pLevel = CLevelMgr::GetInst()->GetCurLevel();
		vector<CGameObject*> maps = pLevel->FindLayerByType(LAYER_TYPE::Terrain)->GetObjects();
		for (int i = 0; i < maps.size(); ++i)
		{
			m_vecMapObj.push_back(maps[i]);
		}

		MapChange(m_iMap);
	}
	else
	{
		if(nullptr == m_pMapObj)
		{
			m_vecMapObj.clear();
			vector<CGameObject*>().swap(m_vecMapObj);

			CLevel* pLevel = CLevelMgr::GetInst()->GetCurLevel();
			vector<CGameObject*> maps = pLevel->FindLayerByType(LAYER_TYPE::Terrain)->GetObjects();
			for (int i = 0; i < maps.size(); ++i)
			{
				m_vecMapObj.push_back(maps[i]);
			}

			MapChange(m_iMap);
		}
	}
	DrawDebug();
}

CGameObject* NavigationMgr::GetMap(int _Idx)
{
	if (_Idx >= m_vecMapObj.size())
		return nullptr;

	return m_vecMapObj[_Idx];
}

void NavigationMgr::MapChange(int _Idx)
{
	if (_Idx >= m_vecMapObj.size() || m_vecMapObj.empty())
		return;

	m_iMap = _Idx;

	if (nullptr == m_pMapObj || m_pMapObj != m_vecMapObj[m_iMap])
	{
		m_pMapObj = m_vecMapObj[m_iMap];
	}
	m_wsMapName = m_pMapObj->GetName();
	m_pRecast->HandleBuild(m_pMapObj);
}

void NavigationMgr::MakeMapQuery(dtNavMeshQuery* _Query)
{
	assert(m_pRecast);

	m_pRecast->MakeQuery(_Query);
	
}

bool NavigationMgr::DoQuery(Vec3 vStart, Vec3 vEnd, vector<Vec3>* vec, int* pathSize)
{
	if(m_pRecast->DoQuery(vStart, vEnd))
	{
		*vec = m_pRecast->GetStraightPath();
		*pathSize = m_pRecast->GetStraightPathSize();

		return true;
	}
	return false;
}

Vec3 NavigationMgr::GetMapRaycastPos()
{
	Vec3 vRayStartPos = CRenderMgr::GetInst()->GetMainCam()->GetRay().vStart;
	Vec3 vRayEndPos = MouseMgr::GetInst()->MousePosRay();
	float hitTime;
	if (m_pRecast->Raycast(vRayStartPos, vRayEndPos, &hitTime))
	{
		Vec3 pos;
		pos.x = vRayStartPos.x + (vRayEndPos.x - vRayStartPos.x) * hitTime;
		pos.y = vRayStartPos.y + (vRayEndPos.y - vRayStartPos.y) * hitTime;
		pos.z = vRayStartPos.z + (vRayEndPos.z - vRayStartPos.z) * hitTime;
		return pos;
	}
	return Vec3::Zero;
}

void NavigationMgr::DrawDebug()
{
	if (nullptr == m_pMapObj)
		return;

	Matrix World= m_pMapObj->Transform()->GetWorldMat();
	DrawDebugMesh(World, L"NaviMesh", 1, Vec4(1.f, 1.f, 1.f, 1.f), 0.f, true);
}
