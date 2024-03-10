#include "pch.h"
#include "CAnimator3D.h"

#include "CAnimation3DShader.h"
#include "CComputeShader.h"
#include "CMaterial.h"
#include "ptr.h"
#include "CStructuredBuffer.h"
#include "CMesh.h"
#include "CMeshRender.h"
#include "CResMgr.h"
#include "CTimeMgr.h"

CAnimator3D::CAnimator3D()
	: CComponent(COMPONENT_TYPE::ANIMATOR3D)
	, m_iFrameCount(30)
	, m_pBoneFinalMatBuffer(nullptr)
	, m_pBoneFinalMatPosBuffer(nullptr)
	, m_pBoneFinalMatRotBuffer(nullptr)
	, m_bFinalMatUpdate(false)
	, m_bLoop(false)
	, m_bMultipleClip(false)
	, m_pCurClip(nullptr)
	, m_bBlend(false)
	, m_fBlendTime(0.1f)
	, m_fBRatio(0.0f)
	, m_fBlendAcc(0.0f)
	, m_iManualIdx(0)
	, m_bMaRatio(false)
	, m_bStop(false)
	, m_bBlendMode(true)
	, m_fSpeedAdj(1.f)
	, m_iHeadIdx(0)
	, m_iCameraIdx(0)
	, m_iWeaponRHandIdx(0)
	, m_iWeaponMuzzleIdx(0)
	, m_iFireBreathIdx(0)
	, m_iTailWeaponIdx(0)
	, m_iKnuckleIdx(0)
	, m_iStomachIdx(0)
	, m_iMouseIdx(0)
	, m_iChestIdx(0)
	, m_bUpdate(false)
	, mClips{}
	, mEvents{}
	, m_pVecBones{}
{
	m_pBoneFinalMatBuffer = new CStructuredBuffer;
	m_pBoneFinalMatPosBuffer = new CStructuredBuffer;
	m_pBoneFinalMatRotBuffer = new CStructuredBuffer;
}

CAnimator3D::CAnimator3D(const CAnimator3D& _origin)
	: m_pVecBones(_origin.m_pVecBones)
	, m_pMapClip(_origin.m_pMapClip)
	, m_iFrameCount(_origin.m_iFrameCount)
	, m_pBoneFinalMatBuffer(nullptr)
	, m_pBoneFinalMatPosBuffer(nullptr)
	, m_pBoneFinalMatRotBuffer(nullptr)
	, m_bFinalMatUpdate(false)
	, m_bLoop(_origin.m_bLoop)
	, m_bMultipleClip(_origin.m_bMultipleClip)
	, m_pCurClip(_origin.m_pCurClip)
	, m_pNextClip(_origin.m_pNextClip)
	, m_bBlendMode(_origin.m_bBlendMode)
	, m_bBlend(_origin.m_bBlend)
	, m_fBlendTime(_origin.m_fBlendTime)
	, m_fSpeedAdj(1.f)
	, m_iHeadIdx(_origin.m_iHeadIdx)
	, m_iCameraIdx(_origin.m_iCameraIdx)
	, m_iWeaponRHandIdx(_origin.m_iWeaponRHandIdx)
	, m_iWeaponMuzzleIdx(_origin.m_iWeaponMuzzleIdx)
	, m_iFireBreathIdx(_origin.m_iFireBreathIdx)
	, m_iTailWeaponIdx(_origin.m_iTailWeaponIdx)
	, m_iKnuckleIdx(_origin.m_iKnuckleIdx)
	, m_iStomachIdx(_origin.m_iStomachIdx)
	, m_iMouseIdx(_origin.m_iMouseIdx)
	, m_iChestIdx(_origin.m_iChestIdx)
	, m_bUpdate(_origin.m_bUpdate)
	, CComponent(COMPONENT_TYPE::ANIMATOR3D)
{
	Copy_Map(_origin.mClips, mClips);
	Copy_Map(_origin.mEvents, mEvents);

	m_pBoneFinalMatBuffer = new CStructuredBuffer;
	m_pBoneFinalMatPosBuffer = new CStructuredBuffer;
	m_pBoneFinalMatRotBuffer = new CStructuredBuffer;
}

