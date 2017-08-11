//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

namespace Kioto
{

enum class eRenderApi
{
    DirectX12
} RenderApi;

void Init(eRenderApi api);
void Shutdown();

}