#pragma once
#include "../../Core.h"
#include <d3d12.h>
#include <vector>

namespace LacertaEngine
{
    
class WinDX12Renderer;
    
struct DescriptorHandle
{
    D3D12_CPU_DESCRIPTOR_HANDLE CPU;
    D3D12_GPU_DESCRIPTOR_HANDLE GPU;

    uint32_t HeapIdx;

    bool IsValid() { return CPU.ptr != 0; }
    bool IsShaderVisible() { return GPU.ptr != 0; }
};

class LACERTAENGINE_API WinDX12DescriptorHeap
{
public:
    WinDX12DescriptorHeap(WinDX12Renderer* renderer, D3D12_DESCRIPTOR_HEAP_TYPE type, int size);
    ~WinDX12DescriptorHeap();

    DescriptorHandle Allocate();
    void Free(DescriptorHandle& handle);
    ID3D12DescriptorHeap* GetHeap() { return m_heap; }

private:
    ID3D12DescriptorHeap* m_heap;
    D3D12_DESCRIPTOR_HEAP_TYPE m_type;
    bool m_isShaderVisible;
    int m_incrementSize;
    int m_heapSize;

    std::vector<bool> m_handlesTable;
};
    
}