CAnimator3D::~CAnimator3D()
{
	if (nullptr != m_pBoneFinalMatBuffer)
		delete m_pBoneFinalMatBuffer;

	if (nullptr != m_pBoneFinalMatPosBuffer)
		delete m_pBoneFinalMatPosBuffer;

	if (nullptr != m_pBoneFinalMatRotBuffer)
		delete m_pBoneFinalMatRotBuffer;

	Safe_Del_Map(mEvents);
	Safe_Del_Map(mClips);

	for (tMTBone& bone : m_pVecBones)
	{
		for (auto& pair : bone.vecKeyFrame)
		{
			pair.second.clear();
		}

		bone.vecKeyFrame.clear();
	}
	m_pVecBones.clear();

	m_pMapClip.clear();

	m_vecClipUpdateTime.clear();
}

void CAnimator3D::finaltick()
{
	Events* events = nullptr;

	if (m_bBlend)
	{
		if (m_pNextClip)
		{
			m_pNextClip->finlatick();
			m_pNextClip->SetSpeedAdj(m_fSpeedAdj);

			CustomEvent(m_pCurClip);
			m_vHeadPos = m_pNextClip->GetCurClip().vecTransKeyFrame[m_iHeadIdx].vTranslate;
		}

		m_fBlendAcc += DT;
		m_fBRatio = m_fBlendAcc / m_fBlendTime;

		if (m_fBlendAcc >= m_fBlendTime)
		{
			m_bBlend = false;
			m_fBlendAcc = 0.0f;
			m_pCurClip = m_pNextClip;
			m_pNextClip = nullptr;
		}
	}
	else
	{
		if (m_pCurClip)
		{
			
			m_pCurClip->SetSpeedAdj(m_fSpeedAdj);
			m_pCurClip->finlatick();

			CustomEvent(m_pCurClip);
			
			m_vHeadPos = m_pCurClip->GetCurClip().vecTransKeyFrame[m_iHeadIdx].vTranslate;

			if (m_pCurClip->IsFinish() && m_bLoop)
			{
				m_pNextClip = m_pCurClip;
				m_pNextClip->Reset();

				SetBlend(true, 0.1f);

				events = FindEvents(m_pNextClip->GetName());
				if (events)
					events->endEvent();

				events = FindEvents(m_pNextClip->GetName());
				if (events)
					events->startEvent();
			}

			if(m_pCurClip->IsFinish() && !m_bLoop)
			{
				events = FindEvents(m_pCurClip->GetName());
				if (events)
					events->endEvent();
			}
		}
	}

	m_bFinalMatUpdate = false;
}

void CAnimator3D::UpdateData()
{
	if (!m_bFinalMatUpdate && m_pCurClip)
	{
		m_bUpdate = true;
		// Animation3D Update Compute Shader
		CAnimation3DShader* pUpdateShader = (CAnimation3DShader*)CResMgr::GetInst()->FindRes<CComputeShader>(L"Animation3DUpdateCS").Get();

		// Bone Data
		Ptr<CMesh> pMesh = MeshRender()->GetMesh();
		check_mesh(pMesh);

		pUpdateShader->SetFrameDataBuffer(pMesh->GetBoneFrameDataBuffer(m_pCurClip->GetAnimName()));
		pUpdateShader->SetOffsetMatBuffer(pMesh->GetBoneOffsetBuffer());
		pUpdateShader->SetIsTransOnly(false);
		pUpdateShader->SetIsRotateOnly(false);
		pUpdateShader->SetOutputBuffer(m_pBoneFinalMatBuffer);

		pUpdateShader->SetOutputPosBuffer(m_pBoneFinalMatPosBuffer);
		pUpdateShader->SetOutputRotBuffer(m_pBoneFinalMatRotBuffer);
		pUpdateShader->SetIsBlend(m_bBlend);

		if (m_bBlend && m_pNextClip)
		{
			pUpdateShader->SetBlendFrameDataBuffer(pMesh->GetBlendFrameDataBuffer(m_pNextClip->GetAnimName()));
			pUpdateShader->SetNextClipFrameIndex(m_pNextClip->GetClipIdx());
			pUpdateShader->SetBlendRatio(m_fBRatio);
		}

		UINT iBoneCount = (UINT)m_pVecBones.size();
		pUpdateShader->SetBoneCount(iBoneCount);
		if (m_bStop)
		{
			pUpdateShader->SetFrameIndex(m_iManualIdx);
			pUpdateShader->SetFrameRatio(m_bMaRatio ? m_iManualRatio : m_pCurClip->GetRatio());
			pUpdateShader->SetFrameNextIndex(m_iManualIdx + 1 > m_pCurClip->GetClipLength() ? 0 : m_iManualIdx + 1);
		}
		else
		{
			pUpdateShader->SetFrameIndex(m_pCurClip->GetClipIdx());
			pUpdateShader->SetFrameRatio(m_pCurClip->GetRatio());
			pUpdateShader->SetFrameNextIndex(m_pCurClip->GetClipNextIdx());
		}

		pUpdateShader->Execute();

		m_bFinalMatUpdate = true;
	}

	m_pBoneFinalMatBuffer->UpdateData(30, PIPELINE_STAGE::PS_VERTEX);
}

