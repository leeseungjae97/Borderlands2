#include "pch.h"
#include "mMRT.h"

#include "CDevice.h"
#include "CEngine.h"

MRT::MRT()
	: m_arrRT{}
	, m_RTCount(0)
	, m_ClearColor{}
	, m_bRenderTarget(false)
	, m_Viewport{}
{
}

MRT::~MRT()
{
}
void MRT::Create(Ptr<CTexture>* _arrRTTex, UINT _RTCount, Ptr<CTexture> _DSTex)
{
	if(nullptr != _arrRTTex)
	{
		for (UINT i = 0; i < _RTCount; ++i)
		{
			m_arrRT[i] = _arrRTTex[i];
		}	
	}
	
	m_RTCount = _RTCount;

	m_DSTex = _DSTex;

	m_Viewport.TopLeftX = 0;
	m_Viewport.TopLeftY = 0;

	if(_arrRTTex)
	{
		m_Viewport.Width = _arrRTTex[0]->Width();
		m_Viewport.Height = _arrRTTex[0]->Height();	
	}else
	{
		m_Viewport.Width = CEngine::GetInst()->GetWindowResolution().x;
		m_Viewport.Height = CEngine::GetInst()->GetWindowResolution().y;
	}
	

	m_Viewport.MinDepth = 0;
	m_Viewport.MaxDepth = 1;
}

void MRT::ClearTarget()
{
	for (UINT i = 0; i < m_RTCount; ++i)
	{
		CONTEXT->ClearRenderTargetView(m_arrRT[i]->GetRTV().Get(), m_ClearColor[i]);
	}
	if (nullptr != m_DSTex)
	{
		CONTEXT->ClearDepthStencilView(m_DSTex->GetDSV().Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0.f);
	}
}

void MRT::OMSet(bool _bStay)
{
	//m_bRenderTarget = true;

	ID3D11RenderTargetView* arrRTV[8] = {};
	for (UINT i = 0; i < m_RTCount; ++i)
	{
		arrRTV[i] = m_arrRT[i]->GetRTV().Get();
	}
	if (nullptr != m_DSTex)
	{
		CONTEXT->OMSetRenderTargets(m_RTCount, arrRTV, m_DSTex->GetDSV().Get());
	}
	else
	{
		ComPtr<ID3D11DepthStencilView> pDSV = nullptr;

		if (_bStay)
		{
			CONTEXT->OMGetRenderTargets(0, nullptr, pDSV.GetAddressOf());
		}

		CONTEXT->OMSetRenderTargets(m_RTCount, arrRTV, pDSV.Get());
	}

	CONTEXT->RSSetViewports(1, &m_Viewport);
}
