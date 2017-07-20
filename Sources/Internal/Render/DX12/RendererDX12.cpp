//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#include "stdafx.h"

#include <array>
#include <vector>

#include "Render/DX12/RendererDX12.h"

namespace Kioto::Renderer
{

using Microsoft::WRL::ComPtr;

void RendererDX12::Init()
{
    UINT dxgiFactoryFlags = 0;
#ifdef _DEBUG
    {
        ComPtr<ID3D12Debug> debugController;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
        {
            debugController->EnableDebugLayer();
            dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
        }
    }
#endif
    ComPtr<IDXGIFactory4> factory;
    CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory));

    ComPtr<IDXGIAdapter1> hardwareAdapter;
    GetHardwareAdapter(factory.Get(), &hardwareAdapter);
}

void RendererDX12::Shutdown()
{

}

void RendererDX12::GetHardwareAdapter(IDXGIFactory4* factory, IDXGIAdapter1** adapter)
{
    std::vector<std::pair<DXGI_ADAPTER_DESC1, UINT>> adapters;
    *adapter = nullptr;
    ComPtr<IDXGIAdapter1> currAdapter;
    for (UINT adapterIndex = 0; factory->EnumAdapters1(adapterIndex, &currAdapter) != DXGI_ERROR_NOT_FOUND; ++adapterIndex)
    {
        DXGI_ADAPTER_DESC1 desc;
        currAdapter->GetDesc1(&desc);

        if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
        {
            continue;
        }

        if (SUCCEEDED(D3D12CreateDevice(currAdapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
        {
            adapters.emplace_back(std::move(desc), adapterIndex);
        }
    }
    static const std::array<UINT, 3> preferredVendors = // [a_vorontsov] http://pcidatabase.com/vendors.php?sort=id
    {
        0x10DE, // NVidia
        0x1002, // AMD
        0x8086 // Intel
    };
    UINT deviceIndex = -1;
    UINT bestVendorIndex = -1;
    for (auto& desc : adapters)
    {
        for (UINT i = 0; i < preferredVendors.size(); ++i)
        {
            if (desc.first.VendorId == preferredVendors[i] && i < bestVendorIndex)
            {
                bestVendorIndex = i;
                deviceIndex = desc.second;
                break;
            }
        }
    }
    if (deviceIndex != -1)
    {
        factory->EnumAdapters1(deviceIndex, adapter);
    }
}

}