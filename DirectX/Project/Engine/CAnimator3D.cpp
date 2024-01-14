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
	//, m_iCurClip(0)
	//, m_dCurTime(0.0)
	, m_iFrameCount(30)
	, m_pBoneFinalMatBuffer(nullptr)
	, m_bFinalMatUpdate(false)
	//, m_iFrameIdx(0)
	//, m_iNextFrameIdx(0)
	//, m_fRatio(0.f)
	, m_iClipIdx(0)
	, m_bLoop(false)
	, m_bMultipleClip(false)
	, m_pCurClip(nullptr)
	, mClips{}
	, mEvents{}
	, CComponent(COMPONENT_TYPE::ANIMATOR3D)
{
	m_pBoneFinalMatBuffer = new CStructuredBuffer;
}

CAnimator3D::CAnimator3D(const CAnimator3D& _origin)
	: m_pVecBones(_origin.m_pVecBones)
	, m_pVecClip(_origin.m_pVecClip)
	//, m_iCurClip(_origin.m_iCurClip)
	//, m_dCurTime(_origin.m_dCurTime)
	, m_iFrameCount(_origin.m_iFrameCount)
	, m_pBoneFinalMatBuffer(nullptr)
	, m_bFinalMatUpdate(false)
	//, m_iFrameIdx(_origin.m_iFrameIdx)
	//, m_iNextFrameIdx(_origin.m_iNextFrameIdx)
	//, m_fRatio(_origin.m_fRatio)
	, m_bLoop(_origin.m_bLoop)
	, m_bMultipleClip(_origin.m_bMultipleClip)
	, m_pCurClip(_origin.m_pCurClip)
	, m_iClipIdx(_origin.m_iClipIdx)
	, CComponent(COMPONENT_TYPE::ANIMATOR3D)
{
	Copy_Map(_origin.mClips, mClips);
	Copy_Map(_origin.mEvents, mEvents);

	m_pBoneFinalMatBuffer = new CStructuredBuffer;
}

CAnimator3D::~CAnimator3D()
{
	if (nullptr != m_pBoneFinalMatBuffer)
		delete m_pBoneFinalMatBuffer;

	Safe_Del_Map(mClips);
	Safe_Del_Map(mEvents);
}

void CAnimator3D::finaltick()
{
	Events* events = nullptr;
	if (m_pCurClip)
	{
		m_pCurClip->finlatick();
	}
	
	if (m_bLoop)
	{
		events = FindEvents(m_pCurClip->GetName());
		if (events)
			events->startEvent();
	}
	else
	{
		events = FindEvents(m_pCurClip->GetName());
		if (events)
			events->endEvent();

	}
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

		pUpdateShader->SetFrameDataBuffer(pMesh->GetBoneFrameDataBuffer(m_iClipIdx));
		pUpdateShader->SetOffsetMatBuffer(pMesh->GetBoneOffsetBuffer());
		pUpdateShader->SetOutputBuffer(m_pBoneFinalMatBuffer);

		UINT iBoneCount = (UINT)m_pVecBones->size();
		pUpdateShader->SetBoneCount(iBoneCount);
		pUpdateShader->SetFrameIndex(m_pCurClip->GetClipFrame());
		pUpdateShader->SetNextFrameIdx(m_pCurClip->GetClipNextFrame());
		pUpdateShader->SetFrameRatio(m_pCurClip->GetRatio());

		// 업데이트 쉐이더 실행
		pUpdateShader->Execute();

		m_bFinalMatUpdate = true;
	}

	// t30 레지스터에 최종행렬 데이터(구조버퍼) 바인딩		
	m_pBoneFinalMatBuffer->UpdateData(30, PIPELINE_STAGE::PS_VERTEX);
}

void CAnimator3D::SetBones(const vector<tMTBone>* _vecBones)
{
	m_pVecBones = _vecBones;
	m_vecFinalBoneMat.resize(m_pVecBones->size());
}

