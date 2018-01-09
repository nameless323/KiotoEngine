//
// Copyright (C) Alexandr Vorontsov 2017.
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#include "stdafx.h"

#include "Render/ConstantBuffer.h"

namespace Kioto::Renderer
{

ConstantBuffer::ConstantBuffer(uint16 index, uint16 space)
    : m_index(index), m_space(space)
{
}

ConstantBuffer::eReturnCode ConstantBuffer::Add(std::string& name, float32 data)
{
    uint32 offset = 0;
    Param* p = nullptr;
    if (Find(name, offset, p))
        return eReturnCode::AlreadyAdded;
    m_params.push_back({ name, Vector4(data, 0.0f, 0.0f, 0.0f), eTypeName::v1 });
    m_regenerateMemLayout = true;
    return eReturnCode::Ok;
}

ConstantBuffer::eReturnCode ConstantBuffer::Add(std::string& name, const Vector2& data)
{
    uint32 offset = 0;
    Param* p = nullptr;
    if (Find(name, offset, p))
        return eReturnCode::AlreadyAdded;
    m_params.push_back({ name, Vector4(data.x, data.y, 0.0f, 0.0f), eTypeName::v2 });
    m_regenerateMemLayout = true;
    return eReturnCode::Ok;
}

ConstantBuffer::eReturnCode ConstantBuffer::Add(std::string& name, const Vector3& data)
{
    uint32 offset = 0;
    Param* p = nullptr;
    if (Find(name, offset, p))
        return eReturnCode::AlreadyAdded;
    m_params.push_back({ name, Vector4(data.x, data.y, data.z, 0.0f), eTypeName::v3 });
    m_regenerateMemLayout = true;
    return eReturnCode::Ok;
}

ConstantBuffer::eReturnCode ConstantBuffer::Add(std::string& name, const Vector4& data)
{
    uint32 offset = 0;
    Param* p = nullptr;
    if (Find(name, offset, p))
        return eReturnCode::AlreadyAdded;
    m_params.push_back({ name, data, eTypeName::v4 });
    m_regenerateMemLayout = true;
    return eReturnCode::Ok;
}

ConstantBuffer::eReturnCode ConstantBuffer::Set(std::string& name, float32 data)
{
    uint32 offset = 0;
    Param* p = nullptr;
    if (!Find(name, offset, p))
        return eReturnCode::NotFound;
    if (p->Type != eTypeName::v1)
        return eReturnCode::TypesDontMatch;
    p->Data.x = data;

    if (!m_regenerateMemLayout)
        m_memData[offset] = data;

    return eReturnCode::Ok;
}

ConstantBuffer::eReturnCode ConstantBuffer::Set(std::string& name, const Vector2& data)
{
    uint32 offset = 0;
    Param* p = nullptr;
    if (!Find(name, offset, p))
        return eReturnCode::NotFound;
    if (p->Type != eTypeName::v2)
        return eReturnCode::TypesDontMatch;
    p->Data.x = data.x;
    p->Data.y = data.y;

    if (!m_regenerateMemLayout)
    {
        m_memData[offset++] = data.x;
        m_memData[offset++] = data.y;
    }

    return eReturnCode::Ok;
}

ConstantBuffer::eReturnCode ConstantBuffer::Set(std::string& name, const Vector3& data)
{
    uint32 offset = 0;
    Param* p = nullptr;
    if (!Find(name, offset, p))
        return eReturnCode::NotFound;
    if (p->Type != eTypeName::v3)
        return eReturnCode::TypesDontMatch;
    p->Data.x = data.x;
    p->Data.y = data.y;
    p->Data.z = data.z;

    if (!m_regenerateMemLayout)
    {
        m_memData[offset++] = data.x;
        m_memData[offset++] = data.y;
        m_memData[offset++] = data.z;
    }

    return eReturnCode::Ok;
}

ConstantBuffer::eReturnCode ConstantBuffer::Set(std::string& name, const Vector4& data)
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
        m_memData[offset++] = data.w;
    }

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

}

bool ConstantBuffer::Find(const std::string& name, uint32& offsetInData, Param* resParam)
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

}