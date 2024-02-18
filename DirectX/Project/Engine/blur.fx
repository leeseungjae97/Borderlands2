#ifndef BLUR_FX
#define BLUR_FX
#include "func.fx"
#include "value.fx"

Texture2D<float4> inputTexture : register(t0);
RWTexture2D<float4> outputTexture : register(u0);

[numthreads(8, 8, 1)]
void CS_Blur(uint3 groupID : SV_GroupID, uint3 groupThreadID : SV_GroupThreadID, uint groupIndex : SV_GroupIndex, uint3 dispatchID : SV_DispatchThreadID)
{
    int2 pixel = int2(dispatchID.x, dispatchID.y);

    float4 accumulatedValue = GaussianBlur(inputTexture, pixel);
    //int radius = radiusAndDirection.x;
    //int2 dir = int2(1 - radiusAndDirection.y, radiusAndDirection.y);

    //float4 accumulatedValue = float4(0.0, 0.0, 0.0, 0.0);

    //for (int i = -radius; i <= radius; ++i)
    //{
    //    uint cIndex = (uint) abs(i);
    //    accumulatedValue += coefficients[cIndex >> 2][cIndex & 3] * inputTexture[mad(i, dir, pixel)];
    //}

    outputTexture[pixel] = accumulatedValue;
}

#endif
