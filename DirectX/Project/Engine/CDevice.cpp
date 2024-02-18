#include "pch.h"
#include "CDevice.h"

#include "CEngine.h"
#include "CConstBuffer.h"

#include "CResMgr.h"

CDevice::CDevice()
    : m_hWnd(nullptr)  
    , m_ViewPort{}
    , m_arrConstBuffer{}
{
}

CDevice::~CDevice()
{
    Safe_Del_Array(m_arrConstBuffer);
}

int CDevice::init(HWND _hWnd, UINT _iWidth, UINT _iHeight)
{
    m_hWnd = _hWnd;    
    m_vRenderResolution = Vec2((float)_iWidth, (float)_iHeight);

    GlobalData.Resolution = m_vRenderResolution;



    int iFlag = 0;
#ifdef _DEBUG
    iFlag = D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_FEATURE_LEVEL eLevel = D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_0;
      

    if (FAILED(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE
        , nullptr, iFlag
        , nullptr, 0
        , D3D11_SDK_VERSION
        , m_Device.GetAddressOf(), &eLevel
        , m_Context.GetAddressOf())))
    {
        MessageBox(nullptr, L"Device, Context 생성 실패", L"Device 초기화 에러", MB_OK);
        return E_FAIL;
    }

    if (FAILED(CreateSwapChain()))
    {
        MessageBox(nullptr, L"스왚체인 생성 실패", L"Device 초기화 에러", MB_OK);
        return E_FAIL;
    }

    if (FAILED(CreateView()))
    {
        MessageBox(nullptr, L"View 생성 실패", L"Device 초기화 에러", MB_OK);
        return E_FAIL;
    }

    //UINT m4x;
    //m_Device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &m4x);

    // ViewPort 설정
    m_ViewPort.TopLeftX = 0.f;
    m_ViewPort.TopLeftY = 0.f;

    Vec2 vWindowResol = CEngine::GetInst()->GetWindowResolution();
    m_ViewPort.Width = vWindowResol.x;
    m_ViewPort.Height = vWindowResol.y;

    m_ViewPort.MinDepth = 0.f;
    m_ViewPort.MaxDepth = 1.f;

    m_Context->RSSetViewports(1, &m_ViewPort);


    // RasterizerState 생성
    if (FAILED(CreateRasterizerState()))
    {
        MessageBox(nullptr, L"레스터라이져 스테이트 생성 실패", L"Device 초기화 에러", MB_OK);
        return E_FAIL;
    }

    // DepthStencilState 생성
    if (FAILED(CreateDepthStencilState()))
    {
        MessageBox(nullptr, L"DepthStencilState 생성 실패", L"Device 초기화 에러", MB_OK);
        return E_FAIL;
    }

    // BlendState 생성
    if (FAILED(CreateBlendState()))
    {
        MessageBox(nullptr, L"BlendState 생성 실패", L"Device 초기화 에러", MB_OK);
        return E_FAIL;
    }

    // 샘플러 생성
    if (FAILED(CreateSampler()))
    {
        MessageBox(nullptr, L"샘플러 생성 실패", L"Device 초기화 에러", MB_OK);
        return E_FAIL;
    }


    // 상수버퍼 생성
    CreateConstBuffer();


    return S_OK; // E_FAIL;
}

int CDevice::CreateSwapChain()
{
    // 스왚체인 설정
    DXGI_SWAP_CHAIN_DESC tDesc = {};

    tDesc.OutputWindow = m_hWnd;    // 출력 윈도우
    tDesc.Windowed = true;          // 창모드, 전체화면 모드

    tDesc.BufferCount = 1;              
    tDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    tDesc.BufferDesc.Width = (UINT)m_vRenderResolution.x;
    tDesc.BufferDesc.Height = (UINT)m_vRenderResolution.y;
    tDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    //tDesc.BufferDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    tDesc.BufferDesc.RefreshRate.Denominator = 1;
    tDesc.BufferDesc.RefreshRate.Numerator = 60;
    tDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    tDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER::DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    
    tDesc.SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_DISCARD;

    tDesc.SampleDesc.Count = 1;
    tDesc.SampleDesc.Quality = 0;
    tDesc.Flags = 0;
    

    // 스왚체인 생성
    ComPtr<IDXGIDevice>  pDXGIDevice;
    ComPtr<IDXGIAdapter> pAdapter;
    ComPtr<IDXGIFactory> pFactory;

    HRESULT hr = S_OK;

    hr = m_Device->QueryInterface(__uuidof(IDXGIDevice), (void**)pDXGIDevice.GetAddressOf());
    hr = pDXGIDevice->GetParent(__uuidof(IDXGIAdapter), (void**)pAdapter.GetAddressOf());
    hr = pAdapter->GetParent(__uuidof(IDXGIFactory), (void**)pFactory.GetAddressOf());

    hr = pFactory->CreateSwapChain(m_Device.Get(), &tDesc, m_SwapChain.GetAddressOf());

    return hr;
}

