#pragma once

#include "CSingleton.h"
#include "CTexture.h"
#include "ptr.h"

class ShaderEffect
	: public CSingleton<ShaderEffect>
{
public:
	ID3D11Buffer* m_DownScaleBuffer;
	ID3D11UnorderedAccessView* m_DownScaleUAV;
	ID3D11ShaderResourceView* m_DownScaleSRV;
	ID3D11Buffer* m_LuminanceAverageBuffer;
	ID3D11UnorderedAccessView* m_LuminanceAverageUAV;
	ID3D11ShaderResourceView* m_LuminanceAverageSRV;
	ID3D11Buffer* m_PrevFrameLumAverageBuffer;
	ID3D11UnorderedAccessView* m_PrevFrameLumAverageUAV;
	ID3D11ShaderResourceView* m_PrevFrameLumAverageSRV;
	ID3D11Texture2D* m_DownScaleRT;
	ID3D11UnorderedAccessView* m_DownScaleRTUAV;
	ID3D11ShaderResourceView* m_DownScaleRTSRV;
	ID3D11Texture2D* m_BloomRT;
	ID3D11UnorderedAccessView* m_BloomUAV;
	ID3D11ShaderResourceView* m_BloomSRV;
	ID3D11Texture2D* m_Temp1RT;
	ID3D11UnorderedAccessView* m_Temp1UAV;
	ID3D11ShaderResourceView* m_Temp1SRV;
	ID3D11Texture2D* m_Temp2RT;
	ID3D11UnorderedAccessView* m_Temp2UAV;
	ID3D11ShaderResourceView* m_Temp2SRV;

	Ptr<CTexture> m_pBloomTex;

	UINT GroupCount;
	UINT GroupSize;

	//bool m_bInit;
	//float m_fAdapTimer;
public:
	void Init();
	void render();

public:
	ShaderEffect();
	~ShaderEffect();
};

