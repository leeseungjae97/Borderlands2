#pragma once

#include "CStructuredBuffer.h"

class CInstancingBuffer
	: public CSingleton<CInstancingBuffer>
{
private:
	ComPtr<ID3D11Buffer>		m_pInstancingBuffer;
	UINT						m_iMaxCount;
	vector<tInstancingData>		m_vecData;
	UINT						m_iInstanceCount;

public:
	void init();
	void Clear()
	{
		m_vecData.clear();
		m_iInstanceCount = 0;
	}
	void AddInstancingData(tInstancingData& _tData) { m_vecData.push_back(_tData); }

	UINT GetInstanceCount() { return m_vecData.empty() ? m_iInstanceCount : (UINT)m_vecData.size(); }
	ComPtr<ID3D11Buffer> GetBuffer() { return m_pInstancingBuffer; }
	UINT GetMaxCount() { return m_iMaxCount; }

	void SetData();

private:
	void Resize(UINT _iCount);

public:
	CInstancingBuffer();
	~CInstancingBuffer();
};
