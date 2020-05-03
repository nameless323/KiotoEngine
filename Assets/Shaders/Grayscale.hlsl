#include "Include\EngineBuffers.hlsl"

[_IN_] Texture2D IntputColor : register(t0);

struct vIn
{
    float3 position : POSITION;
};

struct vOut
{
    float4 position : SV_Position;
    float2 uv : TEXCOORD;
};

vOut vs(vIn i)
{
    vOut o;
    o.position = float4(i.position.xy, 0.0f, 1.0f);
    o.uv = i.position.xy * 0.5 + 0.5;
    return o;
}

float4 ps(vOut i) : SV_Target
{
    float4 color = IntputColor.Sample(LinearClampSampler, i.uv);
    float gs = 0.02126 * color.x + 0.7152 * color.y + 0.0722 * color.z;
    return float4(gs, gs, gs, 1.0);
}