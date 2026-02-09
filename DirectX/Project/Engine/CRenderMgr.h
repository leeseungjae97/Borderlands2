#pragma once
#include "CSingleton.h"

#include "ptr.h"
#include "CTexture.h"

class CCamera;
class CLight2D;
class CLight3D;
class CStructuredBuffer;
class MRT;

class CRenderMgr :
	public CSingleton<CRenderMgr>
{
	SINGLE(CRenderMgr);
private:
	vector<CCamera*>            m_vecCam;           // ���� ���� ���� �����ϴ� ī�޶� ��� �޾Ƶ�
	//CCamera* m_pEditorCam;       // �ܺ� �������ʿ��� �����ϴ� ī�޶� ��� �޾Ƶ�

	vector<tDebugShapeInfo>     m_vecShapeInfo;

	vector<tLightInfo>          m_vecLight2DInfo;
	vector<CLight2D*>			m_vecLight2D;

	vector<tLightInfo>          m_vecLight3DInfo;
	vector<CLight3D*>			m_vecLight3D;

	CStructuredBuffer* m_Light2DBuffer;
	CStructuredBuffer* m_Light3DBuffer;

	void (CRenderMgr::* RENDER_FUNC)(void);

	Ptr<CTexture>               m_RTCopyTex;
	Ptr<CTexture>               m_RTCopyTex2;
	Ptr<CTexture>               m_RTCopyTex3;
	Ptr<CTexture>               m_RTCopyDownScaleTex;

	Ptr<CTexture>               m_pScopeRTTex;
	Ptr<CTexture>               m_pMapRTTex;
	Ptr<CTexture>               m_pMainRTTex;

	MRT*						m_MRT[(UINT)MRT_TYPE::END];


public:
	void init();
	void render();
	void tick();
	void finaltick();

public:
	int RegisterCamera(CCamera* _Cam, int _idx);
	//void RegisterEditorCamera(CCamera* _Cam) { m_pEditorCam = _Cam; }
	void SetRenderFunc(bool _IsPlay);
	UINT RegisterLight2D(CLight2D* _Light2D, const tLightInfo& _Info)
	{
	    m_vecLight2DInfo.push_back(_Info);
	    m_vecLight2D.push_back(_Light2D);
		return m_vecLight2D.size() - 1;

	}
	UINT RegisterLight3D(CLight3D* _Light3D, const tLightInfo& _Info)
	{
	    m_vecLight3DInfo.push_back(_Info);
	    m_vecLight3D.push_back(_Light3D);
		return m_vecLight3D.size() - 1;
	}

	void ClearCamera() { m_vecCam.clear(); }

	void AddDebugShapeInfo(const tDebugShapeInfo& _info) { m_vecShapeInfo.push_back(_info); }
	vector<tDebugShapeInfo>& GetDebugShapeInfo() { return m_vecShapeInfo; }

	const vector<CLight2D*>& GetLight2D() { return m_vecLight2D; }
	const vector<CLight3D*>& GetLight3D() { return m_vecLight3D; }

	//CCamera* GetEditorCam() { return m_pEditorCam; }
	CCamera* GetMainCam();
	CCamera* GetUICam();
	
	void CopyRenderTarget();

	MRT* GetMRT(MRT_TYPE _Type);


private:
	void UpdateData();
	void render_shadowmap();
	void render_play();
	void render_editor();
	void Clear();

	void CreateMRT();
	void ClearMRT();

};

