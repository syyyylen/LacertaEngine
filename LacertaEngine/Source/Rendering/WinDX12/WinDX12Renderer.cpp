#include "WinDX12Renderer.h"
#include "../../Logger/Logger.h"

LacertaEngine::WinDX12Renderer::WinDX12Renderer() : m_device(nullptr), m_debug(nullptr), m_debugDevice(nullptr), m_factory(nullptr), m_adapter(nullptr)
{
}

LacertaEngine::WinDX12Renderer::~WinDX12Renderer()
{
}

void LacertaEngine::WinDX12Renderer::Initialize(int* context, int width, int height, int targetRefreshRate)
{
    HRESULT hr = D3D12GetDebugInterface(IID_PPV_ARGS(&m_debug));
    if(FAILED(hr))
    {
        LOG(Error, "Device : failed to get debug interface !");
        std::string errorMsg = std::system_category().message(hr);
        LOG(Error, errorMsg);
    }
    m_debug->EnableDebugLayer();

    hr = CreateDXGIFactory(IID_PPV_ARGS(&m_factory));
    if(FAILED(hr))
    {
        LOG(Error, "Device : failed to create DXGI factory !");
        std::string errorMsg = std::system_category().message(hr);
        LOG(Error, errorMsg);
    }

    // Getting the hardware adapter
    IDXGIFactory6* Factory6;
    if(SUCCEEDED(m_factory->QueryInterface(IID_PPV_ARGS(&Factory6))))
    {
        for(uint32_t AdapterIndex = 0; SUCCEEDED(Factory6->EnumAdapterByGpuPreference(AdapterIndex, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&m_adapter))); ++AdapterIndex)
        {
            DXGI_ADAPTER_DESC1 Desc;
            m_adapter->GetDesc1(&Desc);

            if(Desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
                continue;

            if(SUCCEEDED(D3D12CreateDevice((IUnknown*)m_adapter, D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device*), nullptr)))
                break;
        }
    }

    if(m_adapter == nullptr) {
        for (uint32_t AdapterIndex = 0; SUCCEEDED(m_factory->EnumAdapters1(AdapterIndex, &m_adapter)); ++AdapterIndex)
        {
            DXGI_ADAPTER_DESC1 Desc;
            m_adapter->GetDesc1(&Desc);

            if(Desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
                continue;

            if(SUCCEEDED(D3D12CreateDevice((IUnknown*)m_adapter, D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device*), nullptr)))
                break;
        }
    }

    hr = D3D12CreateDevice(m_adapter, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&m_device));
    if(FAILED(hr))
    {
        LOG(Error, "Device : failed to create device !");
        std::string errorMsg = std::system_category().message(hr);
        LOG(Error, errorMsg);
    }

    hr = m_device->QueryInterface(IID_PPV_ARGS(&m_debugDevice));
    if(FAILED(hr))
    {
        LOG(Error, "Device : failed to query debug device !");
        std::string errorMsg = std::system_category().message(hr);
        LOG(Error, errorMsg);
    }

    ID3D12InfoQueue* InfoQueue = 0;
    m_device->QueryInterface(IID_PPV_ARGS(&InfoQueue));

    InfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
    InfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);

    D3D12_MESSAGE_SEVERITY SupressSeverities[] = {
        D3D12_MESSAGE_SEVERITY_INFO
    };

    D3D12_MESSAGE_ID SupressIDs[] = {
        D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,
        D3D12_MESSAGE_ID_CLEARDEPTHSTENCILVIEW_MISMATCHINGCLEARVALUE,
        D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,
        D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,
    };

    D3D12_INFO_QUEUE_FILTER filter = {0};
    filter.DenyList.NumSeverities = ARRAYSIZE(SupressSeverities);
    filter.DenyList.pSeverityList = SupressSeverities;
    filter.DenyList.NumIDs = ARRAYSIZE(SupressIDs);
    filter.DenyList.pIDList = SupressIDs;

    InfoQueue->PushStorageFilter(&filter);
    InfoQueue->Release();

    DXGI_ADAPTER_DESC Desc;
    m_adapter->GetDesc(&Desc);
    std::wstring WideName = Desc.Description;
    std::string DeviceName = std::string(WideName.begin(), WideName.end());
    LOG(Debug, "Device : Using GPU : " + DeviceName);

    D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
    msQualityLevels.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    msQualityLevels.SampleCount = 4;
    msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
    msQualityLevels.NumQualityLevels = 0;
    hr = m_device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &msQualityLevels, sizeof(msQualityLevels));
    if(FAILED(hr))
    {
        LOG(Error, "Device : failed to check MSAA support !");
        std::string errorMsg = std::system_category().message(hr);
        LOG(Error, errorMsg);
    }

    m_msaaQualityLevel = msQualityLevels.NumQualityLevels;
}

void LacertaEngine::WinDX12Renderer::LoadShaders()
{
}

void LacertaEngine::WinDX12Renderer::SetBackbufferRenderTargetActive()
{
}

void LacertaEngine::WinDX12Renderer::PresentSwapChain()
{
}

void LacertaEngine::WinDX12Renderer::OnResizeWindow(unsigned width, unsigned height)
{
}

void LacertaEngine::WinDX12Renderer::UpdateConstantBuffer(void* buffer, ConstantBufferType cbufType)
{
}

void LacertaEngine::WinDX12Renderer::SetRasterizerCullState(bool cullFront)
{
}

void LacertaEngine::WinDX12Renderer::SetSamplerState(bool comparisonSampler)
{
}

LacertaEngine::RenderTarget* LacertaEngine::WinDX12Renderer::CreateRenderTarget(int width, int height, RenderTargetType renderTargetType, int& outRTidx, int numRt)
{
    return nullptr;
}

void LacertaEngine::WinDX12Renderer::ExecuteComputeShader(std::string name, UINT x, UINT y, UINT z)
{
}

LacertaEngine::Mesh* LacertaEngine::WinDX12Renderer::ImportMesh(std::string filePath)
{
    return nullptr;
}

LacertaEngine::Texture* LacertaEngine::WinDX12Renderer::CreateTexture(const wchar_t* filePath, int idx)
{
    return nullptr;
}

LacertaEngine::Texture* LacertaEngine::WinDX12Renderer::CreateTexture(int width, int height, TextureType type, int num, int mipNum, int bindFlags)
{
    return nullptr;
}
