#include "pch.h"
#include "CStructuredBuffer.h"

#include "CDevice.h"

CStructuredBuffer::CStructuredBuffer()
	: m_iElementSize(0)
	, m_iElementCount(0)
	, m_iRecentRegisterNum(0)
{
}

CStructuredBuffer::~CStructuredBuffer()
{
}

void CStructuredBuffer::Create(UINT _iElementSize, UINT _iElementCount
	, SB_TYPE _Type, bool _bSysAccess, void* _pSysMem)
{
	Clear();

	m_SB.Reset();
	m_SRV.Reset();
	m_UAV.Reset();

	m_SB_CPU_Read.Reset();
	m_SB_CPU_Write.Reset();

	m_Type = _Type;
	m_bSysAccess = _bSysAccess;

	m_iElementSize = _iElementSize;
	m_iElementCount = _iElementCount;

	UINT iBufferSize = m_iElementSize * _iElementCount;

	// 16����Ʈ ���� �޸� ����
	assert(!(iBufferSize % 16));

	// ������� ����
	m_tDesc.ByteWidth = iBufferSize;				// ���� ũ��
	m_tDesc.StructureByteStride = m_iElementSize;	// ������ ����

	if (SB_TYPE::READ_ONLY == m_Type)
	{
		m_tDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;	// Texture �������Ϳ� ���̵��ϱ� ���� �÷���
	}
	else if(SB_TYPE::READ_WRITE == m_Type)
	{
		m_tDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	}
	
	m_tDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;	// ����ȭ ���� üũ
	m_tDesc.Usage = D3D11_USAGE_DEFAULT;
	m_tDesc.CPUAccessFlags = 0;	

	if (nullptr == _pSysMem)
	{
		if (FAILED(DEVICE->CreateBuffer(&m_tDesc, nullptr, m_SB.ReleaseAndGetAddressOf())))
		{
			assert(nullptr);
		}
	}
	else
	{
		D3D11_SUBRESOURCE_DATA tSubData = {};
		tSubData.pSysMem = _pSysMem;

		HRESULT hr = DEVICE->CreateBuffer(&m_tDesc, &tSubData, m_SB.ReleaseAndGetAddressOf());
		if (hr)
		{
			assert(nullptr);
		}
	}


	// ShaderResourceView ����
	D3D11_SHADER_RESOURCE_VIEW_DESC	m_SRVDesc = {};

	m_SRVDesc.ViewDimension = D3D_SRV_DIMENSION_BUFFEREX;
	m_SRVDesc.BufferEx.NumElements = m_iElementCount;

	if (FAILED(DEVICE->CreateShaderResourceView(m_SB.Get(), &m_SRVDesc, m_SRV.ReleaseAndGetAddressOf())))
	{
		assert(nullptr);
	}

	if (SB_TYPE::READ_WRITE == m_Type)
	{

		D3D11_UNORDERED_ACCESS_VIEW_DESC m_UABDesc = {};
		m_UABDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		m_UABDesc.Buffer.NumElements = m_iElementCount;

		if (FAILED(DEVICE->CreateUnorderedAccessView(m_SB.Get(), &m_UABDesc, m_UAV.ReleaseAndGetAddressOf())))
		{
			assert(nullptr);
		}
	}

	// CPU Access ���� ����
	// CPU Access  
	if (m_bSysAccess)
	{
		D3D11_BUFFER_DESC tCPUDessc = m_tDesc;

		// GPU -> CPU Read (Staging Buffer)
		tCPUDessc.BindFlags = 0;
		tCPUDessc.Usage = D3D11_USAGE_STAGING;
		tCPUDessc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		if (FAILED(DEVICE->CreateBuffer(&tCPUDessc, nullptr, m_SB_CPU_Read.ReleaseAndGetAddressOf())))
		{
			assert(nullptr);
		}

		// CPU -> GPU Write (Dynamic Buffer for SetData)
		tCPUDessc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		tCPUDessc.Usage = D3D11_USAGE_DYNAMIC;
		tCPUDessc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		if (FAILED(DEVICE->CreateBuffer(&tCPUDessc, nullptr, m_SB_CPU_Write.ReleaseAndGetAddressOf())))
		{
			assert(nullptr);
		}
	}
	//DEVICE->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof(char) * bufName.length(), &bufName);
	//DEVICE->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof("testestest") - 1, "testestest");
}

void CStructuredBuffer::SetData(void* _pSrc, UINT _iSize)
{
	if (nullptr == _pSrc || nullptr == m_SB_CPU_Write)	
		return;	

	UINT iSize = _iSize;
	if (0 == iSize)
	{
		iSize = GetBufferSize();
	}

	// CPU -> CPU WriteBuffer
	D3D11_MAPPED_SUBRESOURCE tSub = {};
	CONTEXT->Map(m_SB_CPU_Write.Get(), 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &tSub);
	memcpy(tSub.pData, _pSrc, iSize);
	CONTEXT->Unmap(m_SB_CPU_Write.Get(), 0);

	// CPU WriteBuffer -> Main Buffer
	CONTEXT->CopyResource(m_SB.Get(), m_SB_CPU_Write.Get());
}

void CStructuredBuffer::GetData(void* _pDst)
{
	// Main Buffer -> CPU ReadBuffer
	CONTEXT->CopyResource(m_SB_CPU_Read.Get(), m_SB.Get());

	// CPU ReadBuffer -> CPU
	D3D11_MAPPED_SUBRESOURCE tSub = {};
	CONTEXT->Map(m_SB_CPU_Read.Get(), 0, D3D11_MAP::D3D11_MAP_READ, 0, &tSub);
	memcpy(_pDst, tSub.pData, GetBufferSize());
	CONTEXT->Unmap(m_SB_CPU_Read.Get(), 0);
}

