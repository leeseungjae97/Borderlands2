#include "pch.h"
#include "CKeyMgr.h"

#include "CDevice.h"
#include "CEngine.h"

#include "CCamera.h"
#include "CFontMgr.h"
#include "CLevel.h"
#include "CLevelMgr.h"

int g_arrVK[(UINT)KEY::END]
=
{
	 VK_UP,
	 VK_DOWN,
	 VK_LEFT,
	 VK_RIGHT,

	 VK_SPACE,
	 VK_RETURN,
	 VK_ESCAPE,
	 VK_LMENU,
	 VK_LCONTROL,
	 VK_LSHIFT,
	 VK_TAB,

	 VK_LBUTTON,
	 VK_RBUTTON,

	'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P',
	'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L',
	'Z', 'X', 'C', 'V', 'B', 'N', 'M',

	 '0',
	 '1',
	 '2',
	 '3',
	 '4',
	 '5',
	 '6',
	 '7',
	 '8',
	 '9',

};





CKeyMgr::CKeyMgr()
{

}

CKeyMgr::~CKeyMgr()
{

}


void CKeyMgr::init()
{
	for (int i = 0; i < (int)KEY::END; ++i)
	{
		m_vecKey.push_back(tKeyInfo{ (KEY)i  , KEY_STATE::NONE });
	}
}

void CKeyMgr::tick()
{
	if (GetFocus())
	{
		for (size_t i = 0; i < m_vecKey.size(); ++i)
		{
			if (GetAsyncKeyState(g_arrVK[(UINT)m_vecKey[i].key]) & 0x8000)
			{
				// 이전에는 눌리지 않았다.
				if (false == m_vecKey[i].bPrev)
				{
					m_vecKey[i].state = KEY_STATE::TAP;
					m_vecKey[i].toggle_state = KEY_STATE::TOGGLE;
					m_vecKey[i].bPrev = true;
				}
				else
				{
					// 지금도 눌려있고, 이전 프레임에서도 눌려있었다.
					m_vecKey[i].state = KEY_STATE::PRESSED;
				}
			}
			else
			{
				// 눌려있지 않다.
				if (false == m_vecKey[i].bPrev)
				{
					m_vecKey[i].state = KEY_STATE::NONE;
				}
				else
				{
					m_vecKey[i].state = KEY_STATE::RELEASE;
					m_vecKey[i].bPrev = false;
				}
			}
		}

		
		static bool bMouseShow = false;
		static bool bCur = false;
		if(CLevelMgr::GetInst()->GetCurLevel()->GetState() == LEVEL_STATE::PLAY)
		{
			if (KEY_TAP(KEY::M))
			{
				bMouseShow = !bMouseShow;
				bCur = true;
			}
			if(bCur)
			{
				SetCursor(bMouseShow == false ? NULL : CEngine::GetInst()->GetDefaultCursor());
				bCur = false;
			}
			ShowCursor(bMouseShow);
			if(!bMouseShow)
			{
				Vec2 m_vResolution = CEngine::GetInst()->GetWindowResolution();
				HWND m_hWnd = CEngine::GetInst()->GetMainWnd();

				POINT pt;
				GetCursorPos(&pt);
				Vec2 vCurMousePos = Vec2((float)pt.x, (float)pt.y);
				Vec2 vPrevMousePos = Vec2(m_vMousePos.x, m_vMousePos.y);
				Vec2 vSmoothMousePos = Vec2::SmoothStep(vCurMousePos, vPrevMousePos, 0.5f);

				m_vMouseDir.x = pt.x - vSmoothMousePos.x;
				m_vMouseDir.y = pt.y - vSmoothMousePos.y;

				m_vMouseDir.y *= -1;

				RECT rt = { 0, 0, (int)m_vResolution.x, (int)m_vResolution.y };
				//ClipCursor(&rt);
				GetClientRect(m_hWnd, &rt);

				pt.x = (rt.right - rt.left) / 2;
				pt.y = (rt.bottom - rt.top) / 2;

				ClientToScreen(m_hWnd, &pt);
				SetCursorPos(pt.x, pt.y);

				m_vMousePos.x = pt.x;
				m_vMousePos.y = pt.y;
			}else
			{
				//ClipCursor(NULL);
				m_vPrevMousePos = m_vMousePos;

				POINT ptMousePos = {};
				GetCursorPos(&ptMousePos);
				ScreenToClient(CEngine::GetInst()->GetMainWnd(), &ptMousePos);
				m_vMousePos = Vec2((float)ptMousePos.x, (float)ptMousePos.y);

				m_vMouseDir = m_vMousePos - m_vPrevMousePos;
				m_vMouseDir.y *= -1;
			}
	

		}else
		{
			// Mouse 위치 갱신
			m_vPrevMousePos = m_vMousePos;

			POINT ptMousePos = {};
			GetCursorPos(&ptMousePos);
			ScreenToClient(CEngine::GetInst()->GetMainWnd(), &ptMousePos);
			m_vMousePos = Vec2((float)ptMousePos.x, (float)ptMousePos.y);

			m_vMouseDir = m_vMousePos - m_vPrevMousePos;
			m_vMouseDir.y *= -1;
		}
	}

	// Window 가 focus 상태가 아니다
	else
	{
		for (size_t i = 0; i < m_vecKey.size(); ++i)
		{
			if (KEY_STATE::TAP == m_vecKey[i].state || KEY_STATE::PRESSED == m_vecKey[i].state)
			{
				m_vecKey[i].state = KEY_STATE::RELEASE;
			}

			else if (KEY_STATE::RELEASE == m_vecKey[i].state)
			{
				m_vecKey[i].state = KEY_STATE::NONE;
			}			 
		}
	}
}

Vec3 CKeyMgr::GetUnprojMousePos(CCamera* _Camera, float _CurDepth)
{
	if (nullptr == _Camera) return Vec3(0.f, 0.f, 0.f);

	D3D11_VIEWPORT _Viewport = CDevice().GetInst()->GetD3D11_ViewPort();
	Vec3 _vMousePosVec3 = Vec3(m_vMousePos.x, m_vMousePos.y, _CurDepth);

	Viewport viewport = {0.f, 0.f, _Viewport.Width, _Viewport.Height, _Camera->GetFarZ(), _Camera->GetNearZ() };

	const Matrix identity = { 1.f, 0.f, 0.f, 0.f,
								  0.f, 1.f, 0.f, 0.f,
								  0.f, 0.f, 1.f, 0.f,
								  0.f, 0.f, 0.f, 1.f };

	return viewport.Unproject(_vMousePosVec3, _Camera->GetProjMat(), _Camera->GetViewMat(), identity);
}
