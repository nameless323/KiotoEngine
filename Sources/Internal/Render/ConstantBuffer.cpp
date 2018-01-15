//
// Copyright (C) Alexandr Vorontsov 2017.
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#include "stdafx.h"

#include "Render/ConstantBuffer.h"

namespace Kioto::Renderer
{

ConstantBuffer::ConstantBuffer(uint16 index, uint16 space)
    : m_index(index), m_space(space), m_key(m_index | m_space << 16)
{
}

ConstantBuffer::eReturnCode ConstantBuffer::Add(std::string& name, float32 data)
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
    return eReturnCode::Ok;
}

ConstantBuffer::eReturnCode ConstantBuffer::Add(std::string& name, const Vector2& data)
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
    return eReturnCode::Ok;
}

ConstantBuffer::eReturnCode ConstantBuffer::Add(std::string& name, const Vector3& data)
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
    return eReturnCode::Ok;
}

ConstantBuffer::eReturnCode ConstantBuffer::Add(std::string& name, const Vector4& data)
{
    uint32 offset = 0;
    Param* pptr = nullptr;
    if (Find(name, offset, pptr))
        return eReturnCode::AlreadyAdded;
    m_dataSize4ByteElem += 4;

    Param p;
    p.name = name;
    p.Type = eTypeName::v3;
    p.Data = data;
    m_params.push_back(std::move(p));
    m_regenerateMemLayout = true;
    return eReturnCode::Ok;
}

ConstantBuffer::eReturnCode ConstantBuffer::Add(std::string& name, const Matrix3& data)
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
    return eReturnCode::Ok;
}

ConstantBuffer::eReturnCode ConstantBuffer::Add(std::string& name, const Matrix4& data)
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
    p->Data = data;

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
    p->Data = data;

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
    p->Data = data;

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

ConstantBuffer::eReturnCode ConstantBuffer::Set(std::string& name, const Matrix3& data)
{
    uint32 offset = 0;
    Param* p = nullptr;
    if (!Find(name, offset, p))
        return eReturnCode::NotFound;
    if (p->Type != eTypeName::m4)
        return eReturnCode::TypesDontMatch;
    p->Data = data;

    if (!m_regenerateMemLayout)
        memcpy(m_memData + offset, data.data, sizeof(9 * sizeof(float32)));

    return eReturnCode::Ok;
}

ConstantBuffer::eReturnCode ConstantBuffer::Set(std::string& name, const Matrix4& data)
{
    uint32 offset = 0;
    Param* p = nullptr;
    if (!Find(name, offset, p))
        return eReturnCode::NotFound;
    if (p->Type != eTypeName::m4)
        return eReturnCode::TypesDontMatch;
    p->Data = data;

    if (!m_regenerateMemLayout)
        memcpy(m_memData + offset, data.data, sizeof(16 * sizeof(float32)));

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
            memcpy(ptr, d.data, sizeof(9 * sizeof(float32)));
            ptr += 9;
        }
        else if (param.Type == eTypeName::m4)
        {
            Matrix4 d = std::get<Matrix4>(param.Data);
            memcpy(ptr, d.data, sizeof(16 * sizeof(float32)));
            ptr += 16;
        }
    }
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