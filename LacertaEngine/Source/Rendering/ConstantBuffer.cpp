#include "ConstantBuffer.h"

#include "Drawcall.h"
#include "Renderer.h"
#include "ShadowMapPassLayouts.h"
#include "SkyBoxPassLayouts.h"

namespace LacertaEngine
{
    
ConstantBuffer::ConstantBuffer(): m_data(nullptr), m_cbufferType()
{
}

ConstantBuffer::ConstantBuffer(void* data, ConstantBufferType cbufferType)
    : m_data(data), m_cbufferType(cbufferType)
{
}

ConstantBuffer::~ConstantBuffer()
{
    switch (m_cbufferType)
    {
    case ConstantBufferType::MeshCbuf:
        delete static_cast<SceneMeshConstantBuffer*>(m_data);
        break;

    case ConstantBufferType::SceneCbuf:
        delete static_cast<SceneConstantBuffer*>(m_data);
        break;

    case ConstantBufferType::SkyBoxCbuf:
        delete static_cast<SkyBoxConstantBuffer*>(m_data);
        break;

    case ConstantBufferType::SMLightCubf:
        delete static_cast<ShadowMapLightConstantBuffer*>(m_data);
        break;
    }
}

void ConstantBuffer::SetData(void* data, ConstantBufferType cbufferType)
{
    m_data = data;
    m_cbufferType = cbufferType;
}

void ConstantBuffer::Bind(Renderer* renderer)
{
    renderer->UpdateConstantBuffer(m_data, m_cbufferType);
}
    
}
