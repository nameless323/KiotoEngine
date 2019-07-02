//
// Copyright (C) Aleksandr Vorontcov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include <yaml-cpp/yaml.h>

#include "Math/Matrix4.h"

inline YAML::Emitter& operator << (YAML::Emitter& out, const Kioto::Matrix4& m)
{
    out << YAML::Flow;
    out << YAML::BeginSeq;
    for (Kioto::uint32 i = 0; i < 16; ++i)
        out << m.data[i];
    out << YAML::EndSeq;
    return out;
}