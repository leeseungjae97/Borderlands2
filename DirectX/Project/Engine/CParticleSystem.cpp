#include "pch.h"
#include "CParticleSystem.h"

#include "CCamera.h"
#include "CDevice.h"
#include "CFontMgr.h"
#include "CKeyMgr.h"
#include "CLevel.h"
#include "CLevelMgr.h"
#include "CRenderMgr.h"
#include "CStructuredBuffer.h"

#include "CResMgr.h"
#include "CTransform.h"
#include "CTimeMgr.h"

CParticleSystem::CParticleSystem()
	: CRenderComponent(COMPONENT_TYPE::PARTICLESYSTEM)
	, m_ParticleBuffer(nullptr)
	, m_RWBuffer(nullptr)
	, m_ModuleData{}
	, m_AccTime(0.f)
{
	//m_ModuleData.iMaxParticleCount = 5;
	
	//m_ModuleData.ModuleCheck[(UINT)PARTICLE_MODULE::PARTICLE_SPAWN] = true;
	//m_ModuleData.SpawnRate = 50;
	//m_ModuleData.vSpawnColor = Vec3(0.4f, 1.f, 0.4f);
	//m_ModuleData.vSpawnScaleMin = Vec3(100.f, 10.f, 1.f);
	//m_ModuleData.vSpawnScaleMax = Vec3(50.f, 5.f, 1.f);

	//m_ModuleData.vBoxShapeScale = Vec3(500.f, 500.f, 200.f);	
	//m_ModuleData.Space = 0; // 시뮬레이션 좌표계

	//m_ModuleData.MinLifeTime = 3.f;
	//m_ModuleData.MaxLifeTime = 5.f;

	//m_ModuleData.ModuleCheck[(UINT)PARTICLE_MODULE::SCALE_CHANGE] = true;
	//m_ModuleData.StartScale = 1.5f;
	//m_ModuleData.EndScale = 0.2f;

	//m_ModuleData.ModuleCheck[(UINT)PARTICLE_MODULE::COLOR_CHANGE] = true;
	//m_ModuleData.vStartColor = Vec3(0.2f, 0.3f, 1.0f);
	//m_ModuleData.vEndColor = Vec3(0.4f, 1.f, 0.4f);

	//m_ModuleData.ModuleCheck[(UINT)PARTICLE_MODULE::ADD_VELOCITY] = false;
	//m_ModuleData.AddVelocityType = 2;
	//m_ModuleData.Speed = 300.f;
	//m_ModuleData.vVelocityDir = Vec3(0.f, 1.f, 0.f);
	//m_ModuleData.OffsetAngle;

	//m_ModuleData.ModuleCheck[(UINT)PARTICLE_MODULE::DRAG] = true;
	//m_ModuleData.StartDrag = 500.f;
	//m_ModuleData.EndDrag = -500.f;

	//m_ModuleData.ModuleCheck[(UINT)PARTICLE_MODULE::NOISE_FORCE] = false;
	//m_ModuleData.fNoiseTerm = 1.f;
	//m_ModuleData.fNoiseForce = 100.f;

	//m_ModuleData.ModuleCheck[(UINT)PARTICLE_MODULE::RENDER] = true;
	//m_ModuleData.VelocityAlignment = true;
	//m_ModuleData.VelocityScale = true;
	//m_ModuleData.vMaxVelocityScale = Vec3(15.f, 1.f, 1.f);
	//m_ModuleData.vMaxSpeed = 500.f;



	// 입자 메쉬
	SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"PointMesh"));

	// 파티클 스폰 개수 전달용 버퍼
	m_RWBuffer = new CStructuredBuffer;
	m_RWBuffer->Create(sizeof(tRWParticleBuffer), 1, SB_TYPE::READ_WRITE, true, "particleRW Buffer");

	// 모듈 활성화 및 모듈 설정 정보 버퍼
	m_ModuleDataBuffer = new CStructuredBuffer;
	m_ModuleDataBuffer->Create(sizeof(tParticleModule), 1, SB_TYPE::READ_ONLY, true, "particle Module Buffer");
}

CParticleSystem::~CParticleSystem()
{
	if (nullptr != m_ParticleBuffer)
		delete m_ParticleBuffer;

	if (nullptr != m_RWBuffer)
		delete m_RWBuffer;

	if (nullptr != m_ModuleDataBuffer)
		delete m_ModuleDataBuffer;
}


void CParticleSystem::SetMaxSpawn(int _MaxSpawn)
{
	m_ModuleData.iMaxParticleCount = _MaxSpawn;
	
	m_ParticleBuffer = new CStructuredBuffer;
	m_ParticleBuffer->Create(sizeof(tParticle), m_ModuleData.iMaxParticleCount, SB_TYPE::READ_WRITE, true, "particle Module Buffer");
}

void CParticleSystem::SetParticleTex(const Ptr<CTexture>& _Tex)
{
	GetMaterial(0)->SetTexParam(TEX_0, _Tex);
}


void CParticleSystem::SetNoiseTex(const Ptr<CTexture>& _NoiseTex)
{
	m_UpdateCS->SetNoiseTexture(_NoiseTex);
}

void CParticleSystem::SetComputeShader(const Ptr<CComputeShader>& _ComputeShader)
{
	m_UpdateCS = (CParticleUpdateShader*)_ComputeShader.Get();
}

