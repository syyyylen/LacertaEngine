﻿#include "ConstantBuffer.h"

#include "Drawcall.h"
#include "Renderer.h"

namespace LacertaEngine
{
    
ConstantBuffer::ConstantBuffer(void* data, ConstantBufferType cbufferType)
    : m_data(data), m_cbufferType(cbufferType)
{
}

ConstantBuffer::~ConstantBuffer()
{
    switch (m_cbufferType)
    {
    case ConstantBufferType::MeshCbuf:
        delete static_cast<MeshConstantBuffer*>(m_data);
        break;

    case ConstantBufferType::SceneCbuf:
        delete static_cast<SceneConstantBuffer*>(m_data);
        break;
    }
}

void ConstantBuffer::Bind(Renderer* renderer)
{
    renderer->UpdateConstantBuffer(m_data, m_cbufferType);
}
    
}