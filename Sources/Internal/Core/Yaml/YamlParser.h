// [a_vorontcov] TODO:!!!!!!!!!!!!!!!!! rethink. it won't work for a binary case. need one way to serialize/deserialize

#pragma once

#include <yaml-cpp/yaml.h>

#include "Math/Matrix4.h"
#include "Math/Vector3.h"
#include "Math/Quaternion.h"

inline YAML::Emitter& operator << (YAML::Emitter& out, const Kioto::Matrix4& m)
{
    out << YAML::Flow;
    out << YAML::BeginSeq;
    for (Kioto::uint32 i = 0; i < 16; ++i)
        out << m.data[i];
    out << YAML::EndSeq;
    return out;
}

inline YAML::Emitter& operator << (YAML::Emitter& out, const Kioto::Vector3& v)
{
    out << YAML::Flow;
    out << YAML::BeginSeq;
    for (Kioto::uint32 i = 0; i < 3; ++i)
        out << v.data[i];
    out << YAML::EndSeq;
    return out;
}

inline YAML::Emitter& operator << (YAML::Emitter& out, const Kioto::Quaternion& q)
{
    out << YAML::Flow;
    out << YAML::BeginSeq;
    for (Kioto::uint32 i = 0; i < 4; ++i)
        out << q.data[i];
    out << YAML::EndSeq;
    return out;
}

namespace YAML
{
template<>
struct convert<Kioto::Vector3> 
{
    static Node encode(const Kioto::Vector3& v) 
    {
        Node node;
        for (Kioto::uint32 i = 0; i < 3; ++i)
            node.push_back(v.data[i]);
        return node;
    }

    static bool decode(const Node& node, Kioto::Vector3& v)
    {
        if (!node.IsSequence() || node.size() != 3) 
        {
            return false;
        }
        for (Kioto::uint32 i = 0; i < 3; ++i)
            v.data[i] = node[i].as<float>();
        return true;
    }
};

template<>
struct convert<Kioto::Matrix4>
{
    static Node encode(const Kioto::Matrix4& m)
    {
        Node node;
        for (Kioto::uint32 i = 0; i < 16; ++i)
            node.push_back(m.data[i]);
        return node;
    }

    static bool decode(const Node& node, Kioto::Matrix4& m)
    {
        if (!node.IsSequence() || node.size() != 16)
        {
            return false;
        }
        for (Kioto::uint32 i = 0; i < 16; ++i)
            m.data[i] = node[i].as<float>();
        return true;
    }
};

template<>
struct convert<Kioto::Quaternion>
{
    static Node encode(const Kioto::Quaternion& q)
    {
        Node node;
        for (Kioto::uint32 i = 0; i < 4; ++i)
            node.push_back(q.data[i]);
        return node;
    }

    static bool decode(const Node& node, Kioto::Quaternion& q)
    {
        if (!node.IsSequence() || node.size() != 4)
        {
            return false;
        }
        for (Kioto::uint32 i = 0; i < 4; ++i)
            q.data[i] = node[i].as<float>();
        return true;
    }
};
}