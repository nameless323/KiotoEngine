#include "autogen\GizmosImpostor.hlsl"

struct vOut
{
    float4 position : SV_Position;
    float2 uv : TEXCOORD;
};

vOut vs(vIn i, uint vid : SV_VertexID)
{
    vOut o;

    float3 fwd = cbCamera.CamWorldPosition - impostorData.position.xyz;
    float3 right = cross(fwd, float3(0, 1, 0));
    float3 up = cross(fwd, right);

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
    o.uv = float2(0, 0);
    return o;
}

float4 ps(vOut i) : SV_Target
{
    return float4(1, 1, 1, 1);
}
