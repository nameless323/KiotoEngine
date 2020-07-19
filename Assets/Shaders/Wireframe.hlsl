#include "autogen\Wireframe.hlsl"

struct vOut
{
    float4 position : SV_Position;
};

vOut vs(vIn i)
{
    float4 pos = mul(float4(i.position.xyz, 1.0f), cbRenderObjectBuffer.ToWorld);
    pos = mul(pos, cbCameraBuffer.ViewProjection);
    vOut o;
    o.position = pos;

    return o;
}

float4 ps(vOut i) : SV_Target
{
    return float4(0, 1, 0, 1);
}