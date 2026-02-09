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
	// 1초당 카운팅 증가량
	QueryPerformanceFrequency(&m_llFrequency);
		
	QueryPerformanceCounter(&m_llCurCount);
	QueryPerformanceCounter(&m_llPrevCount);
}

void CTimeMgr::tick()
{	
	QueryPerformanceCounter(&m_llCurCount);

	// tick 간격 시간
	m_fDeltaTime = (float)(m_llCurCount.QuadPart - m_llPrevCount.QuadPart) / (float)m_llFrequency.QuadPart;

	// 누적 시간
	m_fTime += m_fDeltaTime;
	m_dTotalAccTime += m_fDeltaTime;

	// 함수 호출 횟수
	++m_iCallCount;
		
	// 이전 카운트 값을 현재 카운트로 갱신
	m_llPrevCount = m_llCurCount;		

	// GlobalData 갱신
	GlobalData.tDT = m_fDeltaTime;
	GlobalData.tAccTime += m_fDeltaTime;
}

void CTimeMgr::render()
{
	// 1초에 한번
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
			, ib->GetAnimInstancingCount()
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
