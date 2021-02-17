#include "autogen\ShadowMap.hlsl"

struct vOut
{
    float4 position : SV_Position;
};

vOut vs(vIn inp)
{
    float4 pos = mul(float4(inp.position.xyz, 1.0f), cbRenderObject.ToWorld);
    pos = mul(pos, cbDepthVP.DepthVP);
    vOut o;
    o.position = pos;
    return o;
}

void ps(vOut inp)
{
}