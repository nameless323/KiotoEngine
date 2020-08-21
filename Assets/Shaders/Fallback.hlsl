#include "autogen\Fallback.hlsl"

#include "Includes\Lighting.hlsl"

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
    float4 pos = mul(float4(i.position.xyz, 1.0f), cbRenderObject.ToWorld);
    o.wPos = pos.xyz;
    pos = mul(pos, cbCamera.ViewProjection);
    o.position = pos;

    o.normal = mul(float4(i.normal.xyz, 0.0f), cbRenderObject.ToWorld).xyz;
    o.uv = i.uv;
    return o;
}

float4 ps(vOut pIn) : SV_Target
{
    float3 N = normalize(pIn.normal);

    float4 diffuse = float4(0, 0, 0, 0);
    for (uint i = 0; i < LIGHTS_COUNT; ++i)
    {
        diffuse.xyz += BlinnPhong(lights.light[i], N, pIn.wPos);
    }
    return diffuse;
}