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

float CalculateLightAttenuation(Light light, float3 position)
{
    if (light.Type == 0) // [a_vorontcov] Should match eLightType
    {
        return 1.0;
    }
    else if (light.Type == 1)
    {
        float distToLight = length(light.Position - position);
        if (distToLight > light.Data.w)
            return 0.0;
        return 1.0 / (light.Data.x + light.Data.y * distToLight + light.Data.z * distToLight * distToLight);
    }
    else if (light.Type = 2)
    {
        float3 toPoint = normalize(position - light.Position);
        float cosAngle = dot(toPoint, light.Direction);
        float cosInner = cos(light.Data.x);
        float cosOuter = cos(light.Data.y);
        if (cosAngle < cosOuter)
            return 0.0;
        if (cosAngle > cosInner)
            return 1.0;
        return 1.0 - (cosInner - cosAngle) / max((cosInner - cosOuter), 0.0001);
    }
    return 0.0;
}

float4 ps(vOut pIn) : SV_Target
{
    float3 N = normalize(pIn.normal);

    float4 diffuse = float4(0, 0, 0, 0);
    for (uint i = 0; i < LIGHTS_COUNT; ++i)
    {
        float3 L;
        if (lights.light[i].Type == 0)
        {
            L = lights.light[i].Direction;
        }
        else
        {
            L = normalize(pIn.wPos - lights.light[i].Position);
        }
        float3 V = normalize(cbCamera.CamWorldPosition - pIn.wPos);
        float3 halfVector = normalize(-L + V);
        float d = max(0.0, dot(N, -L));
        float s = pow(max(0.0, dot(N, halfVector)), 64);

        float atten = CalculateLightAttenuation(lights.light[i], pIn.wPos);
        if (atten > 0)
            diffuse.xyz += lights.light[i].Color * (d + s) * atten;// diffuse.xyz * lights[i].Color;
    }
    return diffuse;
}