void CAnimator3D::SetBones(const vector<tMTBone>* _vecBones)
{
	m_pVecBones = *_vecBones;
}

void CAnimator3D::SetAnimClip(const map<wstring, tMTAnimClip>& _vecAnimClip)
{
	m_pMapClip = _vecAnimClip;
	m_vecClipUpdateTime.resize(m_pMapClip.size());

	create_clip();

	for (int i = 0; i < m_pVecBones.size(); ++i)
	{
		tMTBone bone = m_pVecBones[i];
		if (bone.strBoneName == L"Head" || bone.strBoneName == L"Eye")
		{
			m_iHeadIdx = i;
		}
		if (bone.strBoneName == L"Heart")
		{
			m_iChestIdx = i;
		}
		if (bone.strBoneName == L"InnerJaw")
		{
			m_iMouseIdx = i;
		}
		if (bone.strBoneName == L"Spine02")
		{
			m_iStomachIdx = i;
		}
		if (bone.strBoneName == L"Camera")
		{
			m_iCameraIdx = i;
		}
		if (bone.strBoneName == L"R_Weapon_Bone")
		{
			m_iWeaponRHandIdx = i;
		}
		if (bone.strBoneName == L"L_Weapon_Bone")
		{
			m_iWeaponLHandIdx = i;
		}
		if (bone.strBoneName == L"L_Hand")
		{
			m_iLHandIdx = i;
		}
		if (bone.strBoneName == L"R_Hand")
		{
			m_iRHandIdx = i;
		}
		if(bone.strBoneName == L"Muzzle")
		{
			m_iWeaponMuzzleIdx = i;
		}
		if (bone.strBoneName == L"sd")
		{
			m_iFireBreathIdx = i;
		}
		if (bone.strBoneName == L"TailWeapon")
		{
			m_iTailWeaponIdx = i;
		}
		if (bone.strBoneName == L"R_Knuckles")
		{
			m_iKnuckleIdx = i;
		}
		
	}
	m_vHeadPos = m_pCurClip->GetCurClip().vecTransKeyFrame[m_iHeadIdx].vTranslate;
	m_vMuzzlePos = m_pCurClip->GetCurClip().vecTransKeyFrame[m_iWeaponMuzzleIdx].vTranslate;
	
}

CAnimClip* CAnimator3D::GetAnimClip(const wstring& _AnimClipName)
{
	auto iter= mClips.find(_AnimClipName);
	if (iter != mClips.end())
		return iter->second;

	return nullptr;
}

void CAnimator3D::SetAnimClipIdx(int _idx)
{
	m_iManualIdx = _idx;
	m_pCurClip->SetManualIdx(m_iManualIdx);
	//m_pCurClip->Set
}

void CAnimator3D::StopPlay()
{
	m_bStop = true;
	if(m_pCurClip)
	{
		m_iManualIdx = m_pCurClip->GetClipIdx();
	}
	
}

void CAnimator3D::ClearData()
{
	m_pBoneFinalMatBuffer->Clear();
	//m_pBoneFinalMatPosBuffer->Clear();
	//m_pBoneFinalMatRotBuffer->Clear();

	UINT iMtrlCount = MeshRender()->GetMtrlCount();
	Ptr<CMaterial> pMtrl = nullptr;
	for (UINT i = 0; i < iMtrlCount; ++i)
	{
		pMtrl = MeshRender()->GetMaterial(i);
		if (nullptr == pMtrl)
			continue;

		pMtrl->SetAnim3D(false); // Animation Mesh 알리기
		pMtrl->SetBoneCount(0);
	}
}

void CAnimator3D::SetBlend(bool _bBlend, float _fBlendTime)
{
	m_bBlend = _bBlend;
	m_fBlendAcc = 0.0f;
	m_fBlendTime = _fBlendTime;
}

