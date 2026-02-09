#include "pch.h"
#include "CTimeMgr.h"

#include "CEngine.h"
#include "CSimpleTextMgr.h"
#include "CInstancingBuffer.h"
#include "InstancingAnimatorMgr.h"


CTimeMgr::CTimeMgr()
	: m_llPrevCount{}
	, m_llCurCount{}
	, m_llFrequency{}
	, m_iCallCount(0)
	, m_fDeltaTime(0.f)
	, m_fTime(0.f)
{

}

CTimeMgr::~CTimeMgr()
{

}



void CTimeMgr::init()
{
	// 1�ʴ� ī���� ������
	QueryPerformanceFrequency(&m_llFrequency);
		
	QueryPerformanceCounter(&m_llCurCount);
	QueryPerformanceCounter(&m_llPrevCount);
}

void CTimeMgr::tick()
{	
	QueryPerformanceCounter(&m_llCurCount);

	// tick ���� �ð�
	m_fDeltaTime = (float)(m_llCurCount.QuadPart - m_llPrevCount.QuadPart) / (float)m_llFrequency.QuadPart;

	// ���� �ð�
	m_fTime += m_fDeltaTime;
	m_dTotalAccTime += m_fDeltaTime;

	// �Լ� ȣ�� Ƚ��
	++m_iCallCount;
		
	// ���� ī��Ʈ ���� ���� ī��Ʈ�� ����
	m_llPrevCount = m_llCurCount;		

	// GlobalData ����
	GlobalData.tDT = m_fDeltaTime;
	GlobalData.tAccTime += m_fDeltaTime;
}

void CTimeMgr::render()
{
	// 1�ʿ� �ѹ�
	static wchar_t szBuff[500] = {};

	if (1.f <= m_fTime)
	{
		InstancingAnimatorMgr * iamg = InstancingAnimatorMgr::GetInst();
		CInstancingBuffer* ib = CInstancingBuffer::GetInst();
		swprintf_s(szBuff, L"FPS : %d, DT : %f\n instance count : %d instance max count %d\n, anim instance count : %d\n,total bone % d, global frame cache % d\n global blend frame cache % d, global offset cache % d"
			, m_iCallCount
			, m_fDeltaTime
			, ib->GetInstanceCount()
			, ib->GetMaxCount()
			, iamg->GetAnimInstancingCount()
			, iamg->m_iTotalOutBones
			, iamg->m_globalFrameCache.size()
			, iamg->m_globalBlendFrameCache.size()
			, iamg->m_globalOffsetCache.size()
		);
		SetWindowText(CEngine::GetInst()->GetMainWnd(), szBuff);

		m_fTime = 0.f;
		m_iCallCount = 0;
	}
	CSimpleTextMgr::GetInst()->DrawFont(szBuff, 10, 20, 16, FONT_RGBA(255, 0, 0, 255));
}
