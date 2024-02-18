#pragma once
#include "Bindable.h"
#include "../Core.h"

namespace LacertaEngine
{

enum ConstantBufferType;
    
class LACERTAENGINE_API ConstantBuffer : public Bindable
{
public:
    ConstantBuffer(void* data, ConstantBufferType cbufferType);
    ~ConstantBuffer();

    void Bind(Renderer* renderer) override;

private:
    void* m_data; // we have ownership of this data
    ConstantBufferType m_cbufferType;
};
    
}