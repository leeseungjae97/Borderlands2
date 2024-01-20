#include "pch.h"
#include "CAnimation3DShader.h"

#include "CStructuredBuffer.h"

CAnimation3DShader::CAnimation3DShader(UINT _iGroupPerThreadX, UINT _iGroupPerThreadY, UINT _iGroupPerThreadZ)
	: m_pFrameDataBuffer(nullptr)
	, m_pOffsetMatBuffer(nullptr)
	, m_pOutputBuffer(nullptr)
{
	m_iGroupPerThreadX = _iGroupPerThreadX;
	m_iGroupPerThreadY = _iGroupPerThreadY;
	m_iGroupPerThreadZ = _iGroupPerThreadZ;
}

CAnimation3DShader::~CAnimation3DShader()
{
}

void CAnimation3DShader::UpdateData()
{
	if(m_pBlendFrameDataBuffer)
		m_pBlendFrameDataBuffer->UpdateData_CS(15, true);

	if(m_pFrameDataBuffer)
		m_pFrameDataBuffer->UpdateData_CS(16, true);

	if(m_pOffsetMatBuffer)
		m_pOffsetMatBuffer->UpdateData_CS(17, true);

	if(m_pOutputBuffer)
		m_pOutputBuffer->UpdateData_CS(0, false);

	m_iGroupX = (m_Const.arrInt[0] / m_iGroupPerThreadX) + 1;
	m_iGroupY = 1;
	m_iGroupZ = 1;
}

void CAnimation3DShader::Clear()
{
	if (nullptr != m_pBlendFrameDataBuffer)
	{
		m_pBlendFrameDataBuffer->Clear_CS(true);
		m_pBlendFrameDataBuffer = nullptr;
	}
	if (nullptr != m_pFrameDataBuffer)
	{
		m_pFrameDataBuffer->Clear_CS(true);
		m_pFrameDataBuffer = nullptr;
	}

	if (nullptr != m_pOffsetMatBuffer)
	{
		m_pOffsetMatBuffer->Clear_CS(true);
		m_pOffsetMatBuffer = nullptr;
	}

	if (nullptr != m_pOutputBuffer)
	{
		m_pOutputBuffer->Clear_CS(false);
		m_pOutputBuffer = nullptr;
	}
}
