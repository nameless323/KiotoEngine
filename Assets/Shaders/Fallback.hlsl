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
        float3 L = normalize(lights.light[i].Direction);
        float3 V = normalize(cbCamera.CamWorldPosition - pIn.wPos);
        float3 halfVector = normalize(-L + V);
        float d = max(0.0, dot(N, -L));
        float s = pow(max(0.0, dot(N, halfVector)), 64);
        diffuse.xyz += lights.light[i].Color * (d + s);// diffuse.xyz * lights[i].Color;
    }
    return diffuse;
}