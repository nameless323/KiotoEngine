#include "autogen\Fallback.hlsl"

struct vOut
{
    float4 position : SV_Position;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
    float4 dbg : COLOR;
};

vOut vs(vIn i)
{
    float4 pos = mul(float4(i.position.xyz, 1.0f), cbRenderObjectBuffer.ToWorld);
    pos = mul(pos, cbCameraBuffer.ViewProjection);
    vOut o;
    o.position = pos;

    o.dbg = float4(0.0f, 0.0f, i.position.x, 1.0f);
    o.normal = i.normal * 0.5f + 0.5f;
    o.uv = i.uv;
    return o;
}

float4 ps(vOut i) : SV_Target
{
    return Diffuse.Sample(LinearClampSampler, i.uv) * Mask.Sample(LinearClampSampler, i.uv);// * (SinTime.w * 0.5f + 0.5f);
}