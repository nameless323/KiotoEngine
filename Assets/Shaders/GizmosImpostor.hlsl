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

    float3 position;
    if (i == 0)
    {
        position = impostorData.position.xyz - right.xyz * scale + up.xyz * scale;
    }
    else if (i == 1)
    {
        position = impostorData.position.xyz - right.xyz * scale - up.xyz * scale;
    }
    else if (i == 2)
    {
        position = impostorData.position.xyz + right.xyz * scale + up.xyz * scale;
    }
    else if (i == 3)
    {
        position = impostorData.position.xyz + right.xyz * scale - up.xyz * scale;
    }

    float4 pos = mul(float4(position, 1.0f), cbCamera.ViewProjection);
    o.position = pos;
    o.uv = float2(0, 0);
    return o;
}

float4 ps(vOut i)
{
    return float4(1, 1, 1, 1);
}
