#include "pch.h"
#include "IndividualBoneSkinningShader.h"

#include "CStructuredBuffer.h"

IndividualBoneSkinningShader::IndividualBoneSkinningShader(UINT _iGroupPerThreadX, UINT _iGroupPerThreadY,
	UINT _iGroupPerThreadZ)
	: m_pOutputBuffer(nullptr)
{
	m_iGroupPerThreadX = _iGroupPerThreadX;
	m_iGroupPerThreadY = _iGroupPerThreadY;
	m_iGroupPerThreadZ = _iGroupPerThreadZ;
}

IndividualBoneSkinningShader::~IndividualBoneSkinningShader()
{
}

void IndividualBoneSkinningShader::UpdateData()
{
	if (m_pOutputBuffer)
		m_pOutputBuffer->UpdateData_CS(0, false);
	if(m_pBoneMatBuffer)
		m_pBoneMatBuffer->UpdateData_CS(1, false);
	m_iGroupX = 1;
	m_iGroupY = 1;
	m_iGroupZ = 1;
}

void IndividualBoneSkinningShader::Clear()
{
	if (nullptr != m_pOutputBuffer)
	{
		m_pOutputBuffer->Clear();
		m_pOutputBuffer = nullptr;
	}
	if (nullptr != m_pBoneMatBuffer)
	{
		m_pBoneMatBuffer->Clear();
		m_pBoneMatBuffer = nullptr;
	}
}
