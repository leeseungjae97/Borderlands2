#include "pch.h"
#include "CAnimator3D.h"

#include "CAnimation3D_CShader.h"
#include "CComputeShader.h"
#include "CMaterial.h"
#include "ptr.h"
#include "CStructuredBuffer.h"
#include "CMesh.h"
#include "CMeshRender.h"
#include "CResMgr.h"
#include "CTimeMgr.h"

CAnimator3D::CAnimator3D()
	: m_pVecBones(nullptr)
	, m_pVecClip(nullptr)
	, m_iCurClip(0)
	, m_dCurTime(0.)
	, m_iFrameCount(30)
	, m_pBoneFinalMatBuffer(nullptr)
	, m_bFinalMatUpdate(false)
	, m_iFrameIdx(0)
	, m_iNextFrameIdx(0)
	, m_fRatio(0.f)
	, m_bLoop(false)
	, mClips{}
	, mEvents{}
	, CComponent(COMPONENT_TYPE::ANIMATOR3D)
{
	m_pBoneFinalMatBuffer = new CStructuredBuffer;
}

CAnimator3D::CAnimator3D(const CAnimator3D& _origin)
	: m_pVecBones(_origin.m_pVecBones)
	, m_pVecClip(_origin.m_pVecClip)
	, m_iCurClip(_origin.m_iCurClip)
	, m_dCurTime(_origin.m_dCurTime)
	, m_iFrameCount(_origin.m_iFrameCount)
	, m_pBoneFinalMatBuffer(nullptr)
	, m_bFinalMatUpdate(false)
	, m_iFrameIdx(_origin.m_iFrameIdx)
	, m_iNextFrameIdx(_origin.m_iNextFrameIdx)
	, m_fRatio(_origin.m_fRatio)
	, CComponent(COMPONENT_TYPE::ANIMATOR3D)
{
	m_pBoneFinalMatBuffer = new CStructuredBuffer;
}

CAnimator3D::~CAnimator3D()
{
	if (nullptr != m_pBoneFinalMatBuffer)
		delete m_pBoneFinalMatBuffer;
}

void CAnimator3D::finaltick()
{
	m_dCurTime = 0.f;

	// 현재 재생중인 Clip 의 시간을 진행한다.
	//TODO 
	// m_vecClipUpdateTime[m_iCurClip] += DT;

	//if (m_vecClipUpdateTime[m_iCurClip] >= m_pVecClip->at(m_iCurClip).dTimeLength)
	//{
	//	m_vecClipUpdateTime[m_iCurClip] = 0.f;
	//}

	m_pCurClip->finlatick();

	//if(m_pCurClip->GetClipTime())

	m_dCurTime = m_pVecClip->at(m_iCurClip).dStartTime + m_vecClipUpdateTime[m_iCurClip];

	// 현재 프레임 인덱스 구하기 
	double dFrameIdx = m_dCurTime * (double)m_iFrameCount;
	m_iFrameIdx = (int)(dFrameIdx);

	// 다음 프레임 인덱스
	Events* events = FindEvents(m_pCurClip->GetName());

	if (m_bLoop)
	{
		m_iNextFrameIdx = m_pCurClip->GetClipStartIdx();
		if (events)
			events->startEvent();
	}
	else
	{
		m_iNextFrameIdx = m_iFrameIdx;	// 끝이면 현재 인덱스를 유지
		if (events)
			events->endEvent();

	}
	
	//if (m_iFrameIdx >= m_pCurClip->GetClipEndFrame())
	//{
	//	m_iNextFrameIdx = m_iFrameIdx;
	//}
	//else
	//	m_iNextFrameIdx = m_iFrameIdx + 1;

	// 프레임간의 시간에 따른 비율을 구해준다.
	// m_fRatio = (float)(dFrameIdx - (double)m_iFrameIdx);

	// 컴퓨트 쉐이더 연산여부
	m_bFinalMatUpdate = false;
}

