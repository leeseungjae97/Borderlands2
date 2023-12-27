#include "pch.h"
#include "CKeyMgr.h"

#include "CDevice.h"
#include "CEngine.h"

#include "CCamera.h"

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

	 'Q',
	 'W',
	 'E',
	 'R',

	 'T',
	 'Y',
	 'U',
	 'I',
	 'O',
	 'P',

	 'A',
	 'S',
	 'D',
	 'F',

	 'Z',
	 'X',
	 'C',
	 'V',
	  
	 	 

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
				// �������� ������ �ʾҴ�.
				if (false == m_vecKey[i].bPrev)
				{
					m_vecKey[i].state = KEY_STATE::TAP;
					m_vecKey[i].bPrev = true;
				}
				else
				{
					// ���ݵ� �����ְ�, ���� �����ӿ����� �����־���.
					m_vecKey[i].state = KEY_STATE::PRESSED;
				}
			}
			else
			{
				// �������� �ʴ�.
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

		// Mouse ��ġ ����
		m_vPrevMousePos = m_vMousePos;

		POINT ptMousePos = {};
		GetCursorPos(&ptMousePos);		
		ScreenToClient(CEngine::GetInst()->GetMainWnd(), &ptMousePos);
		m_vMousePos = Vec2((float)ptMousePos.x, (float)ptMousePos.y);

		m_vMouseDir = m_vMousePos - m_vPrevMousePos;
		m_vMouseDir.y *= -1;
	}

	// Window �� focus ���°� �ƴϴ�
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
