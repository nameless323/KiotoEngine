//
// Copyright (C) Alexandr Vorontsov 2017.
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include "Core/CoreTypes.h"

#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"

#include <vector>

namespace Kioto::Renderer
{
class ConstantBuffer
{
public:
    enum class eReturnCode
    {
        Ok,
        AlreadyAdded,
        NotFound,
        TypesDontMatch
    };

    ConstantBuffer(uint16 index, uint16 space);
    ConstantBuffer(const ConstantBuffer&) = delete;
    ~ConstantBuffer();

    ConstantBuffer& operator= (const ConstantBuffer&) = delete;

    eReturnCode Add(std::string& name, float32 data);
    eReturnCode Add(std::string& name, const Vector2& data);
    eReturnCode Add(std::string& name, const Vector3& data);
    eReturnCode Add(std::string& name, const Vector4& data);


    eReturnCode Set(std::string& name, float32 data);
    eReturnCode Set(std::string& name, const Vector2& data);
    eReturnCode Set(std::string& name, const Vector3& data);
    eReturnCode Set(std::string& name, const Vector4& data);

    void ComposeBufferData();

private:
    enum class eTypeName : byte
    {
        v1 = 1,
        v2 = 2,
        v3 = 3,
        v4 = 4
    };
    struct Param
    {
        std::string name;
        Vector4 Data;
        eTypeName Type;
    };

    bool Find(const std::string& name, uint32& offsetInData, Param* resParam);

    uint16 m_index = 0;
    uint16 m_space = 0;
    uint32 m_key = 0;
    bool m_isDirty = true;
    bool m_regenerateMemLayout = true;

    std::vector<Param> m_params;
    float32* m_memData = nullptr;
    uint32 m_dataSize = 0;
    uint32 m_dataSize4ByteElem = 0;
};
}