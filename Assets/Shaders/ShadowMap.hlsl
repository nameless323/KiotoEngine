#include "autogen\ShadowMap.hlsl"

struct vOut
{
    float4 position : SV_Position;
    float4 outPos : TEXCOORD0;
};

vOut vs(vIn inp)
{
    float4 pos = mul(float4(inp.position.xyz, 1.0f), cbRenderObject.ToWorld);
    pos = mul(pos, cbCamera.ViewProjection);
    vOut o;
    o.position = pos;
    o.outPos = pos;
    return o;
}

float4 ps(vOut inp) : SV_Target
{
    float depth = inp.outPos.z / inp.outPos.w;
    return float4(depth.xxx, 1.0f);
}