void CParticleSystem::finaltick()
{
	// 스폰 레이트 계산
	// 1개 스폰 시간
	//float fTimePerCount = 1.f / (float)m_ModuleData.SpawnRate;
	//m_AccTime += DT;

	//// 누적시간이 개당 생성시간을 넘어서면
	//if (fTimePerCount < m_AccTime)
	//{
	//	// 초과 배율 ==> 생성 개수
	//	float fData = m_AccTime / fTimePerCount;

	//	// 나머지는 남은 시간
	//	m_AccTime = fTimePerCount * (fData - floor(fData));

	//	// 버퍼에 스폰 카운트 전달
	//	tRWParticleBuffer rwbuffer = { (int)fData, };		
	//	m_RWBuffer->SetData(&rwbuffer);
	//}


	// 파티클 업데이트 컴퓨트 쉐이더
	m_ModuleDataBuffer->SetData(&m_ModuleData);

	if(m_ParticleBuffer)
	    m_UpdateCS->SetParticleBuffer(m_ParticleBuffer);

	m_UpdateCS->SetRWParticleBuffer(m_RWBuffer);
	m_UpdateCS->SetModuleData(m_ModuleDataBuffer);

	CCamera* cam = CRenderMgr::GetInst()->GetMainCam();
	//if (CLevelMgr::GetInst()->GetCurLevel()->GetState() == LEVEL_STATE::STOP)
	//	cam = CRenderMgr::GetInst()->GetEditorCam();
	//else
	//	cam = CRenderMgr::GetInst()->GetMainCam();

	Vec3 _UnprojectPos = CKeyMgr::GetInst()->GetUnprojMousePos(cam, Transform()->GetWorldPos().z);
	m_UpdateCS->SetParticleObjectPos(Transform()->GetWorldPos());
	m_UpdateCS->SetParticleMousePos(Vec4(_UnprojectPos.x, _UnprojectPos.y, _UnprojectPos.z,1.f));

	m_UpdateCS->Execute();
}

void CParticleSystem::render()
{
	Transform()->UpdateData();

	// 파티클버퍼 t20 에 바인딩
	if(m_ParticleBuffer)
	    m_ParticleBuffer->UpdateData(20, PIPELINE_STAGE::PS_ALL);

	// 모듈 데이터 t21 에 바인딩
	m_ModuleDataBuffer->UpdateData(21, PIPELINE_STAGE::PS_GEOMETRY);
	CCamera* cam = CRenderMgr::GetInst()->GetMainCam();
	//if( CLevelMgr::GetInst()->GetCurLevel()->GetState() == LEVEL_STATE::STOP)
	//	cam = CRenderMgr::GetInst()->GetEditorCam();
	//else
	//	cam = ;
	
	Vec3 _UnprojectPos = CKeyMgr::GetInst()->GetUnprojMousePos(cam, Transform()->GetWorldPos().z);
	Vec2 _pp = CKeyMgr::GetInst()->GetMousePos();
	//Vec2 mm = Vec2(_UnprojectPos.x, _UnprojectPos.y);
	//GetMaterial()->SetScalarParam(VEC2_1, &mm);

	GetMaterial(0)->UpdateData();
	GetMesh()->render_particle(m_ModuleData.iMaxParticleCount);

	// 파티클 버퍼 바인딩 해제
	if (m_ParticleBuffer)
	    m_ParticleBuffer->Clear();

	m_ModuleDataBuffer->Clear();

	static wchar_t szBuff[256] = {};
	swprintf_s(szBuff, L"MOUSE X : %f, MOUSE Y : %f", _UnprojectPos.x, _UnprojectPos.y);
	CFontMgr::GetInst()->DrawFont(szBuff, 10.f, 36.f, 16.f, FONT_RGBA(255, 0, 0, 255));

	swprintf_s(szBuff, L"MOUSE_W X : %f, MOUSE_W Y : %f", _pp.x, _pp.y);
	CFontMgr::GetInst()->DrawFont(szBuff, 10.f, 52.f, 16.f, FONT_RGBA(255, 0, 0, 255));
}

void CParticleSystem::render(UINT _iSubset)
{
	render();
}

void CParticleSystem::SaveToLevelFile(FILE* _File)
{
	CRenderComponent::SaveToLevelFile(_File);

	if (m_ParticleBuffer)

		m_ParticleBuffer->Clear();

	fwrite(&m_ModuleData, sizeof(tParticleModule), 1, _File);

	SaveResRef(m_UpdateCS.Get(), _File);
}

void CParticleSystem::LoadFromLevelFile(FILE* _File)
{
	CRenderComponent::LoadFromLevelFile(_File);

	fread(&m_ModuleData, sizeof(tParticleModule), 1, _File);
	
	if(nullptr == m_ParticleBuffer)
	{
		m_ParticleBuffer = new CStructuredBuffer;
		m_ParticleBuffer->Create(sizeof(tParticle), m_ModuleData.iMaxParticleCount, SB_TYPE::READ_WRITE, false, "Particle Module Buffer");
	}

	int i = 0;
	fread(&i, sizeof(i), 1, _File);

	if (i)
	{
		wstring strKey, strRelativePath;
		LoadWString(strKey, _File);
		LoadWString(strRelativePath, _File);

		m_UpdateCS = (CParticleUpdateShader*)CResMgr::GetInst()->FindRes<CComputeShader>(strKey).Get();
	}
}
