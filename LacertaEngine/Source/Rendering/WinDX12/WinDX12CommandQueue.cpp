#include "WinDX12CommandQueue.h"

#include "../../Logger/Logger.h"

namespace LacertaEngine
{
    
WinDX12CommandQueue::WinDX12CommandQueue(ID3D12Device* device, D3D12_COMMAND_LIST_TYPE commandType)
{
    m_queueType = commandType;
 
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Type = m_queueType;
    queueDesc.NodeMask = 0;
    HRESULT hr = device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue));
    if(FAILED(hr))
    {
        LOG(Error, "Command Queue : failed to create command queue !");
        std::string errorMsg = std::system_category().message(hr);
        LOG(Error, errorMsg);
    }
 
    hr = device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence));
    if(FAILED(hr))
    {
        LOG(Error, "Command Queue : failed to create fence !");
        std::string errorMsg = std::system_category().message(hr);
        LOG(Error, errorMsg);
    }
}

WinDX12CommandQueue::~WinDX12CommandQueue()
{
    m_fence->Release();
    m_fence = NULL;
 
    m_commandQueue->Release();
    m_commandQueue = NULL;
}

void WinDX12CommandQueue::FlushCommandQueue()
{
    // Advance the fence value to mark commands up to this fence point.
    m_fenceValue++;

    // Add an instruction to the command queue to set a new fence point.  Because we 
    // are on the GPU timeline, the new fence point won't be set until the GPU finishes
    // processing all the commands prior to this Signal().
    HRESULT hr = m_commandQueue->Signal(m_fence, m_fenceValue);
    if(FAILED(hr))
    {
        LOG(Error, "Command Queue : failed to signal !");
        std::string errorMsg = std::system_category().message(hr);
        LOG(Error, errorMsg);
    }

    // Wait until the GPU has completed commands up to this fence point.
    if(m_fence->GetCompletedValue() < m_fenceValue)
    {
        HANDLE eventHandle = CreateEvent(nullptr, false, false, nullptr);

        // Fire event when GPU hits current fence.  
        hr = m_fence->SetEventOnCompletion(m_fenceValue, eventHandle);
        if(FAILED(hr))
        {
            LOG(Error, "Command Queue : failed to set event on completion !");
            std::string errorMsg = std::system_category().message(hr);
            LOG(Error, errorMsg);
        }

        // Wait until the GPU hits current fence event is fired.
        WaitForSingleObject(eventHandle, INFINITE);
        CloseHandle(eventHandle);
    }
}

}
