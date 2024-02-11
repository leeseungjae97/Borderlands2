#include "pch.h"
#include "IndividualBoneSkinningShader.h"

#include "CStructuredBuffer.h"

IndividualBoneSkinningShader::IndividualBoneSkinningShader(UINT _iGroupPerThreadX, UINT _iGroupPerThreadY,
	UINT _iGroupPerThreadZ)
	: m_pOutputMatBuffer(nullptr)
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
	if (m_pOutputMatBuffer)
		m_pOutputMatBuffer->UpdateData_CS(0, false);
	if(m_pBoneMatBuffer)
		m_pBoneMatBuffer->UpdateData_CS(1, false);
	if (m_pOutputVertexBuffer)
		m_pOutputVertexBuffer->UpdateData_CS(2, false);
	m_iGroupX = 1;
	m_iGroupY = 1;
	m_iGroupZ = 1;
}

void IndividualBoneSkinningShader::Clear()
{
	if (nullptr != m_pOutputMatBuffer)
	{
		m_pOutputMatBuffer->Clear();
		m_pOutputMatBuffer = nullptr;
	}
	if (nullptr != m_pBoneMatBuffer)
	{
		m_pBoneMatBuffer->Clear();
		m_pBoneMatBuffer = nullptr;
	}
	if (nullptr != m_pOutputVertexBuffer)
	{
		m_pOutputVertexBuffer->Clear();
		m_pOutputVertexBuffer = nullptr;
	}	
}
