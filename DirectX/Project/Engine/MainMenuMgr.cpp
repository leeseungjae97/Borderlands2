#include "pch.h"
#include "MainMenuMgr.h"

#include "CEventMgr.h"
#include "CLayer.h"
#include "CLevel.h"
#include "CLevelMgr.h"
#include "CMeshRender.h"
#include "CResMgr.h"
#include "CTransform.h"
#include "CUI.h"

MainMenuMgr::MainMenuMgr()
	: m_pHoverImg(nullptr)
{
}

MainMenuMgr::~MainMenuMgr()
{
}

void MainMenuMgr::tick()
{
	if (CLevelMgr::GetInst()->GetCurLevel()->GetName() != "main menu level")
		return;

	if(nullptr == m_pHoverImg)
	{
		makeHoverImg();
	}

	reload();

	bool bHover = false;
	for(int i = 0 ; i < m_vecMainMenuCUI.size(); ++i )
	{
		CUI* cui = m_vecMainMenuCUI[i];
		if(cui->IsHover())
		{
			bHover = true;
			Vec3 vPos = cui->Transform()->GetRelativePos();
			Vec3 vScale = cui->Transform()->GetRelativeScale();
			vPos.x -= vScale.x / 2.f;
			m_pHoverImg->Transform()->SetRelativePos(vPos);
			m_pHoverImg->SetObjectState(CGameObject::OBJECT_STATE::VISIBLE);
		}
	}
	if(!bHover)
	{
		m_pHoverImg->SetObjectState(CGameObject::OBJECT_STATE::INVISIBLE);
	}
}

void MainMenuMgr::makeHoverImg()
{
	Ptr<CTexture> tex = CResMgr::GetInst()->FindRes<CTexture>(L"texture\\UI\\btn_hover.png");
	m_pHoverImg = new CGameObject();
	m_pHoverImg->SetName(L"main menu hover image");
	m_pHoverImg->AddComponent(new CTransform);
	m_pHoverImg->AddComponent(new CMeshRender);
	
	m_pHoverImg->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	m_pHoverImg->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"MainMenuHoverMtrl"), 0);
	m_pHoverImg->MeshRender()->GetMaterial(0)->SetTexParam(TEX_0, tex);

	m_pHoverImg->Transform()->SetRelativeScale(Vec3(tex->Width(), tex->Height(), 1.f));
	m_pHoverImg->Transform()->SetRelativeScale(Vec3(100.f, 100.f, 1.f));

	PreloadGameObject(m_pHoverImg, Vec3(0.f, 0.f, 0.f), LAYER_TYPE::ViewPortUI);

	m_pHoverImg->SetObjectState(CGameObject::OBJECT_STATE::INVISIBLE);
}

void MainMenuMgr::reload()
{
	if (CEventMgr::GetInst()->IsLevelLoad())
	{
		m_vecMainMenuCUI.clear();
		CLevel* plevel = CLevelMgr::GetInst()->GetCurLevel();
		vector<CGameObject*> vec = plevel->GetLayer((UINT)LAYER_TYPE::ViewPortUI)->GetObjects();
		for (int i = 0; i < vec.size(); ++i)
		{
			CUI* cui = static_cast<CUI*>(vec[i]);
			if (cui)
			{
				if (cui->IsUseHover())
				{
					m_vecMainMenuCUI.push_back(cui);
				}
			}
		}
	}
}
