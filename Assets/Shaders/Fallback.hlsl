#include "autogen\Fallback.hlsl"

struct vOut
{
    float4 position : SV_Position;
    float3 normal : NORMAL;
    float3 wPos : TEXCOORD;
    float2 uv : TEXCOORD1;
};

vOut vs(vIn i)
{
    vOut o;
    float4 pos = mul(float4(i.position.xyz, 1.0f), cbRenderObjectBuffer.ToWorld);
    o.wPos = pos.xyz;
    pos = mul(pos, cbCameraBuffer.ViewProjection);
    o.position = pos;

    o.normal = mul(float4(i.normal.xyz, 0.0f), cbRenderObjectBuffer.ToWorld).xyz;
    o.uv = i.uv;
    return o;
}

float4 ps(vOut i) : SV_Target
{
    float3 normal = normalize(i.normal);

    float4 diffuse = Diffuse.Sample(LinearClampSampler, i.uv) * Mask.Sample(LinearClampSampler, i.uv);
    //for (uint i = 0; i < LIGHTS_COUNT; ++i)
    {
        diffuse.xyz *= lights.light[0].Color;// diffuse.xyz * lights[i].Color;
    }

    return diffuse;
}