#include "pch.h"
#include "CEngine.h"

#include "CDevice.h"
#include "CPathMgr.h"
#include "CKeyMgr.h"
#include "CTimeMgr.h"
#include "CResMgr.h"
#include "CLevelMgr.h"
#include "CRenderMgr.h"
#include "CEventMgr.h"
#include "CSimpleTextMgr.h"
#include "CInstancingBuffer.h"
#include "InstancingAnimatorMgr.h"
#include "FieldUIMgr.h"
#include "KeyUseInfoMgr.h"
#include "MainMenuMgr.h"
#include "MapMgr.h"
#include "NavigationMgr.h"
#include "ObjPickerMgr.h"
#include "ParticleMgr.h"
#include "PhysXMgr.h"
#include "PlayerMgr.h"
#include "RandMgr.h"
#include "RaycastMgr.h"
#include "SoundMgr.h"
#include "TextMgr.h"
#include "ThreadMgr.h"
#include "WeaponMgr.h"

#include "../Client/CreateLevels.h"

CEngine::CEngine()
	: m_hWnd(nullptr)
{
}

CEngine::~CEngine()
{

}

int CEngine::init(HWND _hWnd, UINT _iWidth, UINT _iHeight)
{
	m_hWnd = _hWnd;
	m_vResolution = Vec2((float)_iWidth, (float)_iHeight);

	RECT rt = { 0, 0, (int)_iWidth, (int)_iHeight};
	AdjustWindowRect(&rt, WS_OVERLAPPEDWINDOW, false);
	SetWindowPos(m_hWnd, nullptr, 10, 10, rt.right - rt.left, rt.bottom - rt.top, 0);
	ShowWindow(m_hWnd, true);

	m_DefaultCursor = GetCursor();
	GetClientRect(m_hWnd, &rt);

	if (FAILED(CDevice::GetInst()->init(m_hWnd, _iWidth, _iHeight)))
	{
		MessageBox(nullptr, L"Device", L"Fail", MB_OK);
		return E_FAIL;
	}

	CPathMgr::GetInst()->init();

	CResMgr::GetInst()->init();

	CKeyMgr::GetInst()->init();

	CTimeMgr::GetInst()->init();

	CRenderMgr::GetInst()->init();

	CSimpleTextMgr::GetInst()->init();

	TextMgr::GetInst()->init();

	CLevelMgr::GetInst()->init();

	CInstancingBuffer::GetInst()->init();

	InstancingAnimatorMgr::GetInst()->init();

	RandMgr::GetInst()->init();

	NavigationMgr::GetInst()->init();

	ParticleMgr::GetInst()->init();

	SoundMgr::GetInst()->init();

	CResMgr::GetInst()->RoadResource();

	return S_OK;
}

void CEngine::progress()
{
	//firsttick();
	tick();
	finaltick();
	render();

	CEventMgr::GetInst()->fixedtick();
	fixedtick();
}

void CEngine::begin()
{
	CLevelMgr::GetInst()->begin();
}
void CEngine::firsttick()
{
	
}

void CEngine::tick()
{
	// Manager Tick
	CTimeMgr::GetInst()->tick();
	CKeyMgr::GetInst()->tick();
	CResMgr::GetInst()->tick();
	ThreadMgr::GetInst()->tick();

	CLevelMgr::GetInst()->tick();
	RaycastMgr::GetInst()->tick();
	NavigationMgr::GetInst()->tick();
	
	PlayerMgr::GetInst()->tick();
	WeaponMgr::GetInst()->tick();
	MapMgr::GetInst()->tick();

	MainMenuMgr::GetInst()->tick();
	FieldUIMgr::GetInst()->tick();

	SoundMgr::GetInst()->tick();

	CRenderMgr::GetInst()->tick();
}
void CEngine::finaltick()
{
	//CResMgr::GetInst()->finaltick();
	CLevelMgr::GetInst()->finaltick();
	CRenderMgr::GetInst()->finaltick();
}
void CEngine::fixedtick()
{
	if (CTimeMgr::GetInst()->GetTime() <= beforeFixedUpdateTime + fixedCycleTime)
	{
		beforeFixedUpdateTime = CTimeMgr::GetInst()->GetTime();
	}
	else
		return;

	ObjPickerMgr::GetInst()->fixedtick();
	PhysXMgr::GetInst()->fixedTick();
}

void CEngine::render()
{	
	CRenderMgr::GetInst()->render();
	CTimeMgr::GetInst()->render();
}
