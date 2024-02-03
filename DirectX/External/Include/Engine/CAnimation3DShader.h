#pragma once
#include "CComputeShader.h"
class CStructuredBuffer;

class CAnimation3DShader :
    public CComputeShader
{
    private:
	CStructuredBuffer* m_pFrameDataBuffer;			
	CStructuredBuffer* m_pBlendFrameDataBuffer;
	CStructuredBuffer* m_pOffsetMatBuffer;			
	CStructuredBuffer* m_pOutputBuffer;				

public:
	void SetBoneCount(int _iBoneCount) { m_Const.arrInt[0] = _iBoneCount; }
	void SetFrameIndex(int _iFrameIdx) { m_Const.arrInt[1] = _iFrameIdx; }
	void SetFrameNextIndex(int _iNextIdx) { m_Const.arrInt[2] = _iNextIdx; }
	void SetIsBlend(bool _bIsBlend) { m_Const.arrInt[3] = _bIsBlend; }
	void SetNextClipFrameIndex(int _iLastFrame) { m_Const.arrInt[4] = _iLastFrame; }

	void SetFrameRatio(float _fFrameRatio) { m_Const.arrFloat[0] = _fFrameRatio; }
	void SetBlendRatio(float _fFrameRatio) { m_Const.arrFloat[1] = _fFrameRatio; }

	void SetFrameDataBuffer(CStructuredBuffer* _buffer) { m_pFrameDataBuffer = _buffer; }
	void SetOffsetMatBuffer(CStructuredBuffer* _buffer) { m_pOffsetMatBuffer = _buffer; }
	void SetOutputBuffer(CStructuredBuffer* _buffer) { m_pOutputBuffer = _buffer; }
	void SetBlendFrameDataBuffer(CStructuredBuffer* _buffer) { m_pBlendFrameDataBuffer = _buffer; }

public:
	virtual void UpdateData();
	virtual void Clear();

public:
	CAnimation3DShader(UINT _iGroupPerThreadX, UINT _iGroupPerThreadY, UINT _iGroupPerThreadZ);
	virtual ~CAnimation3DShader();
};

