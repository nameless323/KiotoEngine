//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

cbuffer cbEngineBuffer : register(b0)
{
    float4 Time; // [a_vorontsov] Time since start: (t / 20, t, t * 2, t * 3).
    float4 SinTime; // [a_vorontsov] Sin of time: (t / 4, t / 2, t, t * 2).
    float4 CosTime; // [a_vorontsov] Cos of time: (t / 4, t / 2, t, t * 2).
    float4 DeltaTime; // [a_vorontsov] Delta time: (dt, 1 / dt, smoothDt, 1 / smoothDt).
}

float4 vs(float3 position : POSITION) : SV_Position
{
    return float4(position, 1.0f);
}

float4 ps() : SV_Target
{
    return float4(1.0f, 0.0f, 1.0f, 1.0f) * (CosTime.w * 0.5 + 0.5);
}