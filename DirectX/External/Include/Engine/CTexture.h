#pragma once
#include "CRes.h"

#include <DirectXTex\DirectXTex.h>

#ifdef _DEBUG
#pragma comment(lib, "DirectXTex//DirectXTex_debug")
#else 
#pragma comment(lib, "DirectXTex//DirectXTex")
#endif

class CTexture :
    public CRes
{
private:
    ComPtr<ID3D11Texture2D>             m_Tex2D;
    ComPtr<ID3D11ShaderResourceView>    m_SRV;
    ComPtr<ID3D11RenderTargetView>	    m_RTV;
    ComPtr<ID3D11DepthStencilView>	    m_DSV;
    ComPtr<ID3D11UnorderedAccessView>   m_UAV;

    D3D11_TEXTURE2D_DESC                m_Desc;
    ScratchImage                        m_Image;

    UINT                                m_iRecentNum;

public:
    int Create(UINT _Width, UINT _Height, DXGI_FORMAT _pixelformat
        , UINT _BindFlag, D3D11_USAGE _Usage);
    int Create(UINT _Width, UINT _Height, DXGI_FORMAT _pixelformat, D3D11_UNORDERED_ACCESS_VIEW_DESC _uavDesc
        , UINT _BindFlag, D3D11_USAGE _Usage);
    int CreateArrayTexture(const vector<Ptr<CTexture>>& _vecTex, int _iMipLevel);
    void GenerateMip(UINT _iMipLevel);

    int Create(ComPtr<ID3D11Texture2D> _tex2D);
    int Create(ComPtr<ID3D11ShaderResourceView> _tex2D);

public:
    float Width() { return (float)m_Desc.Width; }
	float Height() { return (float)m_Desc.Height; }

    UINT GetArraySize() { return m_Desc.ArraySize; }
    UINT GetRowPitch()const { return (UINT)m_Image.GetImages()->rowPitch; }
    UINT GetSlicePitch()const { return (UINT)m_Image.GetImages()->slicePitch; }
    void* GetSysMem() { return m_Image.GetPixels(); }

    ScratchImage* GetScratchImage() { return &m_Image; }

    void ResizeTex(UINT _iWidth, UINT _iHeight, ScratchImage* image);

    const Image* GetImage(int index) { return m_Image.GetImage(0, 0, index); }
    const Image* GetImages() { return m_Image.GetImages(); }

    const D3D11_TEXTURE2D_DESC&         GetDesc()   { return m_Desc; }
	ComPtr<ID3D11Texture2D>             GetTex2D()  { return m_Tex2D; }
	ComPtr<ID3D11ShaderResourceView>    GetSRV()    { return m_SRV; }
	ComPtr<ID3D11RenderTargetView>	    GetRTV()    { return m_RTV; }
	ComPtr<ID3D11DepthStencilView>	    GetDSV()    { return m_DSV; }
	ComPtr<ID3D11UnorderedAccessView>   GetUAV()    { return m_UAV; }

    void SetSRV(ComPtr<ID3D11ShaderResourceView> srv) { m_SRV = srv; }

private:
    virtual int Load(const wstring& _strFilePath) override;
    int Load(const wstring& _strFilePath, int _iMipLevel);
public:
    virtual int Save(const wstring& _strRelativePath) override;

public:
    // _PipelineStage : PIPELINE_STAGE
    void UpdateData(int _iRegisterNum, int _PipelineStage);
    void UpdateData_CS(int _iRegisterNum, bool _bShaderRes);
    void Clear();
    static void Clear(int _iRegisterNum);
    void Clear_CS(bool _bShaderRes);
    bool IsCube() { return m_Desc.MiscFlags & D3D11_RESOURCE_MISC_TEXTURECUBE ? true : false; }


private:
    virtual void UpdateData() override;

public:
    CTexture(bool _bEngine = false);
    ~CTexture();
};

