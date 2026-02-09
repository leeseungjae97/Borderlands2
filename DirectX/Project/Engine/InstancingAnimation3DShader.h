#pragma once
#include "CComputeShader.h"

#include "pch.h"

class CStructuredBuffer;

class InstancingAnimation3DShader :
    public CComputeShader
{
private:
	CStructuredBuffer* m_pAnimInstBuffer;
	CStructuredBuffer* m_pBlendFrameDataBuffer;
	CStructuredBuffer* m_pBoneFrameDataBuffer;
	CStructuredBuffer* m_pOffsetMatBuffer;
	CStructuredBuffer* m_pDestBuffer;

	UINT m_InstCount;
	UINT m_BoneCount;
public:
	void SetInstCount(UINT instCount) { m_InstCount = instCount; }
	void SetBoneCount(UINT instCount) { m_BoneCount = instCount; }

	void SetAnimInstBuffer(CStructuredBuffer* sb) { m_pAnimInstBuffer = sb; }

	void SetBlendFrameDataBuffer(CStructuredBuffer* sb) { m_pBlendFrameDataBuffer = sb; }
	void SetBoneFrameDataBuffer(CStructuredBuffer* sb) { m_pBoneFrameDataBuffer = sb; }
	void SetOffsetMatBuffer(CStructuredBuffer* sb) { m_pOffsetMatBuffer = sb; }

	void SetOutputBuffer(CStructuredBuffer* sb) { m_pDestBuffer = sb; }

public:
	virtual void UpdateData();
	virtual void Clear();

public:
	InstancingAnimation3DShader(UINT _iGroupPerThreadX, UINT _iGroupPerThreadY, UINT _iGroupPerThreadZ);
	virtual ~InstancingAnimation3DShader();
};