void CAnimator3D::SetAnimClipEventIdx(UINT _Type, int _iEnd, int _iStart, int _iProgress, int _iComplete)
{
	auto iter = m_mapPreDefineAnim.find(_Type);
	if (iter != m_mapPreDefineAnim.end())
	{
		CAnimClip* clip = GetAnimClip(iter->second);
		if(clip)
		{
			if(_iEnd >= 0)
				clip->SetEventEndIdx(_iEnd);
			if (_iStart >= 0)
				clip->SetEventStartIdx(_iStart);
			if (_iProgress >= 0)
				clip->SetEventProgressIdx(_iProgress);
			if (_iComplete >= 0)
				clip->SetEventCompleteIdx(_iComplete);
		}else
		{
			assert(NULL);
		}
	}else
	{
		assert(NULL);
	}
		
}

Matrix CAnimator3D::GetRotMat(int idx)
{
	Matrix mat = Matrix::Identity;
	m_pBoneFinalMatRotBuffer->GetMatrixData(&mat, idx);
	return mat;
}

Matrix CAnimator3D::GetPosMat(int idx)
{
	Matrix mat = Matrix::Identity;
	m_pBoneFinalMatPosBuffer->GetMatrixData(&mat, idx);
	return mat;
}


void CAnimator3D::SetDefineAnimation(wstring animName, UINT _Type)
{
	CAnimClip* clip = FindClip(animName);

	if (nullptr == clip)
		assert(NULL);

	//Events* events = FindEvents(animName);

	DeleteDefine(_Type);
	m_mapPreDefineAnim.insert(make_pair((UINT)_Type, animName));
}

UINT CAnimator3D::FindDefineAnimation(wstring animName)
{
	auto iter = m_mapPreDefineAnim.begin();
	while (iter != m_mapPreDefineAnim.end())
	{
		if (iter->second == animName)
		{
			return iter->first;
		}
		else ++iter;

	}

	if(GetOwner()->IsItem())
	{
		return (UINT)GUN_ANIMATION_TYPE::END;
	}else
	{
		return (UINT)PLAYER_ANIMATION_TYPE::END;
	}
}

const wstring& CAnimator3D::GetDefineAnimationName(UINT _Type)
{
	auto iter = m_mapPreDefineAnim.find((UINT)_Type);
	if (iter != m_mapPreDefineAnim.end())
		return iter->second;

	return L"NO_ANIM_SET";
}

bool CAnimator3D::DeleteDefineAnimation(wstring animName)
{
	if (animName == L"") return false;

	auto iter = m_mapPreDefineAnim.begin();
	while (iter != m_mapPreDefineAnim.end())
	{
		if (iter->second == animName)
		{
			iter->second = L"";
			return true;
		}
		else ++iter;
	}
	return false;
}

void CAnimator3D::DeleteDefine(UINT _Type)
{
	if(GetOwner()->IsItem())
	{
		if (_Type == (UINT)GUN_ANIMATION_TYPE::END) return;
	}
	else
	{
		if (_Type == (UINT)PLAYER_ANIMATION_TYPE::END) return;
	}
	
	auto iter = m_mapPreDefineAnim.find((UINT)_Type);
	if (iter != m_mapPreDefineAnim.end())
	{
		iter = m_mapPreDefineAnim.erase(iter);
	};
}

Vec4 CAnimator3D::GetHeadPos()
{
	return m_vHeadPos;
}

// Animator3DUI에서 강제 플레이용도로 사용되고 있음
void CAnimator3D::Play(const wstring& _Name, bool _Loop)
{
	CAnimClip* pCheckClip = FindClip(_Name);
	Events* events = nullptr;

	if (nullptr == pCheckClip)
		return;

	m_fSpeedAdj = 1.0f;

	if (pCheckClip == m_pCurClip)
	{
		if (m_bLoop)
			return;

		m_pCurClip->Reset();

		events = FindEvents(m_pCurClip->GetName());
		if (events)
			events->startEvent();

		m_bStop = false;

		return;
	}

	if (m_pNextClip && pCheckClip == m_pNextClip)
	{
		if (m_bLoop)
			return;

		m_pNextClip->Reset();

		events = FindEvents(m_pNextClip->GetName());
		if (events)
			events->startEvent();

		m_bStop = false;

		return;
	}


	if (m_pCurClip)
	{
		//m_pCurClip->Reset();
		events = FindEvents(m_pCurClip->GetName());
		if (events)
			events->endEvent();
	}

	if (nullptr != pCheckClip)
	{
		if (m_bBlendMode)
		{
			if (m_pNextClip)
			{
				events = FindEvents(m_pNextClip->GetName());
				if (events)
					events->endEvent();
			}
			m_pNextClip = pCheckClip;
			m_pNextClip->Reset();

			SetBlend(true, 0.1f);

			events = FindEvents(m_pNextClip->GetName());
			if (events)
				events->startEvent();
		}
		else
		{
			m_pCurClip = pCheckClip;
			m_pCurClip->Reset();

			events = FindEvents(m_pCurClip->GetName());
			if (events)
				events->startEvent();
		}
	}
	m_bStop = false;
	m_bLoop = _Loop;
}

