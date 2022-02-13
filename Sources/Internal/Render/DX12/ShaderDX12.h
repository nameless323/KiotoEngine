#pragma once

#include "Render/RendererPublic.h"

#include <d3dcompiler.h>
#include <d3d12.h>
#include <wrl.h>

#include "Dx12Helpers/d3dx12.h"
#include "Render/ShaderData.h"

namespace Kioto::Renderer
{
class ShaderDX12
{
public:
    bool operator== (const ShaderDX12& other) const;
    bool operator!= (const ShaderDX12& other) const;
 
    HRESULT Compile(LPCVOID shaderStr, SIZE_T size, LPCSTR sourceName, const D3D_SHADER_MACRO* defines, ID3DInclude* includes, LPCSTR entry, LPCSTR target, UINT flags1, UINT flags2);
    HRESULT Compile(LPCVOID shaderStr, SIZE_T size, LPCSTR entry, LPCSTR target, UINT flags);

    HRESULT CompileFromFile(LPCWSTR fileName, const D3D_SHADER_MACRO* defines, ID3DInclude* includes, LPCSTR entry, LPCSTR target, UINT flags1, UINT flags2);
    HRESULT CompileFromFile(LPCWSTR fileName, LPCSTR entry, LPCSTR target, UINT flags);

    void SetHandle(uint32 handle);
    ShaderProgramHandle GetHandle() const;
    const CD3DX12_SHADER_BYTECODE& GetBytecode() const;
    char* GetErrorMsg() const;
    bool GetIsCompiled() const;
    ShaderProgramType GetType() const;

private:
    ShaderProgramHandle mHandle;
    ShaderProgramType mType;
    CD3DX12_SHADER_BYTECODE mBytecode;
    Microsoft::WRL::ComPtr<ID3DBlob> mShaderBlob;
    Microsoft::WRL::ComPtr<ID3DBlob> mError;
    bool mCompiled = false;
};

inline bool ShaderDX12::operator== (const ShaderDX12& other) const
{
    return mHandle == other.mHandle;
}

inline bool ShaderDX12::operator!= (const ShaderDX12& other) const
{
    return !(*this == other);
}

inline HRESULT ShaderDX12::Compile(LPCVOID shaderStr, SIZE_T size, LPCSTR sourceName, const D3D_SHADER_MACRO* defines, ID3DInclude* includes, LPCSTR entry, LPCSTR target, UINT flags1, UINT flags2)
{
    mCompiled = false;
    if (std::string(entry) == "vs")
        mType = ShaderProgramType::Vertex;
    else if (std::string(entry) == "ps")
        mType = ShaderProgramType::Fragment;
    else
        throw "NOT IMPLEMENTED";

    HRESULT hr = D3DCompile(shaderStr, size, sourceName, defines, D3D_COMPILE_STANDARD_FILE_INCLUDE, entry, target, flags1, flags2, &mShaderBlob, &mError);
    if (SUCCEEDED(hr))
    {
        mBytecode = CD3DX12_SHADER_BYTECODE(mShaderBlob.Get());
        mCompiled = true;
    }
    return hr;
}

inline HRESULT ShaderDX12::Compile(LPCVOID shaderStr, SIZE_T size, LPCSTR entry, LPCSTR target, UINT flags)
{
    return Compile(shaderStr, size, nullptr, nullptr, nullptr, entry, target, flags, 0);
}

inline HRESULT ShaderDX12::CompileFromFile(LPCWSTR fileName, const D3D_SHADER_MACRO* defines, ID3DInclude* includes, LPCSTR entry, LPCSTR target, UINT flags1, UINT flags2)
{
    mCompiled = false;
    if (std::string(entry) == "vs")
        mType = ShaderProgramType::Vertex;
    else if (std::string(entry) == "ps")
        mType = ShaderProgramType::Fragment;
    else
        throw "NOT IMPLEMENTED";

    HRESULT hr = D3DCompileFromFile(fileName, defines, D3D_COMPILE_STANDARD_FILE_INCLUDE, entry, target, flags1, flags2, &mShaderBlob, &mError);
    if (SUCCEEDED(hr))
    {
        mBytecode = CD3DX12_SHADER_BYTECODE(mShaderBlob.Get());
        mCompiled = true;
    }
    return hr;
}

inline HRESULT ShaderDX12::CompileFromFile(LPCWSTR fileName, LPCSTR entry, LPCSTR target, UINT flags)
{
    return CompileFromFile(fileName, nullptr, nullptr, entry, target, flags, 0);
}


inline void ShaderDX12::SetHandle(uint32 handle)
{
    mHandle = handle;
}

inline ShaderProgramHandle ShaderDX12::GetHandle() const
{
    return mHandle;
}

inline const CD3DX12_SHADER_BYTECODE& ShaderDX12::GetBytecode() const
{
    return mBytecode;
}

inline char* ShaderDX12::GetErrorMsg() const
{
    return reinterpret_cast<char*>(mError->GetBufferPointer());
}

inline bool ShaderDX12::GetIsCompiled() const
{
    return mCompiled;
}

inline ShaderProgramType ShaderDX12::GetType() const
{
    return mType;
}
}