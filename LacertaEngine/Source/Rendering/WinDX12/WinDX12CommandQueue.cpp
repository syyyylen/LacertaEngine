#include "WinDX12CommandQueue.h"

#include "../../Logger/Logger.h"

namespace LacertaEngine
{
    
WinDX12CommandQueue::WinDX12CommandQueue(ID3D12Device* device, D3D12_COMMAND_LIST_TYPE commandType)
{
    m_queueType = commandType;
    m_commandQueue = NULL;
    m_fence = NULL;
    m_nextFenceValue = ((int64_t)m_queueType << 56) + 1;
    m_lastCompletedFenceValue = ((int64_t)m_queueType << 56);
 
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
 
    m_fence->Signal(m_lastCompletedFenceValue);
 
    m_fenceEventHandle = CreateEvent(nullptr, false, false, nullptr);
    if(m_fenceEventHandle == INVALID_HANDLE_VALUE)
        LOG(Error, "Invalid Fence Event");
}

WinDX12CommandQueue::~WinDX12CommandQueue()
{
    CloseHandle(m_fenceEventHandle);
 
    m_fence->Release();
    m_fence = NULL;
 
    m_commandQueue->Release();
    m_commandQueue = NULL;
}

bool WinDX12CommandQueue::IsFenceComplete(int64_t fenceValue)
{
    if(fenceValue > m_lastCompletedFenceValue)
        PollCurrentFenceValue();
 
    return fenceValue <= m_lastCompletedFenceValue;
}

void WinDX12CommandQueue::InsertWait(int64_t fenceValue)
{
    m_commandQueue->Wait(m_fence, fenceValue);
}

void WinDX12CommandQueue::InsertWaitForQueueFence(WinDX12CommandQueue* otherQueue, int64_t fenceValue)
{
    m_commandQueue->Wait(otherQueue->GetFence(), fenceValue);
}

void WinDX12CommandQueue::InsertWaitForQueue(WinDX12CommandQueue* otherQueue)
{
    m_commandQueue->Wait(otherQueue->GetFence(), otherQueue->GetNextFenceValue() - 1);
}

void WinDX12CommandQueue::WaitForFenceCPUBlocking(int64_t fenceValue)
{
    if (IsFenceComplete(fenceValue))
        return;
 
    {
        std::lock_guard<std::mutex> lockGuard(m_eventMutex);
 
        m_fence->SetEventOnCompletion(fenceValue, m_fenceEventHandle);
        WaitForSingleObjectEx(m_fenceEventHandle, INFINITE, false);
        m_lastCompletedFenceValue = fenceValue;
    }
}

int64_t WinDX12CommandQueue::PollCurrentFenceValue()
{
    m_lastCompletedFenceValue = std::max(m_lastCompletedFenceValue, (int64_t)m_fence->GetCompletedValue());
    return m_lastCompletedFenceValue;
}

int64_t WinDX12CommandQueue::ExecuteCommandList(ID3D12CommandList* commandList)
{
    HRESULT hr = ((ID3D12GraphicsCommandList*)commandList)->Close();
    if(FAILED(hr))
    {
        LOG(Error, "Command Queue : failed to execute command list !");
        std::string errorMsg = std::system_category().message(hr);
        LOG(Error, errorMsg);
    }
    
    m_commandQueue->ExecuteCommandLists(1, &commandList);
 
    std::lock_guard<std::mutex> lockGuard(m_fenceMutex);
    
    m_commandQueue->Signal(m_fence, m_nextFenceValue);
 
    return m_nextFenceValue++;
}
    
}