void CAnimator3D::SetAnimClip(const vector<tMTAnimClip>* _vecAnimClip)
{
	m_pVecClip = _vecAnimClip;
	m_vecClipUpdateTime.resize(m_pVecClip->size());

	create_clip();
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
		events = FindEvents(m_pCurClip->GetName() + L"endEvent");
		if (events)
			events->endEvent();
	}

	if (nullptr != pCheckClip)
	{
		m_pCurClip = pCheckClip;
		m_pCurClip->Reset();
		events = FindEvents(m_pCurClip->GetName() + L"startEvent");
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
	fwrite(&m_pVecBones, sizeof(tMTBone), m_pVecBones->size(), _pFile);

	fwrite(&m_pVecBones, sizeof(tMTBone), m_pVecBones->size(), _pFile);
	fwrite(&m_pVecClip, sizeof(tMTAnimClip), m_pVecClip->size(), _pFile);

	fwrite(&m_vecClipUpdateTime, sizeof(float), m_vecClipUpdateTime.size(), _pFile);

	fwrite(&m_vecFinalBoneMat, sizeof(Matrix), m_vecFinalBoneMat.size(), _pFile);
	fwrite(&m_iFrameCount, sizeof(int), 1, _pFile);
	//fwrite(&m_dCurTime, sizeof(double), 1, _pFile);
	//fwrite(&m_iCurClip, sizeof(int), 1, _pFile);
	//fwrite(&m_iFrameIdx, sizeof(int), 1, _pFile);
	//fwrite(&m_iNextFrameIdx, sizeof(int), 1, _pFile);
	//fwrite(&m_fRatio, sizeof(float), 1, _pFile);
	fwrite(&m_bFinalMatUpdate, sizeof(bool), 1, _pFile);
	fwrite(&m_bLoop, sizeof(bool), 1, _pFile);
	fwrite(&m_bMultipleClip, sizeof(bool), 1, _pFile);
	fwrite(&m_pCurClip, sizeof(CAnimClip), 1, _pFile);

	size_t count = mClips.size();

	fwrite(&count, sizeof(size_t), 1, _pFile);
	for(const auto& pair : mClips)
	{
		SaveWString(pair.first, _pFile);
		pair.second->SaveToLevelFile(_pFile);
		mEvents[pair.first]->SaveToLevelFile(_pFile);
	}
}

void CAnimator3D::LoadFromLevelFile(FILE* _pFile)
{
	fread(&m_pVecBones, sizeof(tMTBone), m_pVecBones->size(), _pFile);

	fread(&m_pVecBones, sizeof(tMTBone), m_pVecBones->size(), _pFile);
	fread(&m_pVecClip, sizeof(tMTAnimClip), m_pVecClip->size(), _pFile);

	fread(&m_vecClipUpdateTime, sizeof(float), m_vecClipUpdateTime.size(), _pFile);

	fread(&m_vecFinalBoneMat, sizeof(Matrix), m_vecFinalBoneMat.size(), _pFile);
	fread(&m_iFrameCount, sizeof(int), 1, _pFile);
	//fread(&m_dCurTime, sizeof(double), 1, _pFile);
	//fread(&m_iCurClip, sizeof(int), 1, _pFile);
	//fread(&m_iFrameIdx, sizeof(int), 1, _pFile);
	//fread(&m_iNextFrameIdx, sizeof(int), 1, _pFile);
	//fread(&m_fRatio, sizeof(float), 1, _pFile);
	fread(&m_bFinalMatUpdate, sizeof(bool), 1, _pFile);
	fread(&m_bLoop, sizeof(bool), 1, _pFile);
	fread(&m_bMultipleClip, sizeof(bool), 1, _pFile);
	fread(&m_pCurClip, sizeof(CAnimClip), 1, _pFile);

	size_t count = 0;
	fread(&count, sizeof(size_t), 1, _pFile);
	for (size_t i = 0 ; i < count; ++i)
	{
		wstring strCurAnimName;
		LoadWString(strCurAnimName, _pFile);
		CAnimClip* clip = new CAnimClip();
		clip->LoadFromLevelFile(_pFile);

		mClips.insert(make_pair(strCurAnimName, clip));

		Events* events = new Events();
		events->LoadFromLevelFile(_pFile);
		mEvents.insert(make_pair(strCurAnimName, events));
	}
}

void CAnimator3D::check_mesh(Ptr<CMesh> _pMesh)
{
	UINT iBoneCount = _pMesh->GetBoneCount();
	if (m_pBoneFinalMatBuffer->GetElementCount() != iBoneCount)
	{
		m_pBoneFinalMatBuffer->Create(sizeof(Matrix), iBoneCount, SB_TYPE::READ_WRITE, false, nullptr);
	}
}

void CAnimator3D::create_clip()
{
	if (mClips.size() != 0 || mEvents.size() != 0) return;

	//if (m_pVecClip->size() > 1)
	//{
	//	tMTAnimClip clip  = m_pVecClip->at(0);
	//	CAnimClip* pAnimClip = new CAnimClip();
	//	mClips.insert(std::make_pair(clip.strAnimName, pAnimClip));

	//	pAnimClip->Create(L"test1", clip, 0, 20, true);

	//	m_pCurClip = FindClip(L"test1");
	//}
	//else
	//{
	//	
	//}
	//m_bLoop = true;

	wstring testClip = L"";
	for (size_t i = 0; i < m_pVecClip->size(); ++i)
	{
		tMTAnimClip clip = m_pVecClip->at(i);

		CAnimClip* pAnimClip = new CAnimClip();
		if(L"" == testClip)
			testClip = clip.strAnimName;

		pAnimClip->Create(clip.strAnimName, clip
			, clip.iStartFrame, clip.iEndFrame, true);
		//pAnimClip->Create(clip.strAnimName, m_pVecClip, true);

		mClips.insert(std::make_pair(clip.strAnimName, pAnimClip));
		Events* events = FindEvents(m_pVecClip->at(i).strAnimName);

		mEvents.insert(make_pair(clip.strAnimName, events));
	}

	m_pCurClip = mClips.at(testClip);
}