int CDevice::CreateView()
{
    ComPtr<ID3D11Texture2D> tex2D;
    m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)tex2D.GetAddressOf());
    
    // RenderTarget 용 텍스쳐 등록
    CResMgr::GetInst()->CreateTexture(L"RenderTargetTex", tex2D);

    // DepthStencil 용도 텍스쳐 생성
    CResMgr::GetInst()->CreateTexture(L"DepthStencilTex"
        , (UINT)m_vRenderResolution.x, (UINT)m_vRenderResolution.y
        , DXGI_FORMAT_D24_UNORM_S8_UINT, D3D11_BIND_DEPTH_STENCIL, D3D11_USAGE_DEFAULT);        


    return S_OK;
}

int CDevice::CreateRasterizerState()
{
    m_RSState[(UINT)RS_TYPE::CULL_BACK] = nullptr;

    D3D11_RASTERIZER_DESC Desc = {};
    Desc.CullMode = D3D11_CULL_MODE::D3D11_CULL_FRONT;
    Desc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
    DEVICE->CreateRasterizerState(&Desc, m_RSState[(UINT)RS_TYPE::CULL_FRONT].GetAddressOf());

    Desc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
    Desc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
    DEVICE->CreateRasterizerState(&Desc, m_RSState[(UINT)RS_TYPE::CULL_NONE].GetAddressOf());

    Desc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
    Desc.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
    DEVICE->CreateRasterizerState(&Desc, m_RSState[(UINT)RS_TYPE::WIRE_FRAME].GetAddressOf());

    return S_OK;
}

