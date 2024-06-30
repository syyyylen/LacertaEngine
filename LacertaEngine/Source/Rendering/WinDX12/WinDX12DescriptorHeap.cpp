#include "WinDX12DescriptorHeap.h"

#include <string>

#include "WinDX12Renderer.h"
#include "../../Logger/Logger.h"

LacertaEngine::WinDX12DescriptorHeap::WinDX12DescriptorHeap(WinDX12Renderer* renderer, D3D12_DESCRIPTOR_HEAP_TYPE type, int size)
: m_type(type), m_heapSize(size)
{
    m_handlesTable = std::vector<bool>(size, false);
    
    D3D12_DESCRIPTOR_HEAP_DESC desc = {};
    desc.Type = type;
    desc.NumDescriptors = m_heapSize;
    desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    if (type == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV || type == D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER)
    {
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        m_isShaderVisible = true;
    }

    HRESULT hr = renderer->GetDevice()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_heap));
    if(FAILED(hr))
    {
        LOG(Error, "DescriptorHeap : failed to create descriptor heap !");
        std::string errorMsg = std::system_category().message(hr);
        LOG(Error, errorMsg);
    }

    m_incrementSize = renderer->GetDevice()->GetDescriptorHandleIncrementSize(type);

    LOG(Debug, "DescriptorHeap : allocated a descriptor heap of size : " + std::to_string(size));
}

LacertaEngine::WinDX12DescriptorHeap::~WinDX12DescriptorHeap()
{
    m_handlesTable.clear();
    m_heap->Release();
}

LacertaEngine::DescriptorHandle LacertaEngine::WinDX12DescriptorHeap::Allocate()
{
    int index = -1;
    for(int i = 0; i < m_heapSize; i++)
    {
        if(m_handlesTable[i] == false)
        {
            m_handlesTable[i] = true;
            index = i;
            break;
        }
    }

    if(index == -1)
    {
        LOG(Error, "DescriptorHeap : failed to create descriptor handle !");
        return DescriptorHandle();
    }

    DescriptorHandle descriptorHandle = {};
    descriptorHandle.HeapIdx = index;
    descriptorHandle.CPU = m_heap->GetCPUDescriptorHandleForHeapStart();
    descriptorHandle.CPU.ptr += index * m_incrementSize;

    if (m_type == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV || m_type == D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER)
    {
        descriptorHandle.GPU = m_heap->GetGPUDescriptorHandleForHeapStart();
        descriptorHandle.GPU.ptr += index * m_incrementSize;
    }

    return descriptorHandle;
}

void LacertaEngine::WinDX12DescriptorHeap::Free(DescriptorHandle& handle)
{
    if(!handle.IsValid())
        return;

    m_handlesTable[handle.HeapIdx] = false;
    handle.CPU.ptr = 0;
}
