#include "pch.h"
#include "CColorMapShader.h"

#include "CStructuredBuffer.h"

CColorMapShader::CColorMapShader(UINT _iGroupPerThreadX, UINT _iGroupPerThreadY, UINT _iGroupPerThreadZ)
	: m_pInput(nullptr)
{
	m_iGroupPerThreadX = _iGroupPerThreadX;
	m_iGroupPerThreadY = _iGroupPerThreadY;
	m_iGroupPerThreadZ = _iGroupPerThreadZ;
}

CColorMapShader::~CColorMapShader()
{
}

void CColorMapShader::UpdateData()
{
	m_Const.arrInt[0] = (int)m_pColorMap->Width();
	m_Const.arrInt[1] = (int)m_pColorMap->Height();

	m_Const.arrV2[0] = m_vScale;

	// 높이맵
	m_pColorMap->UpdateData_CS(0, false);

	// 브러쉬
	m_pBrushTex->UpdateData_CS(0, true);

	// 피킹정보
	if (nullptr != m_pInput)
	{
		m_pInput->UpdateData_CS(16, true);
	}

	// 실행 시킬 스레드 그룹 수 지정
	m_iGroupX = ((UINT)m_pColorMap->Width() / m_iGroupPerThreadX) + 1;
	m_iGroupY = ((UINT)m_pColorMap->Height() / m_iGroupPerThreadY) + 1;
	m_iGroupZ = 1;
}

void CColorMapShader::Clear()
{
	if (nullptr != m_pColorMap)
	{
		m_pColorMap->Clear_CS(false);
		m_pColorMap = nullptr;
	}

	if (nullptr != m_pBrushTex)
	{
		m_pBrushTex->Clear(0);
		m_pBrushTex = nullptr;
	}

	if (nullptr != m_pInput)
	{
		m_pInput->Clear();
		m_pInput = nullptr;
	}
}