void CAnimator3D::Play(UINT _type, bool _Loop, bool _Force)
{
	wstring animName = GetDefineAnimationName(_type);

	if (animName == L"NO_ANIM_SET") return;

	CAnimClip* pCheckClip = FindClip(animName);
	Events* events;

	if (nullptr == pCheckClip)
		return;

	m_fSpeedAdj = 1.0f;

	if (pCheckClip == m_pCurClip)
	{
		if(m_bLoop)
			return;

		if(_Force)
		{
			m_pCurClip->Reset();

			events = FindEvents(m_pCurClip->GetName());
			if (events)
				events->startEvent();

			m_bStop = false;
		}
		
		return;
	}

	if (m_pNextClip && pCheckClip == m_pNextClip)
	{
		if (m_bLoop)
			return;

		if (_Force)
		{
			m_pNextClip->Reset();

			events = FindEvents(m_pNextClip->GetName());
			if (events)
				events->startEvent();

			m_bStop = false;
		}

		return;
	}

	if (m_pCurClip)
	{
		//m_pCurClip->Reset();
		events = FindEvents(m_pCurClip->GetName());
		if (events)
			events->endEvent();
	}

	if (m_bBlendMode)
	{
		if (m_pNextClip)
		{
			events = FindEvents(m_pNextClip->GetName());
			if (events)
				events->endEvent();
		}
		m_pNextClip = pCheckClip;
		m_pNextClip->Reset();
		
		SetBlend(true, 0.1f);

		events = FindEvents(m_pNextClip->GetName());
		if (events)
			events->startEvent();
	}
	else
	{
		m_pCurClip = pCheckClip;
		m_pCurClip->Reset();
		events = FindEvents(m_pCurClip->GetName());
		if (events)
			events->startEvent();
	}
	m_bStop = false;
	m_bLoop = _Loop;
}

std::shared_ptr<std::function<void()>>& CAnimator3D::StartEvent(const wstring key)
{
	Events* events = FindEvents(key);
	return events->startEvent.mEvent;
}
std::shared_ptr<std::function<void()>>& CAnimator3D::StartEvent(const UINT key)
{
	wstring animName = GetDefineAnimationName(key);
	if (animName == L"NO_SET_ANIM")
	{
		OutputDebugStringW(L"NO ANIMATION DEFINE!");
		assert(NULL);
	}

	Events* events = FindEvents(animName);
	return events->startEvent.mEvent;
}

std::shared_ptr<std::function<void()>>& CAnimator3D::CompleteEvent(const wstring key)
{
	Events* events = FindEvents(key);
	return events->completeEvent.mEvent;
}
std::shared_ptr<std::function<void()>>& CAnimator3D::CompleteEvent(const UINT key)
{
	wstring animName = GetDefineAnimationName(key);
	if (animName == L"NO_SET_ANIM")
	{
		OutputDebugStringW(L"NO ANIMATION DEFINE!");
		assert(NULL);
	}

	Events* events = FindEvents(animName);
	return events->completeEvent.mEvent;
}

std::shared_ptr<std::function<void()>>& CAnimator3D::EndEvent(const wstring key)
{
	Events* events = FindEvents(key);
	return events->endEvent.mEvent;
}
std::shared_ptr<std::function<void()>>& CAnimator3D::EndEvent(const UINT key)
{
	wstring animName = GetDefineAnimationName(key);
	if (animName == L"NO_SET_ANIM")
	{
		OutputDebugStringW(L"NO ANIMATION DEFINE!");
		assert(NULL);
	}

	Events* events = FindEvents(animName);
	if (nullptr == events)
		assert(NULL);

	return events->endEvent.mEvent;
}

