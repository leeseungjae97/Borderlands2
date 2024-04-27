#include "pch.h"
#include "MapMgr.h"

#include "CCamera.h"
#include "CEventMgr.h"
#include "CGameObject.h"
#include "CLayer.h"
#include "CLevel.h"
#include "CLevelMgr.h"
#include "CMeshRender.h"
#include "CRenderMgr.h"
#include "CResMgr.h"
#include "CTransform.h"
#include "physx_util.h"

MapMgr::MapMgr()
	: m_pUI_Map(nullptr)
{
}

MapMgr::~MapMgr()
{
}

void MapMgr::tick()
{
	if (CRenderMgr::GetInst()->GetMainCam() && CRenderMgr::GetInst()->GetMainCam()->IsCinematic())
	{
		if (m_pUI_Map)
			m_pUI_Map->SetObjectState(CGameObject::OBJECT_STATE::INVISIBLE);
		if (m_pUI_MapMarker)
			m_pUI_MapMarker->SetObjectState(CGameObject::OBJECT_STATE::INVISIBLE);
		if (m_pUI_MapMarkerRange)
			m_pUI_MapMarkerRange->SetObjectState(CGameObject::OBJECT_STATE::INVISIBLE);
	}else
	{
		if(CLevelMgr::GetInst()->GetCurLevel()->GetName() != L"main menu level"
			&& CLevelMgr::GetInst()->GetCurLevel()->GetName() != L"loading")
		{
			if (m_pUI_Map)
				m_pUI_Map->SetObjectState(CGameObject::OBJECT_STATE::VISIBLE);
			if (m_pUI_MapMarker)
				m_pUI_MapMarker->SetObjectState(CGameObject::OBJECT_STATE::VISIBLE);
			if (m_pUI_MapMarkerRange)
				m_pUI_MapMarkerRange->SetObjectState(CGameObject::OBJECT_STATE::VISIBLE);
		}
	}
}

void MapMgr::init()
{
}

void MapMgr::begin()
{
	Ptr<CMaterial> pMtrl = nullptr;
	{
		if(nullptr == m_pUI_Map)
		{
			pMtrl = new CMaterial(true);
			pMtrl->SetShader(CResMgr::GetInst()->FindRes<CGraphicsShader>(L"MapShader"));
			CResMgr::GetInst()->AddRes(L"UIMapMtrl", pMtrl);

			m_pUI_Map = new CGameObject;

			m_pUI_Map->SetName(L"UI Map");
			m_pUI_Map->AddComponent(new CTransform);
			m_pUI_Map->AddComponent(new CMeshRender);

			m_pUI_Map->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
			m_pUI_Map->MeshRender()->SetMaterial(pMtrl, 0);

			m_pUI_Map->MeshRender()->GetMaterial(0)->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"texture\\UI\\map.png"));
			m_pUI_Map->MeshRender()->GetMaterial(0)->SetTexParam(TEX_1, CResMgr::GetInst()->FindRes<CTexture>(L"texture\\UI\\map_mask.png"));
			m_pUI_Map->MeshRender()->GetMaterial(0)->SetTexParam(TEX_2, CResMgr::GetInst()->FindRes<CTexture>(L"RTCopyTex2"));

			m_pUI_Map->Transform()->SetRelativeScale(Vec3(250.f, 200.f, 1.f));
			m_pUI_Map->Transform()->SetRelativeRot(Vec3(0.f, 0.f, 3.f * Util::DegToRad()));
			SpawnGameObject(m_pUI_Map, Vec3(517.f, 250.f, 0.f), LAYER_TYPE::ViewPortUI);
		}
	}
	{
		if(nullptr == m_pUI_MapMarker)
		{
			pMtrl = new CMaterial(true);
			pMtrl->SetShader(CResMgr::GetInst()->FindRes<CGraphicsShader>(L"UI2DShader"));
			CResMgr::GetInst()->AddRes(L"MapMarkerMtrl", pMtrl);
			pMtrl->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"texture\\UI\\Waypoint_Marker.png"));

			m_pUI_MapMarker = new CGameObject;

			m_pUI_MapMarker->SetName(L"UIMapMarker");
			m_pUI_MapMarker->AddComponent(new CTransform);
			m_pUI_MapMarker->AddComponent(new CMeshRender);

			m_pUI_MapMarker->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
			m_pUI_MapMarker->MeshRender()->SetMaterial(pMtrl, 0);

			m_pUI_MapMarker->Transform()->SetRelativeScale(Vec3(20.f, 20.f, 1.f));
			SpawnGameObject(m_pUI_MapMarker, Vec3(517.f, 254.f, 0.f), LAYER_TYPE::ViewPortUI);
		}
	}
	{
		if(nullptr == m_pUI_MapMarkerRange)
		{
			pMtrl = new CMaterial(true);
			pMtrl->SetShader(CResMgr::GetInst()->FindRes<CGraphicsShader>(L"UI2DShader"));
			CResMgr::GetInst()->AddRes(L"MapMarkerRangeMtrl", pMtrl);
			pMtrl->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"texture\\UI\\player_sight.png"));

			m_pUI_MapMarkerRange = new CGameObject;

			m_pUI_MapMarkerRange->SetName(L"UI Marker Range");
			m_pUI_MapMarkerRange->AddComponent(new CTransform);
			m_pUI_MapMarkerRange->AddComponent(new CMeshRender);

			m_pUI_MapMarkerRange->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
			m_pUI_MapMarkerRange->MeshRender()->SetMaterial(pMtrl, 0);

			m_pUI_MapMarkerRange->Transform()->SetRelativeScale(Vec3(100.f, 270.f, 1.f));
			m_pUI_MapMarkerRange->Transform()->SetRelativeRot(Vec3(0.f, 0.f, 90.f * Util::DegToRad()));
			SpawnGameObject(m_pUI_MapMarkerRange, Vec3(517.f, 310.f, 0.f), LAYER_TYPE::ViewPortUI);
		}
	}

	if (m_pUI_Map)
		m_pUI_Map->SetObjectState(CGameObject::OBJECT_STATE::INVISIBLE);
	if (m_pUI_MapMarker)
		m_pUI_MapMarker->SetObjectState(CGameObject::OBJECT_STATE::INVISIBLE);
	if (m_pUI_MapMarkerRange)
		m_pUI_MapMarkerRange->SetObjectState(CGameObject::OBJECT_STATE::INVISIBLE);
}
