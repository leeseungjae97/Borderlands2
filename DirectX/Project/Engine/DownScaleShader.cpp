#include "pch.h"
#include "DownScaleShader.h"

#include "CEngine.h"

DownScaleShader::DownScaleShader(UINT _iGroupPerThreadX, UINT _iGroupPerThreadY, UINT _iGroupPerThreadZ)
{
	m_iGroupPerThreadX = _iGroupPerThreadX;
	m_iGroupPerThreadY = _iGroupPerThreadY;
	m_iGroupPerThreadZ = _iGroupPerThreadZ;
}

DownScaleShader::~DownScaleShader()
{
}

void DownScaleShader::UpdateData()
{
	m_pOriginTexBuffer->UpdateData_CS(0, true);
	m_pDownScaleTexBuffer->UpdateData_CS(0, false);

	Vec2 vResol = CEngine::GetInst()->GetWindowResolution();
	int iPixelCnt = vResol.x * vResol.y;
	int iGroupCnt = (unsigned int)ceil((float)(iPixelCnt / 16.f) / m_iGroupPerThreadX);
	m_iGroupX = iGroupCnt;
	m_iGroupY = 1;
	m_iGroupZ = 1;
}

void DownScaleShader::Clear()
{
	m_pOriginTexBuffer->Clear_CS(true);
	m_pDownScaleTexBuffer->Clear_CS(false);
}