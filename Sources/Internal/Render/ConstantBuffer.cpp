#include "stdafx.h"

#include "Render/ConstantBuffer.h"
#include "Render/Renderer.h"

namespace Kioto::Renderer
{

ConstantBuffer::ConstantBuffer(const ConstantBuffer& other)
    : m_index(other.m_index)
    , m_space(other.m_space)
    , m_key(other.m_key)
    , m_regenerateMemLayout(other.m_regenerateMemLayout)
    , m_dataSize(other.m_dataSize)
    , m_dataSize4ByteElem(other.m_dataSize4ByteElem)
    , m_params(other.m_params)
{
    memcpy(m_memData, other.m_memData, m_dataSize);
}

ConstantBuffer::ConstantBuffer(ConstantBuffer&& other)
{
    swap(*this, other);
}

ConstantBuffer& ConstantBuffer::operator=(ConstantBuffer other)
{
    swap(*this, other);
    return *this;
}

void ConstantBuffer::MakeShallowCopy(ConstantBuffer& target, bool queueForUpdate) const
{
    target.m_index = m_index;
    target.m_space = m_space;
    target.m_key = m_key;
    target.m_regenerateMemLayout = true;

    for (auto& param : m_params)
    {
        if (param.Type == eTypeName::v1)
        {
            target.Add(param.name, std::get<float32>(param.Data), queueForUpdate);
        }
        else if (param.Type == eTypeName::v2)
        {
            target.Add(param.name, std::get<Vector2>(param.Data), queueForUpdate);
        }
        else if (param.Type == eTypeName::v3)
        {
            target.Add(param.name, std::get<Vector3>(param.Data), queueForUpdate);
        }
        else if (param.Type == eTypeName::v4)
        {
            target.Add(param.name, std::get<Vector4>(param.Data), queueForUpdate);
        }
        else if (param.Type == eTypeName::m3)
        {
            target.Add(param.name, std::get<Matrix3>(param.Data), queueForUpdate);
        }
        else
        {
            target.Add(param.name, std::get<Matrix4>(param.Data), queueForUpdate);
        }
    }
}

void ConstantBuffer::ScheduleToUpdate()
{
    Renderer::QueueConstantBufferForUpdate(*this);
}
}