std::shared_ptr<std::function<void()>>& CAnimator3D::ProgressEvent(const wstring key)
{
	Events* events = FindEvents(key);
	return events->progressEvent.mEvent;
}
std::shared_ptr<std::function<void()>>& CAnimator3D::ProgressEvent(const UINT key)
{
	wstring animName = GetDefineAnimationName(key);
	if (animName == L"NO_SET_ANIM")
	{
		OutputDebugStringW(L"NO ANIMATION DEFINE!");
		assert(NULL);
	}

	Events* events = FindEvents(animName);
	return events->progressEvent.mEvent;
}

Events* CAnimator3D::FindEvents(const wstring& name)
{
	std::map<wstring, Events*>::iterator iter
		= mEvents.find(name);

	if (iter == mEvents.end())
		return nullptr;

	return iter->second;
}

CAnimClip* CAnimator3D::FindClip(const wstring& name)
{
	std::map<wstring, CAnimClip*>::iterator iter
		= mClips.find(name);

	if (iter == mClips.end())
		return nullptr;

	return iter->second;
}

void CAnimator3D::SaveToLevelFile(FILE* _pFile)
{

	UINT vecSize = m_pVecBones.size();
	fwrite(&vecSize, sizeof(UINT), 1, _pFile);

	for (UINT i = 0; i < vecSize; ++i)
	{
		SaveWString(m_pVecBones[i].strBoneName, _pFile);
		fwrite(&m_pVecBones[i].iDepth, sizeof(int), 1, _pFile);
		fwrite(&m_pVecBones[i].iParentIndx, sizeof(int), 1, _pFile);
		fwrite(&m_pVecBones[i].matBone, sizeof(Matrix), 1, _pFile);
		fwrite(&m_pVecBones[i].matOffset, sizeof(Matrix), 1, _pFile);

		size_t count = m_pVecBones[i].vecKeyFrame.size();
		fwrite(&count, sizeof(size_t), 1, _pFile);
		for (const auto& pair : m_pVecBones[i].vecKeyFrame)
		{
			SaveWString(pair.first, _pFile);
			size_t vecSize = pair.second.size();
			fwrite(&vecSize, sizeof(size_t), 1, _pFile);
			for (size_t j = 0; j < vecSize; ++j)
			{
				fwrite(&pair.second[j], sizeof(tMTKeyFrame), 1, _pFile);
			}
		}
	}

	UINT mapSize = m_pMapClip.size();
	fwrite(&mapSize, sizeof(UINT), 1, _pFile);
	for (const auto& pair : m_pMapClip)
	{
		SaveWString(pair.second.strAnimName, _pFile);
		fwrite(&pair.second.dStartTime, sizeof(double), 1, _pFile);
		fwrite(&pair.second.dEndTime, sizeof(double), 1, _pFile);
		fwrite(&pair.second.dTimeLength, sizeof(double), 1, _pFile);
		fwrite(&pair.second.eMode, sizeof(int), 1, _pFile);
		fwrite(&pair.second.fUpdateTime, sizeof(float), 1, _pFile);
		fwrite(&pair.second.iStartFrame, sizeof(int), 1, _pFile);
		fwrite(&pair.second.iEndFrame, sizeof(int), 1, _pFile);
		fwrite(&pair.second.iFrameLength, sizeof(int), 1, _pFile);
		fwrite(&pair.second.iFrameCount, sizeof(int), 1, _pFile);
		int size = pair.second.vecTransKeyFrame.size();
		fwrite(&size, sizeof(int), 1, _pFile);
		for(int i = 0; i < pair.second.vecTransKeyFrame.size(); ++i)
		{
			fwrite(&pair.second.vecTransKeyFrame[i].vTranslate, sizeof(Vec4), 1, _pFile);
			fwrite(&pair.second.vecTransKeyFrame[i].vScale, sizeof(Vec4), 1, _pFile);
			fwrite(&pair.second.vecTransKeyFrame[i].qRot, sizeof(Vec4), 1, _pFile);
		}
		
	}

	fwrite(&m_iFrameCount, sizeof(int), 1, _pFile);
	fwrite(&m_bFinalMatUpdate, sizeof(bool), 1, _pFile);
	fwrite(&m_bLoop, sizeof(bool), 1, _pFile);
	fwrite(&m_bMultipleClip, sizeof(bool), 1, _pFile);

	fwrite(&m_iHeadIdx, sizeof(int), 1, _pFile);
	fwrite(&m_iCameraIdx, sizeof(int), 1, _pFile);
	fwrite(&m_iWeaponRHandIdx, sizeof(int), 1, _pFile);
	fwrite(&m_iWeaponMuzzleIdx, sizeof(int), 1, _pFile);

	fwrite(&m_iFireBreathIdx, sizeof(int), 1, _pFile);
	fwrite(&m_iTailWeaponIdx, sizeof(int), 1, _pFile);
	fwrite(&m_iKnuckleIdx, sizeof(int), 1, _pFile);
	fwrite(&m_iStomachIdx, sizeof(int), 1, _pFile);
	fwrite(&m_iMouseIdx, sizeof(int), 1, _pFile);
	fwrite(&m_iChestIdx, sizeof(int), 1, _pFile);

	fwrite(&m_vMuzzlePos, sizeof(Vec4), 1, _pFile);

	mapSize = m_mapPreDefineAnim.size();
	fwrite(&mapSize, sizeof(UINT), 1, _pFile);
	auto iter = m_mapPreDefineAnim.begin();
	while (iter != m_mapPreDefineAnim.end())
	{
		SaveWString(iter->second, _pFile);
		fwrite(&iter->first, sizeof(UINT), 1, _pFile);
		++iter;
	}
}

