#include "Include\EngineBuffers.hlsl"

cbuffer cbRenderObjectBuffer : register(b0, space0)
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
};

vOut vs(vIn i)
{
    float4 pos = mul(float4(i.position.xyz, 1.0f), ToWorld);
    pos = mul(pos, ViewProjection);
    vOut o;
    o.position = pos;

    return o;
}

float4 ps(vOut i) : SV_Target
{
    return float4(0, 1, 0, 1);
}