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
#include "FieldUIMgr.h"
#include "KeyUseInfoMgr.h"
#include "MainMenuMgr.h"
#include "NavigationMgr.h"
#include "ObjPickerMgr.h"
#include "ParticleMgr.h"
#include "PhysXMgr.h"
#include "PlayerMgr.h"
#include "RandMgr.h"
#include "RaycastMgr.h"
#include "TextMgr.h"
#include "WeaponMgr.h"

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
		MessageBox(nullptr, L"Device 초기화 실패", L"에러", MB_OK);
		return E_FAIL;
	}

	CPathMgr::GetInst()->init();

	CKeyMgr::GetInst()->init();

	CTimeMgr::GetInst()->init();

	CResMgr::GetInst()->init();

	CRenderMgr::GetInst()->init();

	CSimpleTextMgr::GetInst()->init();

	TextMgr::GetInst()->init();

	CLevelMgr::GetInst()->init();

	CInstancingBuffer::GetInst()->init();

	RandMgr::GetInst()->init();

	NavigationMgr::GetInst()->init();

	ParticleMgr::GetInst()->init();

	return S_OK;
}

void CEngine::progress()
{
	tick();

	render();

	CEventMgr::GetInst()->tick();
	ObjPickerMgr::GetInst()->tick();
	PhysXMgr::GetInst()->fixedTick();
}

void CEngine::tick()
{
	// Manager Tick
	CResMgr::GetInst()->tick();
	CTimeMgr::GetInst()->tick();
	CKeyMgr::GetInst()->tick();	
	// FMOD Update
	CSound::g_pFMOD->update();

	CLevelMgr::GetInst()->tick();
	PlayerMgr::GetInst()->tick();
	WeaponMgr::GetInst()->tick();
	RaycastMgr::GetInst()->tick();
	NavigationMgr::GetInst()->tick();
	FieldUIMgr::GetInst()->tick();

	MainMenuMgr::GetInst()->tick();
}

void CEngine::render()
{	
	CRenderMgr::GetInst()->render();

	CTimeMgr::GetInst()->render();

	KeyUseInfoMgr::GetInst()->render();
}
