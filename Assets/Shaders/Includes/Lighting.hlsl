#include "autogen\CommonStructures.hlsl"

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

float3 BlinnPhong(Light light, float3 normal, float3 position)
{
    float3 L;
    if (light.Type == 0)
    {
        L = light.Direction;
    }
    else
    {
        L = normalize(position - light.Position);
    }
    float3 V = normalize(cbCamera.CamWorldPosition - position);
    float3 halfVector = normalize(-L + V);
    float d = max(0.0, dot(normal, -L));
    float s = pow(max(0.0, dot(normal, halfVector)), 64);

    float atten = CalculateLightAttenuation(light, position);
    if (atten > 0)
        return light.Color * (d + s) * atten;
    return float3(0, 0, 0);
}