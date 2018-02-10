//
// Copyright (C) Alexandr Vorontsov. 2018
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#include "stdafx.h"

#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Math/Matrix3.h"
#include "Math/Matrix4.h"
#include "Render/DX12/Shader/ShaderParser.h"

#include "AssetsSystem/AssetsSystem.h"

namespace Kioto::Renderer
{
namespace ShaderParser // [a_vorontsov] Real parser via AST tree too time consuming for now.
{

std::vector<std::string> m_preprocessedHeaders;
static constexpr uint16 m_maxDepth = 128;
static constexpr uint8 m_floatLen = 5;

std::string UnfoldIncludes(std::string& source, uint16 recursionDepth)
{
    if (recursionDepth > m_maxDepth)
    {
        throw "Too deep shader includes.";
        return "";
    }
    size_t includePos = source.find("#include", 0);
    size_t includeFin = source.find("\n", includePos);
    while (includePos != std::string::npos)
    {
        if (includeFin != std::string::npos)
        {
            std::string s = source.substr(includePos, includeFin - includePos);
            source.erase(includePos, includeFin - includePos);

            std::string relativeIncludePath;
            bool startFound = false;
            for (int i = 8; i < s.size(); ++i)
            {
                if (s[i] == '\"' && !startFound)
                {
                    startFound = true;
                    continue;
                }
                else if (!startFound)
                    continue;
                else if (s[i] == '\"')
                    break;
                relativeIncludePath += s[i];
            }
            relativeIncludePath.insert(0, "Shaders\\");

            auto it = std::find(m_preprocessedHeaders.begin(), m_preprocessedHeaders.end(), relativeIncludePath);
            if (it == m_preprocessedHeaders.end())
            {
                std::string shaderPath = AssetsSystem::GetAssetFullPath(relativeIncludePath);
                bool isExist = AssetsSystem::CheckIfFileExist(shaderPath);
                std::string incl = AssetsSystem::ReadFileAsString(shaderPath);
                source.insert(includePos, UnfoldIncludes(incl, recursionDepth + 1));
                m_preprocessedHeaders.push_back(relativeIncludePath);
            }
        }

        includePos = source.find("#include", includePos);
        includeFin = source.find("\n", includePos);
    }

    return source;
}

void TrimLineComments(std::string& source)
{
    size_t pos = source.find("//");
    while (pos != std::string::npos)
    {
        size_t i = pos;
        for (; i < source.length(); ++i)
        {
            if (source[i] == '\n')
            {
                source.erase(pos, i - pos);
                break;
            }
        }
        if (i == source.length())
            source.erase(pos, i - pos);
        pos = source.find("//", pos);
    }
}

void TrimMultilineComments(std::string& source)
{
    size_t open = source.find("/*");
    size_t close = source.find("*/");
    while (open != std::string::npos || close != std::string::npos)
    {
        if (close < open)
            return;
        source.erase(open, close + 2 - open);
        open = source.find("/*", open);
        close = source.find("*/", open);
    }
}

bool IsEmptyChar(char c)
{
    return c == '\n' || c == ' ' || c == '\r\n';
}

uint8 CharToInt(char c)
{
    return c - '0';
}

bool TryParseSemantics(const std::string& source, size_t pos, eVertexSemantic& semantic, uint8& semanticIndex, uint8& semStringLen)
{
    if (source.substr(pos, 8) == "POSITION")
    {
        semantic = eVertexSemantic::Position;
        if (isdigit(source[pos + 8]))
        {
            semanticIndex = CharToInt(source[pos + 8]);
            semStringLen = 9;
            return true;
        }
        semanticIndex = 0;
        semStringLen = 8;
        return true;
    }
    if (source.substr(pos, 6) == "NORMAL")
    {
        semantic = eVertexSemantic::Normal;
        if (isdigit(source[pos + 6]))
        {
            semanticIndex = CharToInt(source[pos + 6]);
            semStringLen = 7;
            return true;
        }
        semanticIndex = 0;
        semStringLen = 6;
        return true;
    }
    if (source.substr(pos, 8) == "TEXCOORD")
    {
        semantic = eVertexSemantic::Texcoord;
        if (isdigit(source[pos + 8]))
        {
            semanticIndex = CharToInt(source[pos + 8]);
            semStringLen = 9;
            return true;
        }
        semanticIndex = 0;
        semStringLen = 8;
        return true;
    }
    if (source.substr(pos, 5) == "COLOR")
    {
        semantic = eVertexSemantic::Color;
        if (isdigit(source[pos + 5]))
        {
            semanticIndex = CharToInt(source[pos + 5]);
            semStringLen = 6;
            return true;
        }
        semanticIndex = 0;
        semStringLen = 6;
        return true;
    }
    if (source.substr(pos, 9) == "BITANGENT")
    {
        semantic = eVertexSemantic::Bitangent;
        if (isdigit(source[pos + 9]))
        {
            semanticIndex = CharToInt(source[pos + 9]);
            semStringLen = 10;
            return true;
        }
        semanticIndex = 0;
        semStringLen = 9;
        return true;
    }
    if (source.substr(pos, 7) == "TANGENT")
    {
        semantic = eVertexSemantic::Tangent;
        if (isdigit(source[pos + 7]))
        {
            semanticIndex = CharToInt(source[pos + 7]);
            semStringLen = 8;
            return true;
        }
        semanticIndex = 0;
        semStringLen = 7;
        return true;
    }
    return false;
}

VertexLayout GetVertexLayout(const std::string& source)
{
    VertexLayout res;
    size_t structBegin = source.find("struct vIn", 0);
    if (structBegin == std::string::npos)
        throw "Input structure does not exist";
    size_t closeBracetPos = structBegin;
    for (; closeBracetPos < source.size() - 1; ++closeBracetPos)
    {
        if (source[closeBracetPos] == '}')
            break;
    }
    for (size_t i = structBegin; i < closeBracetPos - 6; ++i)
    {
        eDataFormat format;
        eVertexSemantic semantic;
        byte semanticIndex;
        if (source.substr(i, 5) == "float")
        {
            bool typeFound = false;
            if (IsEmptyChar(source[i + m_floatLen]))
            {
                format = eDataFormat::R32;
                typeFound = true;
            }
            else if (isdigit(source[i + m_floatLen]) && IsEmptyChar(source[i + m_floatLen + 1]))
            {
                uint8 dim = CharToInt(source[i + m_floatLen]);
                if (dim == 2)
                    format = eDataFormat::R32_G32;
                else if (dim == 3)
                    format = eDataFormat::R32_G32_B32;
                else if (dim == 4)
                    format = eDataFormat::R32_G32_B32_A32;
                else
                    throw "wtf?";

                typeFound = true;
            }

            if (typeFound)
            {
                bool foundSemStart = false;
                for (size_t j = i + m_floatLen; j < closeBracetPos; ++j)
                {
                    if (source[j] == ':')
                        foundSemStart = true;
                    if (source[j] == ';')
                    {
                        if (!foundSemStart)
                            throw "wtf?";
                        break;
                    }
                    uint8 semStingLen = 0;
                    if (foundSemStart && TryParseSemantics(source, j, semantic, semanticIndex, semStingLen))
                    {
                        res.AddElement(semantic, semanticIndex, format);
                        i = j + semStingLen;
                        break;
                    }
                }
            }
        }
    }
    return res;
}

PipelineState GetPipelineState(std::string& source)
{
    size_t stateStart = source.find("PIPELINE_DESCR:");
    size_t stateEnd = source.find("PIPELINE_DESCR_END;");
    if (stateStart == std::string::npos || stateEnd == std::string::npos)
        return PipelineState();
    std::string stateStr = source.substr(stateStart + 15, stateEnd - stateStart - 15);
    source.erase(stateStart, stateEnd - stateStart + 19);

    PipelineState state = PipelineState::FromYaml(stateStr);
    return state;
}

bool TryParseConstantBufferIndex(const std::string& source, size_t pos, size_t bound, uint16& index, uint16& space)
{
    if (source.substr(pos, 8) == "register")
    {
        space = 0;
        bool indexFound = false;
        size_t p = pos + 8;
        for (; p < bound - 1; ++p)
        {
            if (!indexFound && source[p] == 'b' && isdigit(source[p + 1]))
            {
                index = CharToInt(source[p + 1]);
                indexFound = true;
                break;
            }
        }
        for (; p < bound - 5; ++p)
        {
            if (source.substr(p, 5) == "space" && isdigit(source[p + 5]))
            {
                space = CharToInt(source[p + 5]);
                return true;
            }
        }

        if (!indexFound)
            throw "wtf";
        else
            return true;
    }
    return false;
}

void TryParseParams(const std::string& source, size_t start, size_t end, ConstantBuffer& buffer)
{
    eDataFormat format;
    for (size_t i = start; i < end; ++i)
    {
        bool typeFound = false;
        if (source.substr(i, m_floatLen) == "float")
        {
            if (!typeFound && IsEmptyChar(source[i + m_floatLen]))
            {
                format = eDataFormat::R32;
                i += m_floatLen;
                typeFound = true;
            }
            else if (!typeFound && source[i + m_floatLen] == '3' && source[i + m_floatLen + 1] == 'x' && source[i + m_floatLen + 2] == '3' && IsEmptyChar(source[i + m_floatLen + 3]))
            {
                format = eDataFormat::MATRIX3x3;

                i += m_floatLen + 3;
                typeFound = true;
            }
            else if (!typeFound && source[i + m_floatLen] == '4' && source[i + m_floatLen + 1] == 'x' && source[i + m_floatLen + 2] == '4' && IsEmptyChar(source[i + m_floatLen + 3]))
            {
                format = eDataFormat::MATRIX4x4;

                i += m_floatLen + 3;
                typeFound = true;
            }
            else if (!typeFound && isdigit(source[i + m_floatLen]) && IsEmptyChar(source[i + m_floatLen + 1]))
            {
                uint8 dim = CharToInt(source[i + m_floatLen]);
                if (dim == 2)
                    format = eDataFormat::R32_G32;
                else if (dim == 3)
                    format = eDataFormat::R32_G32_B32;
                else if (dim == 4)
                    format = eDataFormat::R32_G32_B32_A32;
                else
                    throw "wtf?";
                i += m_floatLen + 1;

                typeFound = true;
            }
        }
        if (typeFound)
        {
            size_t opEnd = source.find(";", i);
            if (opEnd == std::string::npos && opEnd > end)
                throw "wtf";
            std::string name;
            bool startComponseName = false;
            for (; i < opEnd; ++i)
            {
                if (!IsEmptyChar(source[i]))
                {
                    startComponseName = true;
                    name += source[i];
                }
                else if (startComponseName == true)
                    break;
            }
            if (!startComponseName)
                throw "wtf";

            if (format == eDataFormat::R32)
                buffer.Add(name, 0);
            else if (format == eDataFormat::R32_G32)
                buffer.Add(name, Vector2());
            else if (format == eDataFormat::R32_G32_B32)
                buffer.Add(name, Vector3());
            else if (format == eDataFormat::R32_G32_B32_A32)
                buffer.Add(name, Vector4());
            else if (format == eDataFormat::MATRIX3x3)
                buffer.Add(name, Matrix3());
            else if (format == eDataFormat::MATRIX4x4)
                buffer.Add(name, Matrix4());
            else
                throw "wtf";
            i = opEnd;
        }
    }
}

std::vector<ConstantBuffer> GetConstantBuffers(const std::string& source)
{
    size_t cbStart = source.find("cbuffer ", 0);
    std::vector<ConstantBuffer> res;
    res.reserve(8);
    while (cbStart != std::string::npos)
    {
        size_t closedBPos = 0;
        if (cbStart == 0 || IsEmptyChar(source[cbStart - 1]))
        {
            uint16 index = 0;
            uint16 space = 0;
            size_t openBPos = source.find("{", cbStart + 6);
            closedBPos = source.find("}", cbStart + 7);
            if (openBPos == std::string::npos || closedBPos == std::string::npos || closedBPos < openBPos)
                throw "wtf";
            bool found = false;
            bool indexAcqired = false;
            for (size_t pos = cbStart + 6; pos < openBPos; ++pos)
            {
                if (source[pos] == ':')
                {
                    found = true;
                    continue;
                }
                if (found && TryParseConstantBufferIndex(source, pos, openBPos, index, space))
                {
                    indexAcqired = true;
                    break;
                }
            }
            if (!found || !indexAcqired)
                throw "wtf";
            res.emplace_back(index, space);
            TryParseParams(source, openBPos, closedBPos, res.back());
        }
        cbStart = source.find("cbuffer ", closedBPos);
    }
    return res;
}

bool TryParseTextureIndex(const std::string& source, size_t pos, size_t bound, uint16& index)
{
    bool registerFound = false;
    for (size_t i = pos; i < bound; ++i)
    {
        if (!registerFound)
        {
            if (!registerFound && source.substr(i, 8) == "register")
            {
                registerFound = true;
                i += 8;
            }
            continue;
        }
        if (source[i] == 't' && isdigit(source[i + 1]))
        {
            index = CharToInt(source[i + 1]);
            return true;
        }
    }
    return false;
}

TextureSet ParseTextures(std::string& source)
{
    TextureSet res;
    size_t texBegin = source.find("[_IN_]");
    while (texBegin != std::string::npos)
    {
        source.erase(texBegin, 6);
        size_t texEnd = source.find(";", texBegin);
        if (texEnd == std::string::npos)
            throw "wtf";
        bool typeFound = false;
        bool nameFound = false;
        std::string name;
        for (size_t i = texBegin; i < texEnd; ++i)
        {
            if (!typeFound)
            {
                if (source.substr(i, 9) == "Texture2D")
                {
                    i += 9;
                    typeFound = true;
                }
                continue;
            }

            if (!nameFound)
            {
                if (IsEmptyChar(source[i]))
                    continue;
                name += source[i];
                if (source[i + 1] == ' ')
                    nameFound = true;
                continue;
            }

            uint16 index = 0;
            if (!TryParseTextureIndex(source, i, texEnd, index))
                throw "wtf";

            res.AddTexture(name, index, nullptr);
            break;
        }
        if (!typeFound || !nameFound)
            throw "wtf";

        texBegin = source.find("[_IN_]", texBegin);
    }
    return res;
}

std::string DXPreprocess(const std::string& source, const std::vector<ShaderDefine>* const defines)
{
    Microsoft::WRL::ComPtr<ID3DBlob> rr; // [a_vorontsov] TODO: dependence from dx compiler. Do better later.
    Microsoft::WRL::ComPtr<ID3DBlob> err;

    const D3D_SHADER_MACRO* macroData = nullptr;
    std::vector<D3D_SHADER_MACRO> macroDefines;
    char buff[4];
    if (defines != nullptr)
    {
        for (const auto& d : *defines)
        {
            macroDefines.emplace_back();
            D3D_SHADER_MACRO* m = &macroDefines.back();
            m->Name = d.Name.c_str();
            snprintf(buff, 4, "%d", d.Value);
            m->Definition = buff;
        }
        macroDefines.emplace_back();
        D3D_SHADER_MACRO* m = &macroDefines.back();
        m->Name = NULL;
        m->Definition = NULL;
        macroData = macroDefines.data();
    };
    D3DPreprocess(source.c_str(), source.size(), nullptr, macroData, nullptr, rr.GetAddressOf(), err.GetAddressOf());
    if (err != nullptr)
        throw "wtf";

    return std::string(reinterpret_cast<char*>(rr->GetBufferPointer()));
}

ShaderData ParseShader(const std::string& path, const std::vector<ShaderDefine>* const defines)
{
    std::string shaderStr = AssetsSystem::ReadFileAsString(path);
    return ParseShaderFromString(shaderStr, defines);
}

ShaderData ParseShaderFromString(std::string source, const std::vector<ShaderDefine>* const defines)
{
    ShaderData res;
    m_preprocessedHeaders.clear();
    source = UnfoldIncludes(source, 0);
    GetPipelineState(source);
    res.textureSet = ParseTextures(source);
    source = DXPreprocess(source, defines);
    OutputDebugStringA(source.c_str());
    res.vertexLayout = GetVertexLayout(source);
    res.constantBuffers = GetConstantBuffers(source);
    res.output = source;
    return res;
}

}
}