void CStructuredBuffer::CopyData()
{
	CONTEXT->CopyResource(m_SB_CPU_Read.Get(), m_SB.Get());
}

template <typename T>
void CStructuredBuffer::GetArrData(T* _pDst)
{
	// Main Buffer -> CPU ReadBuffer
	CONTEXT->CopyResource(m_SB_CPU_Read.Get(), m_SB.Get());

	// CPU ReadBuffer -> CPU
	D3D11_MAPPED_SUBRESOURCE tSub = {};
	CONTEXT->Map(m_SB_CPU_Read.Get(), 0, D3D11_MAP::D3D11_MAP_READ, 0, &tSub);
	T* data = reinterpret_cast<T*>(tSub.pData);
	for(int i = 0; i < m_iElementCount; ++i)
	{
		T t = data[i];
	}
	//memcpy(_pDst, tSub.pData, GetBufferSize());
	CONTEXT->Unmap(m_SB_CPU_Read.Get(), 0);
}


void CStructuredBuffer::GetMatrixData(vector<Matrix>& _pDst)
{
	if (nullptr == m_SB_CPU_Read)
		return;

	CONTEXT->CopyResource(m_SB_CPU_Read.Get(), m_SB.Get());

	if (_pDst.size() < m_iElementCount)
	{
		_pDst.clear();
		_pDst.reserve(m_iElementCount);
	}

	D3D11_MAPPED_SUBRESOURCE tSub = {};
	if (SUCCEEDED(CONTEXT->Map(m_SB_CPU_Read.Get(), 0, D3D11_MAP::D3D11_MAP_READ, 0, &tSub)))
	{
		if (Matrix* data = reinterpret_cast<Matrix*>(tSub.pData))
		{
			_pDst.assign(data, data + m_iElementCount);
		}

		CONTEXT->Unmap(m_SB_CPU_Read.Get(), 0);
	}
}

void CStructuredBuffer::GetMatrixData(Matrix* _pDst, int _Idx)
{
	CONTEXT->CopyResource(m_SB_CPU_Read.Get(), m_SB.Get());

	D3D11_MAPPED_SUBRESOURCE tSub = {};
	CONTEXT->Map(m_SB_CPU_Read.Get(), 0, D3D11_MAP::D3D11_MAP_READ, 0, &tSub);
	Matrix* data = reinterpret_cast<Matrix*>(tSub.pData);
	*_pDst = data[_Idx];

	CONTEXT->Unmap(m_SB_CPU_Read.Get(), 0);
}


void CStructuredBuffer::UpdateData(UINT _iRegisterNum, UINT _iPipeLineStage)
{
	m_iRecentRegisterNum = _iRegisterNum;

	if (PIPELINE_STAGE::PS_VERTEX & _iPipeLineStage)
	{
		CONTEXT->VSSetShaderResources(_iRegisterNum, 1, m_SRV.GetAddressOf());
	}

	if (PIPELINE_STAGE::PS_HULL & _iPipeLineStage)
	{
		CONTEXT->HSSetShaderResources(_iRegisterNum, 1, m_SRV.GetAddressOf());
	}

	if (PIPELINE_STAGE::PS_DOMAIN & _iPipeLineStage)
	{
		CONTEXT->DSSetShaderResources(_iRegisterNum, 1, m_SRV.GetAddressOf());
	}

	if (PIPELINE_STAGE::PS_GEOMETRY & _iPipeLineStage)
	{
		CONTEXT->GSSetShaderResources(_iRegisterNum, 1, m_SRV.GetAddressOf());
	}

	if (PIPELINE_STAGE::PS_PIXEL & _iPipeLineStage)
	{
		CONTEXT->PSSetShaderResources(_iRegisterNum, 1, m_SRV.GetAddressOf());
	}
}

void CStructuredBuffer::UpdateData_CS(UINT _iRegisterNum, bool _IsShaderRes)
{
	m_iRecentRegisterNum = _iRegisterNum;
	
	if (_IsShaderRes)
	{
		CONTEXT->CSSetShaderResources(_iRegisterNum, 1, m_SRV.GetAddressOf());
	}
	else
	{
		UINT i = -1;
		CONTEXT->CSSetUnorderedAccessViews(_iRegisterNum, 1, m_UAV.GetAddressOf(), &i);
	}	
}

void CStructuredBuffer::Clear()
{
	ID3D11ShaderResourceView* pSRV = nullptr;
	CONTEXT->VSSetShaderResources(m_iRecentRegisterNum, 1, &pSRV);
	CONTEXT->HSSetShaderResources(m_iRecentRegisterNum, 1, &pSRV);
	CONTEXT->DSSetShaderResources(m_iRecentRegisterNum, 1, &pSRV);
	CONTEXT->GSSetShaderResources(m_iRecentRegisterNum, 1, &pSRV);
	CONTEXT->PSSetShaderResources(m_iRecentRegisterNum, 1, &pSRV);
	CONTEXT->CSSetShaderResources(m_iRecentRegisterNum, 1, &pSRV);

}

void CStructuredBuffer::Clear_CS(bool _IsShaderRes)
{
	if (_IsShaderRes)
	{
		ID3D11ShaderResourceView* pSRV = nullptr;
		CONTEXT->CSSetShaderResources(m_iRecentRegisterNum, 1, &pSRV);
	}
	else
	{
		ID3D11UnorderedAccessView* pUAV = nullptr;
		UINT i = -1;
		CONTEXT->CSSetUnorderedAccessViews(m_iRecentRegisterNum, 1, &pUAV, &i);
	}
}