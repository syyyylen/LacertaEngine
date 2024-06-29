#include "WinDX12Renderer.h"
#include "../../Logger/Logger.h"

LacertaEngine::WinDX12Renderer::WinDX12Renderer()
    : m_device(nullptr), m_debug(nullptr), m_debugDevice(nullptr), m_factory(nullptr), m_adapter(nullptr), m_swapChain(nullptr),
      m_graphicsQueue(nullptr), m_computeQueue(nullptr), m_copyQueue(nullptr), m_commandAllocator(nullptr), m_commandList(nullptr),
      m_msaaQualityLevel(0)
{
}

LacertaEngine::WinDX12Renderer::~WinDX12Renderer()
{
    delete m_graphicsQueue;
    delete m_computeQueue;
    delete m_copyQueue;
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

    hr = CreateDXGIFactory1(IID_PPV_ARGS(&m_factory));
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
    msQualityLevels.Format = m_swapChainFormat;
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

    // Command Queues creation
    m_graphicsQueue = new WinDX12CommandQueue(m_device, D3D12_COMMAND_LIST_TYPE_DIRECT);
    m_computeQueue = new WinDX12CommandQueue(m_device, D3D12_COMMAND_LIST_TYPE_COMPUTE);
    m_copyQueue = new WinDX12CommandQueue(m_device, D3D12_COMMAND_LIST_TYPE_COPY);

    hr = m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator));
    if(FAILED(hr))
    {
        LOG(Error, "Device : failed to create command allocator !");
        std::string errorMsg = std::system_category().message(hr);
        LOG(Error, errorMsg);
    }

    hr = m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator, nullptr, IID_PPV_ARGS(&m_commandList));
    if(FAILED(hr))
    {
        LOG(Error, "Device : failed to create command list !");
        std::string errorMsg = std::system_category().message(hr);
        LOG(Error, errorMsg);
    }

    hr = m_commandList->Close();
    if(FAILED(hr))
    {
        LOG(Error, "Device : failed to close command list !");
        std::string errorMsg = std::system_category().message(hr);
        LOG(Error, errorMsg);
    }

    // Swap Chain creation
    HWND wnd = (HWND)context;
    
    DXGI_SWAP_CHAIN_DESC  sd = {};
    sd.BufferDesc.Width = width;
    sd.BufferDesc.Height = height;
    sd.BufferDesc.RefreshRate.Numerator = targetRefreshRate;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferDesc.Format = m_swapChainFormat;
    sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    sd.SampleDesc.Count = m_msaaEnabled ? 4 : 1;
    sd.SampleDesc.Quality = m_msaaEnabled ? (m_msaaQualityLevel - 1) : 0;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.BufferCount = m_swapChainBufferCount;
    sd.OutputWindow = wnd;
    sd.Windowed = true;
    sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    hr = m_factory->CreateSwapChain(m_graphicsQueue->GetCommandQueue(), &sd, &m_swapChain);
    if(FAILED(hr))
    {
        LOG(Error, "SwapChain : failed to create swap chain !");
        std::string errorMsg = std::system_category().message(hr);
        LOG(Error, errorMsg);
    }

    // TODO temp, relocate this
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
    rtvHeapDesc.NumDescriptors = m_swapChainBufferCount;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    rtvHeapDesc.NodeMask = 0;
    
    hr = m_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap));
    if(FAILED(hr))
    {
        LOG(Error, "SwapChain : failed to create swap chain !");
        std::string errorMsg = std::system_category().message(hr);
        LOG(Error, errorMsg);
    }

    m_rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    OnResizeWindow(width, height);
}

void LacertaEngine::WinDX12Renderer::LoadShaders()
{
}

void LacertaEngine::WinDX12Renderer::SetBackbufferRenderTargetActive()
{
}

void LacertaEngine::WinDX12Renderer::PresentSwapChain()
{
    // TODO for now placeholder function to debug D3D12 setup 
}

void LacertaEngine::WinDX12Renderer::OnResizeWindow(unsigned width, unsigned height)
{
    m_graphicsQueue->FlushCommandQueue();
    HRESULT hr = m_commandList->Reset(m_commandAllocator, nullptr);
    if(FAILED(hr))
    {
        LOG(Error, "CommandList : failed to Reset");
        std::string errorMsg = std::system_category().message(hr);
        LOG(Error, errorMsg);
    }

    for(int i = 0; i < m_swapChainBufferCount; i++)
        delete m_swapChainBuffer[i];

    m_swapChain->ResizeBuffers(m_swapChainBufferCount, width, height, m_swapChainFormat, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);

    // D3D12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle = m_rtvHeap->GetCPUDescriptorHandleForHeapStart();
    // for (UINT i = 0; i < m_swapChainBufferCount; i++)
    // {
    //     hr = m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_swapChainBuffer[i]));
    //     if(FAILED(hr))
    //     {
    //         LOG(Error, "SwapChain : failed to get buffer");
    //         std::string errorMsg = std::system_category().message(hr);
    //         LOG(Error, errorMsg);
    //     }
    //     
    //     m_device->CreateRenderTargetView(m_swapChainBuffer[i], nullptr, rtvHeapHandle);
    //     rtvHeapHandle.ptr += m_rtvDescriptorSize;
    // }

    m_commandList->Close();
    ID3D12CommandList* cmdLists[] = { m_commandList };
    m_graphicsQueue->GetCommandQueue()->ExecuteCommandLists(1, cmdLists);

    m_graphicsQueue->FlushCommandQueue();
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

void LacertaEngine::WinDX12Renderer::WaitForAllQueuesIdle()
{
    m_graphicsQueue->FlushCommandQueue();
    m_computeQueue->FlushCommandQueue();
    m_copyQueue->FlushCommandQueue();
}
