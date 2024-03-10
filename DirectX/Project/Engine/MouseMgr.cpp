#include "pch.h"
#include "MouseMgr.h"

#include "CCamera.h"
#include "CRenderMgr.h"
#include "CEngine.h"
#include "CKeyMgr.h"
#include "CTransform.h"

MouseMgr::MouseMgr()
{
}

MouseMgr::~MouseMgr()
{
}

void MouseMgr::MouseOut()
{
}

Vec2 MouseMgr::UnprojectionMouse2DPos(float z)
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
		vResol.x,
		vResol.y,
		10.f,
		-1.f,
	};
	Vec2 vMousePos = CKeyMgr::GetInst()->GetMousePos();
	Vec3 mousePos3 = Vector3(vMousePos.x, vMousePos.y, z);

	Vec3 unprojMousePos = viewport.Unproject(mousePos3, matPro, matView, Matrix::Identity);

	return Vec2(unprojMousePos.x, unprojMousePos.y);
}

Vec3 MouseMgr::MousePosRay()
{
	CCamera* _Cam = CRenderMgr::GetInst()->GetMainCam();

	Vec3 vStart = _Cam->GetRay().vStart;
	Vec3 vDir = _Cam->GetRay().vDir;

	Vec3 vRay = vStart + (vDir * 20e6f);

	return vRay;
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
