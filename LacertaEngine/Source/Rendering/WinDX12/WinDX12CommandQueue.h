#pragma once
#include <d3d12.h>
#include <mutex>

#include "../../Core.h"

namespace LacertaEngine
{
    
class WinDX12CommandQueue
{
public:
    WinDX12CommandQueue(ID3D12Device* device, D3D12_COMMAND_LIST_TYPE commandType);
    ~WinDX12CommandQueue();

    bool IsFenceComplete(int64_t fenceValue);
    void InsertWait(int64_t fenceValue);
    void InsertWaitForQueueFence(WinDX12CommandQueue* otherQueue, int64_t fenceValue);
    void InsertWaitForQueue(WinDX12CommandQueue* otherQueue);
 
    void WaitForFenceCPUBlocking(int64_t fenceValue);
    void WaitForIdle() { WaitForFenceCPUBlocking(m_nextFenceValue - 1); }
 
    ID3D12CommandQueue* GetCommandQueue() { return m_commandQueue; }
 
    int64_t PollCurrentFenceValue();
    int64_t GetLastCompletedFence() { return m_lastCompletedFenceValue; }
    int64_t GetNextFenceValue() { return m_nextFenceValue; }
    ID3D12Fence* GetFence() { return m_fence; }
 
    int64_t ExecuteCommandList(ID3D12CommandList* commandList);
 
private:
    ID3D12CommandQueue* m_commandQueue;
    D3D12_COMMAND_LIST_TYPE m_queueType;
 
    std::mutex m_fenceMutex;
    std::mutex m_eventMutex;
 
    ID3D12Fence* m_fence;
    int64_t m_nextFenceValue;
    int64_t m_lastCompletedFenceValue;
    HANDLE m_fenceEventHandle;
};
    
}
