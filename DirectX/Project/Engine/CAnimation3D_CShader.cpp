#include "pch.h"
#include "CAnimation3D_CShader.h"

#include "CStructuredBuffer.h"

CAnimation3D_CShader::CAnimation3D_CShader(UINT _iGroupPerThreadX, UINT _iGroupPerThreadY, UINT _iGroupPerThreadZ)
	: m_pFrameDataBuffer(nullptr)
	, m_pOffsetMatBuffer(nullptr)
	, m_pOutputBuffer(nullptr)
{
	m_iGroupPerThreadX = _iGroupPerThreadX;
	m_iGroupPerThreadY = _iGroupPerThreadY;
	m_iGroupPerThreadZ = _iGroupPerThreadZ;
}

CAnimation3D_CShader::~CAnimation3D_CShader()
{
}

void CAnimation3D_CShader::UpdateData()
{
	// ����ȭ���� ����
	m_pFrameDataBuffer->UpdateData_CS(16, true); // t16
	m_pOffsetMatBuffer->UpdateData_CS(17, true); // t17
	m_pOutputBuffer->UpdateData_CS(0, false);   // u0

	m_iGroupX = (m_Const.arrInt[0] / m_iGroupPerThreadX) + 1;
	m_iGroupY = 1;
	m_iGroupZ = 1;
}

void CAnimation3D_CShader::Clear()
{
	// ������ ����ȭ���� Ŭ����	
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