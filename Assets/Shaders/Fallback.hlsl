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

cbuffer cbPassBuffer : register(b1)
{
    float4x4 ViewProjection;
    float4x4 View;
    float4 RTParams; // [a_vorontsov] x is the current render target width in pixels, y is the current render target height in pixels, z is (1.0 + 1.0 / width) and w is (1.0 + 1.0/height).
    float3 ProjParams; // [a_vorontsov] x is the camera’s near, z is the camera’s far and w is 1/FarPlane.
    float Pad0;
    float3 CamWorldPosition;
    float Pad1;
};

cbuffer cbRenderObjectBuffer : register(b2)
{
    float4x4 ToWorld;
    float4x4 ToModel;
};

struct vIn
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;
};

struct vOut
{
    float4 position : SV_Position;
    float3 normal : NORMAL;
    float4 dbg : COLOR;
};

vOut vs(vIn i)
{
    float4x4 worldViewProj = ToWorld * ViewProjection;
    float4 pos = mul(float4(i.position.xyz, 1.0f), ToWorld);
    pos = mul(pos, ViewProjection);
    float4 p = mul(float4(i.position.xyz, 1.0f), ToWorld);
    vOut o;
    o.position = float4(i.position.xy, 0.2f, 1.0f);
    o.position = float4(p.xy, 0.2f, 1.0f);
    o.position = pos;

    o.dbg = float4(abs(worldViewProj._m00_m01_m02), 1.0f);
    o.dbg.xyz = (pos.xyz / pos.w);
    o.dbg.xyz = (pos.z / pos.w) > 1.0f;
    float4x4 wv = ToWorld * View;
    o.dbg = mul(float4(i.position, 1.0f), wv);
    o.dbg = float4(0.0f, 0.0f, pos.z > 1.0f, 1.0f);
    o.dbg = float4(0.0f, 0.0f, i.position.x, 1.0f);
    o.normal = i.normal * 0.5f + 0.5f;
    return o;
}

float4 ps(vOut i) : SV_Target
{
    return float4(i.normal, 1.0f);// * (CosTime.w * 0.5 + 0.5);
}