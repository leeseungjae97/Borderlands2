#include "pch.h"
#include "MouseMgr.h"

#include "CCamera.h"
#include "CRenderMgr.h"
#include "CEngine.h"
#include "CKeyMgr.h"

MouseMgr::MouseMgr()
{
}

MouseMgr::~MouseMgr()
{
}

void MouseMgr::MouseOut()
{
}

Vec2 MouseMgr::UnprojectionMousePos(float z)
{
	CCamera* _Cam = CRenderMgr::GetInst()->GetUICam();

	if (nullptr == _Cam)
		return Vec2::Zero;

	Matrix matPro = _Cam->GetProjMat();
	Matrix matView = _Cam->GetViewMat();
	Vec2 vResol = CEngine::GetInst()->GetWindowResolution();
	
	Viewport viewport = {
		0.f,
		0.f,
		vResol.x / 2.f,
		vResol.y / 2.f,
		10.f,
		-1.f,
	};
	Vec2 vMousePos = CKeyMgr::GetInst()->GetMousePos();
	Vec3 mousePos3 = Vector3(vMousePos.x, vMousePos.y, z);

	Vec3 unprojMousePos = viewport.Unproject(mousePos3, matPro, matView, Matrix::Identity);

	return Vec2(unprojMousePos.x, unprojMousePos.y);
}

void MouseMgr::init()
{
}

void MouseMgr::tick()
{
}

void MouseMgr::finaltick()
{
}

void MouseMgr::UpdateMouseHoverUI()
{
}