int CDevice::CreateDepthStencilState()
{
    // Less
    m_DSState[(UINT)DS_TYPE::LESS] = nullptr;

    // Less Equal
    D3D11_DEPTH_STENCIL_DESC Desc = {};
    Desc.DepthEnable = true;
    Desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
    Desc.StencilEnable = false;
    Desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    DEVICE->CreateDepthStencilState(&Desc, m_DSState[(UINT)DS_TYPE::LESS_EQUAL].GetAddressOf());
    
    // Greater
    Desc.DepthEnable = true;
    Desc.DepthFunc = D3D11_COMPARISON_GREATER;
    Desc.StencilEnable = false;
    Desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    DEVICE->CreateDepthStencilState(&Desc, m_DSState[(UINT)DS_TYPE::GREATER].GetAddressOf());

    // Greater Equal
    Desc.DepthEnable = true;
    Desc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;
    Desc.StencilEnable = false;
    Desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    DEVICE->CreateDepthStencilState(&Desc, m_DSState[(UINT)DS_TYPE::GREATER_EQUAL].GetAddressOf());

    // No Write
    Desc.DepthEnable = true;
    Desc.DepthFunc = D3D11_COMPARISON_LESS;
    Desc.StencilEnable = false;
    Desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    DEVICE->CreateDepthStencilState(&Desc, m_DSState[(UINT)DS_TYPE::NO_WRITE].GetAddressOf());

    // NoTest NoWrite
    Desc.DepthEnable = false;    
    Desc.StencilEnable = false;
    Desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    DEVICE->CreateDepthStencilState(&Desc, m_DSState[(UINT)DS_TYPE::NO_TEST_NO_WRITE].GetAddressOf());


    Desc.DepthEnable = true;
    Desc.DepthFunc = D3D11_COMPARISON_LESS;
    Desc.StencilEnable = false;
    Desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    DEVICE->CreateDepthStencilState(&Desc, m_DSState[(UINT)DS_TYPE::NO_TEST].GetAddressOf());

    // 볼륨 메쉬의 BackFace 보다 앞에 있는 영역에 stencil 값 증가
    Desc.DepthEnable = true;  
    Desc.DepthFunc = D3D11_COMPARISON_GREATER;          // 렌더링되는 볼륨메쉬의 뒷면이 더 멀어야 통과
    Desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;  // 볼륨메쉬는 깊이를 남기지 않음

    Desc.StencilEnable = true;                          // 스텐실 On

    Desc.BackFace.StencilFunc           = D3D11_COMPARISON_ALWAYS;  // 스텐실 테스트는 항상 통과
    Desc.BackFace.StencilPassOp         = D3D11_STENCIL_OP_INCR;    // 스텐실 값 1 증가
    Desc.BackFace.StencilFailOp         = D3D11_STENCIL_OP_KEEP;    // 유지
    Desc.BackFace.StencilDepthFailOp    = D3D11_STENCIL_OP_KEEP;    // 유지 
      

    // 볼륨 메쉬의 FrontFace 보다 뒤에 있는 영역에 stencil 값 유지
    Desc.DepthEnable = true;
    Desc.DepthFunc = D3D11_COMPARISON_LESS;             // 렌더링되는 볼륨메쉬의 앞면이 더 가까워야 통과
    Desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;  // 볼륨메쉬는 깊이를 남기지 않음

    Desc.StencilEnable = true;                          // 스텐실 On

    Desc.FrontFace.StencilFunc           = D3D11_COMPARISON_EQUAL;
    Desc.FrontFace.StencilPassOp         = D3D11_STENCIL_OP_KEEP;   // 유지
    Desc.FrontFace.StencilFailOp         = D3D11_STENCIL_OP_KEEP;   // 유지
    Desc.FrontFace.StencilDepthFailOp    = D3D11_STENCIL_OP_DECR;   // 깊이 테스트 실패 -> 스텐실 값 감소
    DEVICE->CreateDepthStencilState(&Desc, m_DSState[(UINT)DS_TYPE::STENCIL_CULL_TEST_T].GetAddressOf());
    

    // 볼륨 메쉬내부 판정, 스텐실값 변경 (CULL_NONE)
    Desc.DepthEnable = true;
    Desc.DepthFunc = D3D11_COMPARISON_GREATER;             // 렌더링되는 볼륨메쉬의 앞면이 더 가까워야 통과
    Desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;  // 볼륨메쉬는 깊이를 남기지 않음

    Desc.StencilEnable = true;                          // 스텐실 On

    Desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    Desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_INCR;   // 
    Desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;   // 유지
    Desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;   // 깊이 테스트 실패 -> 스텐실 값 감소

    Desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    Desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_DECR;   // 유지
    Desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;   // 유지
    Desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;   // 깊이 테스트 실패 -> 스텐실 값 감소

    DEVICE->CreateDepthStencilState(&Desc, m_DSState[(UINT)DS_TYPE::STENCIL_CULL_TEST_O].GetAddressOf());

    // 스텐실 값이 1을 유지하고 있는 영역에 대해서 통과
    Desc.DepthEnable = false;
    Desc.DepthFunc = D3D11_COMPARISON_GREATER;           // 렌더링되는 볼륨메쉬의 뒷면이 더 멀어야 통과
    Desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;   // 볼륨메쉬는 깊이를 남기지 않음

    Desc.StencilEnable = true;                           // 스텐실 On

    Desc.BackFace.StencilFunc = D3D11_COMPARISON_NOT_EQUAL;  // 스텐실 테스트는 항상 통과
    Desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_ZERO;        // 스텐실 값 0
    Desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_ZERO;        // 스텐실 값 0
    Desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_ZERO;   // 스텐실 값 0
    
    DEVICE->CreateDepthStencilState(&Desc, m_DSState[(UINT)DS_TYPE::STENCIL_CULL_DEPLOY].GetAddressOf());

    return S_OK;
}

int CDevice::CreateBlendState()
{
    // No Blend
    m_BSState[(UINT)BS_TYPE::DEFAULT] = nullptr;

    // Mask
    D3D11_BLEND_DESC Desc = {};
    Desc.AlphaToCoverageEnable = true;
    Desc.IndependentBlendEnable = false;   

    Desc.RenderTarget[0].BlendEnable = true;
    Desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    Desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
    Desc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;

    Desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    Desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    Desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;

    Desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    DEVICE->CreateBlendState(&Desc, m_BSState[(UINT)BS_TYPE::MASK].GetAddressOf());


    // Alpha Blend
    Desc.AlphaToCoverageEnable = false;
    Desc.IndependentBlendEnable = false;

    Desc.RenderTarget[0].BlendEnable = true;
    Desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    Desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    Desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;

    Desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    Desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
    Desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;

    //Desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    //Desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;

    Desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    DEVICE->CreateBlendState(&Desc, m_BSState[(UINT)BS_TYPE::ALPHA_BLEND].GetAddressOf());




    // One One
    Desc.AlphaToCoverageEnable = false;
    Desc.IndependentBlendEnable = false;

    Desc.RenderTarget[0].BlendEnable = true;
    Desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    Desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
    Desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;

    Desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    Desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    Desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;

    Desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    DEVICE->CreateBlendState(&Desc, m_BSState[(UINT)BS_TYPE::ONE_ONE].GetAddressOf());

    // DEFEREED_DECAL_BLEND
    Desc.AlphaToCoverageEnable = false;
    Desc.IndependentBlendEnable = true;

    Desc.RenderTarget[0].BlendEnable = true;
    Desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    Desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    Desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;

    Desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    Desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
    Desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;

    Desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    Desc.RenderTarget[1].BlendEnable = true;
    Desc.RenderTarget[1].BlendOp = D3D11_BLEND_OP_ADD;
    Desc.RenderTarget[1].SrcBlend = D3D11_BLEND_ONE;
    Desc.RenderTarget[1].DestBlend = D3D11_BLEND_ONE;

    Desc.RenderTarget[1].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    Desc.RenderTarget[1].SrcBlendAlpha = D3D11_BLEND_ZERO;
    Desc.RenderTarget[1].DestBlendAlpha = D3D11_BLEND_ONE;

    Desc.RenderTarget[1].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    DEVICE->CreateBlendState(&Desc, m_BSState[(UINT)BS_TYPE::DEFERRED_DECAL_BLEND].GetAddressOf());

    return S_OK;
}







