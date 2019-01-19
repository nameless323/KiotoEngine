//
// Copyright (C) Aleksandr Vorontcov 2017.
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#include "stdafx.h"

#include "Render/ConstantBuffer.h"
#include "Render/Renderer.h"

namespace Kioto::Renderer
{

ConstantBuffer::ConstantBuffer(uint16 index, uint16 space)
    : m_index(index), m_space(space), m_key(m_index | m_space << 16)
{
}

ConstantBuffer::ConstantBuffer(const ConstantBuffer& other)
    : m_index(other.m_index)
    , m_space(other.m_space)
    , m_key(other.m_key)
    , m_isDirty(other.m_isDirty)
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

ConstantBuffer::eReturnCode ConstantBuffer::Add(const std::string& name, float32 data, bool queueForUpdate)
{
    uint32 offset = 0;
    Param* pptr = nullptr;
    if (Find(name, offset, pptr))
        return eReturnCode::AlreadyAdded;
    m_dataSize4ByteElem++;

    Param p;
    p.name = name;
    p.Type = eTypeName::v1;
    p.Data = data;
    m_params.push_back(std::move(p));
    m_regenerateMemLayout = true;

    if (queueForUpdate)
        Renderer::QueueConstantBufferForUpdate(*this);

    return eReturnCode::Ok;
}

ConstantBuffer::eReturnCode ConstantBuffer::Add(const std::string& name, const Vector2& data, bool queueForUpdate)
{
    uint32 offset = 0;
    Param* pptr = nullptr;
    if (Find(name, offset, pptr))
        return eReturnCode::AlreadyAdded;
    m_dataSize4ByteElem += 2;

    Param p;
    p.name = name;
    p.Type = eTypeName::v2;
    p.Data = data;
    m_params.push_back(std::move(p));
    m_regenerateMemLayout = true;

    if (queueForUpdate)
        Renderer::QueueConstantBufferForUpdate(*this);

    return eReturnCode::Ok;
}

ConstantBuffer::eReturnCode ConstantBuffer::Add(const std::string& name, const Vector3& data, bool queueForUpdate)
{
    uint32 offset = 0;
    Param* pptr = nullptr;
    if (Find(name, offset, pptr))
        return eReturnCode::AlreadyAdded;
    m_dataSize4ByteElem += 3;

    Param p;
    p.name = name;
    p.Type = eTypeName::v3;
    p.Data = data;
    m_params.push_back(std::move(p));
    m_regenerateMemLayout = true;

    if (queueForUpdate)
        Renderer::QueueConstantBufferForUpdate(*this);

    return eReturnCode::Ok;
}

ConstantBuffer::eReturnCode ConstantBuffer::Add(const std::string& name, const Vector4& data, bool queueForUpdate)
{
    uint32 offset = 0;
    Param* pptr = nullptr;
    if (Find(name, offset, pptr))
        return eReturnCode::AlreadyAdded;
    m_dataSize4ByteElem += 4;

    Param p;
    p.name = name;
    p.Type = eTypeName::v4;
    p.Data = data;
    m_params.push_back(std::move(p));
    m_regenerateMemLayout = true;

    if (queueForUpdate)
        Renderer::QueueConstantBufferForUpdate(*this);

    return eReturnCode::Ok;
}

ConstantBuffer::eReturnCode ConstantBuffer::Add(const std::string& name, const Matrix3& data, bool queueForUpdate)
{
    uint32 offset = 0;
    Param* pptr = nullptr;
    if (Find(name, offset, pptr))
        return eReturnCode::AlreadyAdded;
    m_dataSize4ByteElem += 9;

    Param p;
    p.name = name;
    p.Type = eTypeName::m3;
    p.Data = data;
    m_params.push_back(std::move(p));
    m_regenerateMemLayout = true;

    if (queueForUpdate)
        Renderer::QueueConstantBufferForUpdate(*this);

    return eReturnCode::Ok;
}

ConstantBuffer::eReturnCode ConstantBuffer::Add(const std::string& name, const Matrix4& data, bool queueForUpdate)
{
    uint32 offset = 0;
    Param* pptr = nullptr;
    if (Find(name, offset, pptr))
        return eReturnCode::AlreadyAdded;
    m_dataSize4ByteElem += 16;

    Param p;
    p.name = name;
    p.Type = eTypeName::m4;
    p.Data = data;
    m_params.push_back(std::move(p));
    m_regenerateMemLayout = true;

    if (queueForUpdate)
        Renderer::QueueConstantBufferForUpdate(*this);

    return eReturnCode::Ok;
}

ConstantBuffer::eReturnCode ConstantBuffer::Set(const std::string& name, float32 data, bool queueForUpdate)
{
    uint32 offset = 0;
    Param* p = nullptr;
    if (!Find(name, offset, p))
        return eReturnCode::NotFound;
    if (p->Type != eTypeName::v1)
        return eReturnCode::TypesDontMatch;
    p->Data = data;

    if (!m_regenerateMemLayout)
        m_memData[offset] = data;

    if (queueForUpdate)
        Renderer::QueueConstantBufferForUpdate(*this);

    return eReturnCode::Ok;
}

ConstantBuffer::eReturnCode ConstantBuffer::Set(const std::string& name, const Vector2& data, bool queueForUpdate)
{
    uint32 offset = 0;
    Param* p = nullptr;
    if (!Find(name, offset, p))
        return eReturnCode::NotFound;
    if (p->Type != eTypeName::v2)
        return eReturnCode::TypesDontMatch;
    p->Data = data;

    if (!m_regenerateMemLayout)
    {
        m_memData[offset++] = data.x;
        m_memData[offset++] = data.y;
    }

    if (queueForUpdate)
        Renderer::QueueConstantBufferForUpdate(*this);

    return eReturnCode::Ok;
}

ConstantBuffer::eReturnCode ConstantBuffer::Set(const std::string& name, const Vector3& data, bool queueForUpdate)
{
    uint32 offset = 0;
    Param* p = nullptr;
    if (!Find(name, offset, p))
        return eReturnCode::NotFound;
    if (p->Type != eTypeName::v3)
        return eReturnCode::TypesDontMatch;
    p->Data = data;

    if (!m_regenerateMemLayout)
    {
        m_memData[offset++] = data.x;
        m_memData[offset++] = data.y;
        m_memData[offset++] = data.z;
    }

    if (queueForUpdate)
        Renderer::QueueConstantBufferForUpdate(*this);

    return eReturnCode::Ok;
}

ConstantBuffer::eReturnCode ConstantBuffer::Set(const std::string& name, const Vector4& data, bool queueForUpdate)
{
    uint32 offset = 0;
    Param* p = nullptr;
    if (!Find(name, offset, p))
        return eReturnCode::NotFound;
    if (p->Type != eTypeName::v4)
        return eReturnCode::TypesDontMatch;
    p->Data = data;

    if (!m_regenerateMemLayout)
    {
        m_memData[offset++] = data.x;
        m_memData[offset++] = data.y;
        m_memData[offset++] = data.z;
        m_memData[offset++] = data.w;
    }

    if (queueForUpdate)
        Renderer::QueueConstantBufferForUpdate(*this);

    return eReturnCode::Ok;
}

ConstantBuffer::eReturnCode ConstantBuffer::Set(const std::string& name, const Matrix3& data, bool queueForUpdate)
{
    uint32 offset = 0;
    Param* p = nullptr;
    if (!Find(name, offset, p))
        return eReturnCode::NotFound;
    if (p->Type != eTypeName::m4)
        return eReturnCode::TypesDontMatch;
    p->Data = data;

    if (!m_regenerateMemLayout)
        memcpy(m_memData + offset, data.data, 9 * sizeof(float32));

    if (queueForUpdate)
        Renderer::QueueConstantBufferForUpdate(*this);

    return eReturnCode::Ok;
}

ConstantBuffer::eReturnCode ConstantBuffer::Set(const std::string& name, const Matrix4& data, bool queueForUpdate)
{
    uint32 offset = 0;
    Param* p = nullptr;
    if (!Find(name, offset, p))
        return eReturnCode::NotFound;
    if (p->Type != eTypeName::m4)
        return eReturnCode::TypesDontMatch;
    p->Data = data;

    if (!m_regenerateMemLayout)
        memcpy(m_memData + offset, data.data, 16 * sizeof(float32));

    if (queueForUpdate)
        Renderer::QueueConstantBufferForUpdate(*this);

    return eReturnCode::Ok;
}

ConstantBuffer::~ConstantBuffer()
{
    SafeDeleteArray(m_memData);
}

void ConstantBuffer::ComposeBufferData()
{
    if (!m_regenerateMemLayout)
        return;

    m_regenerateMemLayout = false;
    SafeDeleteArray(m_memData);
    m_dataSize = m_dataSize4ByteElem * sizeof(float32);
    m_memData = new float32[m_dataSize4ByteElem];
    float32* ptr = m_memData;
    for (const auto& param : m_params)
    {
        if (param.Type == eTypeName::v1)
        {
            *ptr++ = std::get<float32>(param.Data);
        }
        else if (param.Type == eTypeName::v2)
        {
            Vector2 d = std::get<Vector2>(param.Data);
            *ptr++ = d.x;
            *ptr++ = d.y;
        }
        else if (param.Type == eTypeName::v3)
        {
            Vector3 d = std::get<Vector3>(param.Data);
            *ptr++ = d.x;
            *ptr++ = d.y;
            *ptr++ = d.z;
        }
        else if (param.Type == eTypeName::v4)
        {
            Vector4 d = std::get<Vector4>(param.Data);
            *ptr++ = d.x;
            *ptr++ = d.y;
            *ptr++ = d.z;
            *ptr++ = d.w;
        }
        else if (param.Type == eTypeName::m3)
        {
            Matrix3 d = std::get<Matrix3>(param.Data);
            memcpy(ptr, d.data, 9 * sizeof(float32));
            ptr += 9;
        }
        else if (param.Type == eTypeName::m4)
        {
            Matrix4 d = std::get<Matrix4>(param.Data);
            memcpy(ptr, d.data, 16 * sizeof(float32));
            ptr += 16;
        }
    }
}

bool ConstantBuffer::Find(const std::string& name, uint32& offsetInData, Param*& resParam)
{
    offsetInData = 0;
    for (uint32 i = 0; i < m_params.size(); ++i)
    {
        if (m_params[i].name == name)
        {
            resParam = &m_params[i];
            return true;
        }
        offsetInData += static_cast<byte>(m_params[i].Type);
    }
    return false;
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
    target.m_isDirty = true;
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