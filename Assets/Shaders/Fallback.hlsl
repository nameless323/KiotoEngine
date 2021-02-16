#include "autogen\Fallback.hlsl"

#include "Includes\Lighting.hlsl"

struct vOut
{
    float4 position : SV_Position;
    float3 normal : NORMAL;
    float3 wPos : TEXCOORD;
    float2 uv : TEXCOORD1;
    float4 lightPos : TEXCOORD2;
};

vOut vs(vIn i)
{
    vOut o;
    float4 pos = mul(float4(i.position.xyz, 1.0f), cbRenderObject.ToWorld);
    o.wPos = pos.xyz;
    o.lightPos = mul(float4(o.wPos, 1.0f), lights.shadowTransform);
    pos = mul(pos, cbCamera.ViewProjection);
    o.position = pos;

    o.normal = mul(float4(i.normal.xyz, 0.0f), cbRenderObject.ToWorld).xyz;
    o.uv = i.uv;
    return o;
}

float4 ps(vOut pIn) : SV_Target
{
    float3 N = normalize(pIn.normal);

    pIn.lightPos /= pIn.lightPos.w;
    pIn.lightPos.xy = pIn.lightPos.xy * 0.5 + 0.5;
    pIn.lightPos.y = 1.0 - pIn.lightPos.y;

    float4 diffuse = float4(0, 0, 0, 0);
    for (uint i = 0; i < LIGHTS_COUNT; ++i)
    {
        float3 blinnPhong = BlinnPhong(lights.light[i], N, pIn.wPos);
        float shadow = 1.0f;
        if (lights.light[i].Type == 0)
        {
            shadow = ShadowTexture.SampleCmpLevelZero(ShadowSampler, pIn.lightPos.xy, pIn.lightPos.z);
            shadow = ShadowTexture.SampleLevel(LinearClampSampler, pIn.lightPos.xy, 0).r;
            shadow = shadow > pIn.lightPos.z - 0.0001;
        }
        diffuse.xyz += (blinnPhong * shadow);
    }
    return diffuse;
}