void CAnimator3D::LoadFromLevelFile(FILE* _pFile)
{
	UINT vecSize = 0;
	fread(&vecSize, sizeof(UINT), 1, _pFile);
	m_pVecBones.resize(vecSize);
	for (int i = 0; i < vecSize; ++i)
	{
		LoadWString(m_pVecBones[i].strBoneName, _pFile);
		fread(&m_pVecBones[i].iDepth, sizeof(int), 1, _pFile);
		fread(&m_pVecBones[i].iParentIndx, sizeof(int), 1, _pFile);
		fread(&m_pVecBones[i].matBone, sizeof(Matrix), 1, _pFile);
		fread(&m_pVecBones[i].matOffset, sizeof(Matrix), 1, _pFile);

		size_t count = 0;
		fread(&count, sizeof(size_t), 1, _pFile);
		for (size_t j = 0; j < count; ++j)
		{
			wstring animName;
			LoadWString(animName, _pFile);
			size_t vecSize;
			fread(&vecSize, sizeof(size_t), 1, _pFile);
			vector<tMTKeyFrame> vectMT;
			for (size_t k = 0; k < vecSize; ++k)
			{
				tMTKeyFrame _tMTKeyFrame;
				fread(&_tMTKeyFrame, sizeof(tMTKeyFrame), 1, _pFile);
				vectMT.push_back(_tMTKeyFrame);
			}

			m_pVecBones[i].vecKeyFrame.insert(make_pair(animName, vectMT));
		}
	}

	UINT mapSize = 0;
	fread(&mapSize, sizeof(int), 1, _pFile);
	for (int i = 0; i < mapSize; ++i)
	{
		tMTAnimClip tClip = {};

		LoadWString(tClip.strAnimName, _pFile);
		fread(&tClip.dStartTime, sizeof(double), 1, _pFile);
		fread(&tClip.dEndTime, sizeof(double), 1, _pFile);
		fread(&tClip.dTimeLength, sizeof(double), 1, _pFile);
		fread(&tClip.eMode, sizeof(int), 1, _pFile);
		fread(&tClip.fUpdateTime, sizeof(float), 1, _pFile);
		fread(&tClip.iStartFrame, sizeof(int), 1, _pFile);
		fread(&tClip.iEndFrame, sizeof(int), 1, _pFile);
		fread(&tClip.iFrameLength, sizeof(int), 1, _pFile);
		fread(&tClip.iFrameCount, sizeof(int), 1, _pFile);
		int size = 0;
		fread(&size, sizeof(int), 1, _pFile);
		tClip.vecTransKeyFrame.resize(size);
		for (int i = 0; i < size; ++i)
		{
			tFrameTrans frameTrans;
			fread(&frameTrans.vTranslate, sizeof(Vec4), 1, _pFile);
			fread(&frameTrans.vScale, sizeof(Vec4), 1, _pFile);
			fread(&frameTrans.qRot, sizeof(Vec4), 1, _pFile);

			tClip.vecTransKeyFrame.push_back(frameTrans);
		}

		//m_mapAnimClip.push_back(tClip);
		m_pMapClip.insert(make_pair(tClip.strAnimName, tClip));
	}
	fread(&m_iFrameCount, sizeof(int), 1, _pFile);
	fread(&m_bFinalMatUpdate, sizeof(bool), 1, _pFile);
	fread(&m_bLoop, sizeof(bool), 1, _pFile);
	fread(&m_bMultipleClip, sizeof(bool), 1, _pFile);

	fread(&m_iHeadIdx, sizeof(int), 1, _pFile);
	fread(&m_iCameraIdx, sizeof(int), 1, _pFile);
	fread(&m_iWeaponRHandIdx, sizeof(int), 1, _pFile);
	fread(&m_iWeaponMuzzleIdx, sizeof(int), 1, _pFile);

	fread(&m_iFireBreathIdx, sizeof(int), 1, _pFile);
	fread(&m_iTailWeaponIdx, sizeof(int), 1, _pFile);
	fread(&m_iKnuckleIdx, sizeof(int), 1, _pFile);
	fread(&m_iStomachIdx, sizeof(int), 1, _pFile);
	fread(&m_iMouseIdx, sizeof(int), 1, _pFile);
	fread(&m_iChestIdx, sizeof(int), 1, _pFile);

	fread(&m_vMuzzlePos, sizeof(Vec4), 1, _pFile);

	mapSize = m_mapPreDefineAnim.size();
	fread(&mapSize, sizeof(UINT), 1, _pFile);
	wstring animName = L"";
	UINT _type = (UINT)PLAYER_ANIMATION_TYPE::END;
	for (int i = 0; i < mapSize; ++i)
	{
		LoadWString(animName, _pFile);
		fread(&_type, sizeof(UINT), 1, _pFile);
		m_mapPreDefineAnim.insert(make_pair(_type, animName));
	}

	create_clip();
}

