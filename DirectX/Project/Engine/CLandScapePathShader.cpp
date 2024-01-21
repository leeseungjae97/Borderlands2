#include "pch.h"
#include "CLandScapePathShader.h"

CLandScapePathShader::CLandScapePathShader(UINT _iGroupPerThreadX, UINT _iGroupPerThreadY, UINT _iGroupPerThreadZ)
	: m_iLandWidth(0)
	, m_iLandHeight(0)
	, m_iFaceX(32)
	, m_iFaceY(32)
	, m_vLandPos(Vec3(0.f, 0.f, 0.f))
{
	m_iGroupPerThreadX = _iGroupPerThreadX;
	m_iGroupPerThreadY = _iGroupPerThreadY;
	m_iGroupPerThreadZ = _iGroupPerThreadZ;
}

CLandScapePathShader::~CLandScapePathShader()
{
}

void CLandScapePathShader::UpdateData()
{
	m_Const.arrInt[0] = m_iLandWidth;
	m_Const.arrInt[1] = m_iLandHeight;
	m_Const.arrInt[2] = m_iFaceX;
	m_Const.arrInt[3] = m_iFaceY;

	m_Const.arrV4[0] = m_vLandPos;

	m_pPathBuffer->UpdateData_CS(0, false);
}

void CLandScapePathShader::Clear()
{
	if (m_pPathBuffer)
	{
		m_pPathBuffer->Clear_CS(false);
		m_pPathBuffer = nullptr;
	}
}
