#include "autogen\GizmosImpostor.hlsl"

struct vOut
{
    float4 position : SV_Position;
    float2 uv : TEXCOORD;
};

vOut vs(vIn i, uint vid : SV_VertexID)
{
    vOut o;

    o.uv = i.position * 0.5 + 0.5;

    float3 fwd = normalize(cbCamera.CamWorldPosition - impostorData.position.xyz);
    float3 right = normalize(cross(float3(0, 1, 0), fwd));
    float3 up = normalize(cross(fwd, right));

    float scale = impostorData.scale;
    right *= scale;
    up *= scale;

    float3 position;
    if (vid == 0)
    {
        position = impostorData.position.xyz - right.xyz + up.xyz;
    }
    else if (vid == 1)
    {
        position = impostorData.position.xyz - right.xyz - up.xyz;
    }
    else if (vid == 2)
    {
        position = impostorData.position.xyz + right.xyz + up.xyz;
    }
    else if (vid == 3)
    {
        position = impostorData.position.xyz + right.xyz - up.xyz;
    }

    float4 pos = mul(float4(position, 1.0f), cbCamera.ViewProjection);
    o.position = pos;
    return o;
}

float4 ps(vOut i) : SV_Target
{
    float4 color = ImpostorSprite.Sample(LinearClampSampler, i.uv);
    if (color.a < impostorData.cutoff)
        discard;
    color.xyz = impostorData.color;
    return color;
}