void CAnimator3D::check_mesh(Ptr<CMesh> _pMesh)
{
	UINT iBoneCount = _pMesh->GetBoneCount();
	
	m_pBoneFinalMatBuffer->Create(sizeof(Matrix), iBoneCount, SB_TYPE::READ_WRITE, true);
	m_pBoneFinalMatPosBuffer->Create(sizeof(Matrix), iBoneCount, SB_TYPE::READ_WRITE, true);
	m_pBoneFinalMatRotBuffer->Create(sizeof(Matrix), iBoneCount, SB_TYPE::READ_WRITE, true);
}

void CAnimator3D::check_mesh(Ptr<CMesh> _pMesh, CStructuredBuffer* _buffer)
{
	UINT iBoneCount = _pMesh->GetBoneCount();

	_buffer->Create(sizeof(Matrix), iBoneCount, SB_TYPE::READ_WRITE, true);
}

void CAnimator3D::create_clip()
{
	mEvents.clear();
	mClips.clear();

	wstring tempClipName = L"";
	for (auto& pair : m_pMapClip)
	{
		tMTAnimClip clip = pair.second;

		CAnimClip* pAnimClip = new CAnimClip();

		if (L"" == tempClipName)
			tempClipName = clip.strAnimName;

		pAnimClip->Create(clip.strAnimName, clip
			, clip.iStartFrame, clip.iEndFrame, true);

		mClips.insert(std::make_pair(clip.strAnimName, pAnimClip));
		Events* events = FindEvents(clip.strAnimName);

		if (nullptr == events)
			events = new Events();

		mEvents.insert(make_pair(clip.strAnimName, events));
	}

	m_pCurClip = mClips.at(tempClipName);
}

void CAnimator3D::CustomEvent(CAnimClip* _AnimClip)
{
	Events* events = nullptr;
	events = FindEvents(_AnimClip->GetName());
	int idx =  _AnimClip->GetClipIdx();

	if(_AnimClip->GetEventEndIdx() == idx)
	{
		if (events)
			events->endEvent();
	}
	if (_AnimClip->GetEventStartIdx() == idx)
	{
		if (events)
			events->startEvent();
	}
	if (_AnimClip->GetEventProgressIdx() == idx)
	{
		if (events)
			events->progressEvent();
	}
	if (_AnimClip->GetEventCompleteIdx() == idx)
	{
		if (events)
			events->completeEvent();
	}

}
