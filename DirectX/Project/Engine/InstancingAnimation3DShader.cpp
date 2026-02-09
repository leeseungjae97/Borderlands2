#include "pch.h"
#include "InstancingAnimation3DShader.h"
#include "CStructuredBuffer.h"

InstancingAnimation3DShader::InstancingAnimation3DShader(UINT _iGroupPerThreadX, UINT _iGroupPerThreadY, UINT _iGroupPerThreadZ)
	: m_pAnimInstBuffer(nullptr),
	m_pBlendFrameDataBuffer(nullptr),
	m_pBoneFrameDataBuffer(nullptr),
	m_pOffsetMatBuffer(nullptr),
	m_pDestBuffer(nullptr)
{
	m_iGroupPerThreadX = _iGroupPerThreadX;
	m_iGroupPerThreadY = _iGroupPerThreadY;
	m_iGroupPerThreadZ = _iGroupPerThreadZ;
}

InstancingAnimation3DShader::~InstancingAnimation3DShader()
{

}


void InstancingAnimation3DShader::UpdateData()
{
	if (m_pAnimInstBuffer)
		m_pAnimInstBuffer->UpdateData_CS(18, true);

	if(m_pBlendFrameDataBuffer)
		m_pBlendFrameDataBuffer->UpdateData_CS(15, true);

	if (m_pBoneFrameDataBuffer)
		m_pBoneFrameDataBuffer->UpdateData_CS(16, true);

	if (m_pOffsetMatBuffer)
		m_pOffsetMatBuffer->UpdateData_CS(17, true);

	if(m_pDestBuffer)
		m_pDestBuffer->UpdateData_CS(0, false);

	m_Const.arrInt[0] = (int)m_InstCount;
	m_iGroupX = (m_InstCount + 31) / 32;
	m_iGroupY = (m_BoneCount + 31) / 32;
	m_iGroupZ = 1;
}

void InstancingAnimation3DShader::Clear()
{
	if (nullptr != m_pAnimInstBuffer)
	{
		m_pAnimInstBuffer->Clear_CS(true);
		m_pAnimInstBuffer = nullptr;
	}
	if (nullptr != m_pBlendFrameDataBuffer)
	{

		m_pBlendFrameDataBuffer->Clear_CS(true);
		m_pBlendFrameDataBuffer = nullptr;
	}
	if (nullptr != m_pOffsetMatBuffer)
	{
		m_pOffsetMatBuffer->Clear_CS(true);
		m_pOffsetMatBuffer = nullptr;
	}

	if (nullptr != m_pBoneFrameDataBuffer)
	{
		m_pBoneFrameDataBuffer->Clear_CS(true);
		m_pBoneFrameDataBuffer = nullptr;
	}

	if (nullptr != m_pDestBuffer)
	{
		m_pDestBuffer->Clear_CS(false);
		m_pDestBuffer = nullptr;
	}
}
