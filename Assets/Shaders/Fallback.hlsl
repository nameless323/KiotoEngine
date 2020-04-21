#include "Include\EngineBuffers.hlsl"

SamplerState LinearClampSampl : register(s0);
[_IN_] Texture2D Diffuse : register(t0);
[_IN_] Texture2D Mask : register(t1);

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
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
    float4 dbg : COLOR;
};

vOut vs(vIn i)
{
    float4 pos = mul(float4(i.position.xyz, 1.0f), ToWorld);
    pos = mul(pos, ViewProjection);
    vOut o;
    o.position = pos;

    o.dbg = float4(0.0f, 0.0f, i.position.x, 1.0f);
    o.normal = i.normal * 0.5f + 0.5f;
    o.uv = i.uv;
    return o;
}

float4 ps(vOut i) : SV_Target
{
    return Diffuse.Sample(LinearClampSampl, i.uv) * Mask.Sample(LinearClampSampl, i.uv);// * (SinTime.w * 0.5f + 0.5f);
}