void CAnimator3D::UpdateData()
{
	if (nullptr == m_pCurClip) 
		return;

	if (!m_bFinalMatUpdate)
	{

		// Animation3D Update Compute Shader
		CAnimation3D_CShader* pUpdateShader = (CAnimation3D_CShader*)CResMgr::GetInst()->FindRes<CComputeShader>(L"Animation3DUpdateCS").Get();

		// Bone Data
		Ptr<CMesh> pMesh = MeshRender()->GetMesh();
		check_mesh(pMesh);

		pUpdateShader->SetFrameDataBuffer(pMesh->GetBoneFrameDataBuffer());
		pUpdateShader->SetOffsetMatBuffer(pMesh->GetBoneOffsetBuffer());
		pUpdateShader->SetOutputBuffer(m_pBoneFinalMatBuffer);

		UINT iBoneCount = (UINT)m_pVecBones->size();
		pUpdateShader->SetBoneCount(iBoneCount);
		pUpdateShader->SetFrameIndex(m_pCurClip->GetClipFrame());
		pUpdateShader->SetNextFrameIdx(m_pCurClip->GetClipNextFrame());
		pUpdateShader->SetFrameRatio(m_fRatio);

		// 업데이트 쉐이더 실행
		pUpdateShader->Execute();

		m_bFinalMatUpdate = true;
	}

	// t30 레지스터에 최종행렬 데이터(구조버퍼) 바인딩		
	m_pBoneFinalMatBuffer->UpdateData(30, PIPELINE_STAGE::PS_VERTEX);
}

void CAnimator3D::SetAnimClip(const vector<tMTAnimClip>* _vecAnimClip)
{
	m_pVecClip = _vecAnimClip;
	m_vecClipUpdateTime.resize(m_pVecClip->size());

	// 테스트 코드
	/*static float fTime = 0.f;
	fTime += 1.f;
	m_vecClipUpdateTime[0] = fTime;*/
}

void CAnimator3D::ClearData()
{
	m_pBoneFinalMatBuffer->Clear();

	UINT iMtrlCount = MeshRender()->GetMtrlCount();
	Ptr<CMaterial> pMtrl = nullptr;
	for (UINT i = 0; i < iMtrlCount; ++i)
	{
		pMtrl = MeshRender()->GetSharedMaterial(i);
		if (nullptr == pMtrl)
			continue;

		pMtrl->SetAnim3D(false); // Animation Mesh 알리기
		pMtrl->SetBoneCount(0);
	}
}

void CAnimator3D::Play(const std::wstring& _Name, bool _Loop)
{
	CAnimClip* pCheckClip = FindClip(_Name);
	Events* events;

	if (m_pCurClip)
	{
		m_pCurClip->Reset();
		events = FindEvents(m_pCurClip->GetName());
		if (events)
			events->endEvent();
	}

	if (nullptr != pCheckClip)
	{
		m_pCurClip = pCheckClip;
		m_pCurClip->Reset();
		events = FindEvents(m_pCurClip->GetName());
		if (events)
			events->startEvent();
	}

	m_bLoop = _Loop;
}

std::shared_ptr<std::function<void()>>& CAnimator3D::StartEvent(const std::wstring key)
{
	Events* events = FindEvents(key + L"startEvent");
	return events->startEvent.mEvent;
}

std::shared_ptr<std::function<void()>>& CAnimator3D::CompleteEvent(const std::wstring key)
{
	Events* events = FindEvents(key + L"completeEvent");
	return events->completeEvent.mEvent;
}

std::shared_ptr<std::function<void()>>& CAnimator3D::EndEvent(const std::wstring key)
{
	Events* events = FindEvents(key + L"endEvent");
	return events->endEvent.mEvent;
}

std::shared_ptr<std::function<void()>>& CAnimator3D::ProgressEvent(const std::wstring key)
{
	Events* events = FindEvents(key + L"progressEvent");
	return events->progressEvent.mEvent;
}

CAnimator3D::Events* CAnimator3D::FindEvents(const std::wstring& name)
{
	std::map<std::wstring, Events*>::iterator iter
		= mEvents.find(name);

	if (iter == mEvents.end())
		return nullptr;

	return iter->second;
}

CAnimClip* CAnimator3D::FindClip(const std::wstring& name)
{
	std::map<std::wstring, CAnimClip*>::iterator iter
		= mClips.find(name);

	if (iter == mClips.end())
		return nullptr;

	return iter->second;
}


void CAnimator3D::SaveToLevelFile(FILE* _pFile)
{
}

void CAnimator3D::LoadFromLevelFile(FILE* _pFile)
{
}

void CAnimator3D::check_mesh(Ptr<CMesh> _pMesh)
{
	UINT iBoneCount = _pMesh->GetBoneCount();
	if (m_pBoneFinalMatBuffer->GetElementCount() != iBoneCount)
	{
		m_pBoneFinalMatBuffer->Create(sizeof(Matrix), iBoneCount, SB_TYPE::READ_WRITE, false, nullptr);
	}
}