int CDevice::CreateSampler()
{
    D3D11_SAMPLER_DESC tSamDesc = {};

    tSamDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    tSamDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    tSamDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    tSamDesc.Filter   = D3D11_FILTER_ANISOTROPIC;
    tSamDesc.MaxLOD = D3D11_FLOAT32_MAX;
    DEVICE->CreateSamplerState(&tSamDesc, m_Sampler[0].GetAddressOf());

    tSamDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    tSamDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    tSamDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    tSamDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    tSamDesc.MaxLOD = D3D11_FLOAT32_MAX;
    DEVICE->CreateSamplerState(&tSamDesc, m_Sampler[1].GetAddressOf());

    tSamDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    tSamDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    tSamDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

    tSamDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    //tSamDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    DEVICE->CreateSamplerState(&tSamDesc, m_Sampler[2].GetAddressOf());

    tSamDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
    tSamDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
    tSamDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;

    tSamDesc.BorderColor[0] = (FLOAT)0.f;
    tSamDesc.BorderColor[1] = (FLOAT)0.f;
    tSamDesc.BorderColor[2] = (FLOAT)0.f;
    tSamDesc.BorderColor[3] = (FLOAT)0.f;

    //tSamDesc.Filter = D3D11_FILTER_COMPARISON_ANISOTROPIC;
    m_iFilterIdx = 6;
    tSamDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;;
    tSamDesc.ComparisonFunc = D3D11_COMPARISON_LESS;
    DEVICE->CreateSamplerState(&tSamDesc, m_Sampler[3].GetAddressOf());

    CONTEXT->VSSetSamplers(0, 1, m_Sampler[0].GetAddressOf());
    CONTEXT->HSSetSamplers(0, 1, m_Sampler[0].GetAddressOf());
    CONTEXT->DSSetSamplers(0, 1, m_Sampler[0].GetAddressOf());
    CONTEXT->GSSetSamplers(0, 1, m_Sampler[0].GetAddressOf());
    CONTEXT->PSSetSamplers(0, 1, m_Sampler[0].GetAddressOf());
    CONTEXT->CSSetSamplers(0, 1, m_Sampler[0].GetAddressOf());

    CONTEXT->VSSetSamplers(1, 1, m_Sampler[1].GetAddressOf());
    CONTEXT->HSSetSamplers(1, 1, m_Sampler[1].GetAddressOf());
    CONTEXT->DSSetSamplers(1, 1, m_Sampler[1].GetAddressOf());
    CONTEXT->GSSetSamplers(1, 1, m_Sampler[1].GetAddressOf());
    CONTEXT->PSSetSamplers(1, 1, m_Sampler[1].GetAddressOf());
    CONTEXT->CSSetSamplers(1, 1, m_Sampler[1].GetAddressOf());

    CONTEXT->VSSetSamplers(2, 1, m_Sampler[2].GetAddressOf());
    CONTEXT->HSSetSamplers(2, 1, m_Sampler[2].GetAddressOf());
    CONTEXT->DSSetSamplers(2, 1, m_Sampler[2].GetAddressOf());
    CONTEXT->GSSetSamplers(2, 1, m_Sampler[2].GetAddressOf());
    CONTEXT->PSSetSamplers(2, 1, m_Sampler[2].GetAddressOf());
    CONTEXT->CSSetSamplers(2, 1, m_Sampler[2].GetAddressOf());

    CONTEXT->VSSetSamplers(3, 1, m_Sampler[3].GetAddressOf());
    CONTEXT->HSSetSamplers(3, 1, m_Sampler[3].GetAddressOf());
    CONTEXT->DSSetSamplers(3, 1, m_Sampler[3].GetAddressOf());
    CONTEXT->GSSetSamplers(3, 1, m_Sampler[3].GetAddressOf());
    CONTEXT->PSSetSamplers(3, 1, m_Sampler[3].GetAddressOf());
    CONTEXT->CSSetSamplers(3, 1, m_Sampler[3].GetAddressOf());
    
    return S_OK;
}

