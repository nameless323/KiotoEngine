#include "autogen\Grayscale.hlsl"

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
    o.uv.y = 1.0f - o.uv.y;
    return o;
}

float4 ps(vOut i) : SV_Target
{
    float4 color = InputColor.Sample(LinearClampSampler, i.uv);
    float gs = 0.02126 * color.x + 0.7152 * color.y + 0.0722 * color.z;
    return color;
    //return float4(gs, gs, gs, 1.0);
}