//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

float4 vert(float3 position : POSITION) : SV_Position
{
    return float4(position, 1.0f);
}

float4 frag() : SV_Target
{
    return float4(1.0f, 0.0f, 1.0f, 1.0f);
}