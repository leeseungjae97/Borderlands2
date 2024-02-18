#include "pch.h"
#include "UpSamplingShader.h"

#include "CEngine.h"

UpSamplingShader::UpSamplingShader(UINT _iGroupPerThreadX, UINT _iGroupPerThreadY, UINT _iGroupPerThreadZ)
{
	m_iGroupPerThreadX = _iGroupPerThreadX;
	m_iGroupPerThreadY = _iGroupPerThreadY;
	m_iGroupPerThreadZ = _iGroupPerThreadZ;
}

UpSamplingShader::~UpSamplingShader()
{
}

void UpSamplingShader::Clear()
{
	if (nullptr != m_pInput)
	{
		m_pInput->Clear_CS(true);
		m_pInput = nullptr;
	}

	if (nullptr != m_pOutput)
	{
		m_pOutput->Clear_CS(false);
		m_pOutput = nullptr;
	}
}

void UpSamplingShader::UpdateData()
{
	m_pInput->UpdateData_CS(0, true);
	m_pOutput->UpdateData_CS(0, false);

	Vec2 resol = CEngine::GetInst()->GetWindowResolution();

	m_iGroupX = resol.x / 16;
	m_iGroupY = resol.y / 16;
	m_iGroupZ = 1;
}
