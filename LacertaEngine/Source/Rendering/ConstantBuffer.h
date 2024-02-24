#pragma once
#include "Bindable.h"
#include "../Core.h"

namespace LacertaEngine
{

enum ConstantBufferType
{
    SceneCbuf,
    MeshCbuf,
    SkyBoxCbuf
};

    
class LACERTAENGINE_API ConstantBuffer : public Bindable
{
public:
    ConstantBuffer();
    ConstantBuffer(void* data, ConstantBufferType cbufferType);
    ~ConstantBuffer();

    void SetData(void* data, ConstantBufferType cbufferType);
    void Bind(Renderer* renderer) override;

private:
    void* m_data; // we have ownership of this data
    ConstantBufferType m_cbufferType;
};
    
}