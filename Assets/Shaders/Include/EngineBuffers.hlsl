cbuffer cbEngineBuffer : register(b0, space1)
{
    float4 Time; // [a_vorontsov] Time since start: (t / 20, t, t * 2, t * 3).
    float4 SinTime; // [a_vorontsov] Sin of time: (t / 4, t / 2, t, t * 2).
    float4 CosTime; // [a_vorontsov] Cos of time: (t / 4, t / 2, t, t * 2).
    float4 DeltaTime; // [a_vorontsov] Delta time: (dt, 1 / dt, smoothDt, 1 / smoothDt).
}

cbuffer cbCameraBuffer : register(b1, space1)
{
    float4x4 ViewProjection;
    float4x4 View;
    float4 RTParams; // [a_vorontsov] x is the current render target width in pixels, y is the current render target height in pixels, z is (1.0 + 1.0 / width) and w is (1.0 + 1.0/height).
    float3 ProjParams; // [a_vorontsov] x is the camera’s near, z is the camera’s far and w is 1/FarPlane.
    float Pad0;
    float3 CamWorldPosition;
    float Pad1;
};
