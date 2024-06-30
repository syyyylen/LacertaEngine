#pragma once
#include <d3d12.h>
#include <mutex>

#include "../../Core.h"

namespace LacertaEngine
{
    
class LACERTAENGINE_API WinDX12CommandQueue
{
public:
    WinDX12CommandQueue(ID3D12Device* device, D3D12_COMMAND_LIST_TYPE commandType);
    ~WinDX12CommandQueue();

    ID3D12CommandQueue* GetCommandQueue() { return m_commandQueue; }
 
    int64_t GetFenceValue() { return m_fenceValue; }
    ID3D12Fence* GetFence() { return m_fence; }
 
    void FlushCommandQueue();
 
private:
    ID3D12CommandQueue* m_commandQueue;
    D3D12_COMMAND_LIST_TYPE m_queueType;
    ID3D12Fence* m_fence;
    int64_t m_fenceValue;
};
    
}