void CDevice::SetShadowSamplerFilter(int _FilterIdx)
{
    if (m_iFilterIdx == _FilterIdx) return;
    m_iFilterIdx = _FilterIdx;

    ID3D11SamplerState* pBackupSamplerState;
    CONTEXT->VSGetSamplers(3, 1, &pBackupSamplerState);

    ComPtr<ID3D11SamplerState> pNewSamplerState;
    D3D11_SAMPLER_DESC tSamDesc = {};
    pBackupSamplerState->GetDesc(&tSamDesc);
    tSamDesc.Filter = sampleFilters[m_iFilterIdx];

    DEVICE->CreateSamplerState(&tSamDesc, &pNewSamplerState);

    CONTEXT->VSSetSamplers(3, 1, pNewSamplerState.GetAddressOf());
    CONTEXT->HSSetSamplers(3, 1, pNewSamplerState.GetAddressOf());
    CONTEXT->DSSetSamplers(3, 1, pNewSamplerState.GetAddressOf());
    CONTEXT->GSSetSamplers(3, 1, pNewSamplerState.GetAddressOf());
    CONTEXT->PSSetSamplers(3, 1, pNewSamplerState.GetAddressOf());
    CONTEXT->CSSetSamplers(3, 1, pNewSamplerState.GetAddressOf());

    m_Sampler[3] = pNewSamplerState;

    pBackupSamplerState->Release();
}

void CDevice::SetSamplerAddress(int _AdsIdx)
{
    if (m_iAdsIdx == _AdsIdx) return;
    m_iAdsIdx = _AdsIdx;

    ID3D11SamplerState* pBackupSamplerState;
    CONTEXT->VSGetSamplers(3, 1, &pBackupSamplerState);

    ComPtr<ID3D11SamplerState> pNewSamplerState;
    D3D11_SAMPLER_DESC tSamDesc = {};
    pBackupSamplerState->GetDesc(&tSamDesc);

    if(_AdsIdx == 0)
    {
        tSamDesc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
        tSamDesc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
        tSamDesc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
    }
    if(_AdsIdx == 1)
    {
        tSamDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        tSamDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        tSamDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    }
    if (_AdsIdx == 2)
    {
        tSamDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
        tSamDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
        tSamDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    }
    if (_AdsIdx == 3)
    {
        tSamDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
        tSamDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
        tSamDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
    }

    DEVICE->CreateSamplerState(&tSamDesc, &pNewSamplerState);

    CONTEXT->VSSetSamplers(3, 1, pNewSamplerState.GetAddressOf());
    CONTEXT->HSSetSamplers(3, 1, pNewSamplerState.GetAddressOf());
    CONTEXT->DSSetSamplers(3, 1, pNewSamplerState.GetAddressOf());
    CONTEXT->GSSetSamplers(3, 1, pNewSamplerState.GetAddressOf());
    CONTEXT->PSSetSamplers(3, 1, pNewSamplerState.GetAddressOf());
    CONTEXT->CSSetSamplers(3, 1, pNewSamplerState.GetAddressOf());

    m_Sampler[3] = pNewSamplerState;

    pBackupSamplerState->Release();
}

void CDevice::CreateConstBuffer()
{
    m_arrConstBuffer[(UINT)CB_TYPE::TRANSFORM] = new CConstBuffer((UINT)CB_TYPE::TRANSFORM);
    m_arrConstBuffer[(UINT)CB_TYPE::TRANSFORM]->Create(sizeof(tTransform), 1);

    m_arrConstBuffer[(UINT)CB_TYPE::MATERIAL] = new CConstBuffer((UINT)CB_TYPE::MATERIAL);
    m_arrConstBuffer[(UINT)CB_TYPE::MATERIAL]->Create(sizeof(tMtrlConst), 1);

    m_arrConstBuffer[(UINT)CB_TYPE::GLOBAL] = new CConstBuffer((UINT)CB_TYPE::GLOBAL);
    m_arrConstBuffer[(UINT)CB_TYPE::GLOBAL]->Create(sizeof(tGlobal), 1);
}
