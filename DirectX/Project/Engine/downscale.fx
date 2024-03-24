#ifndef DOWN_SCALE_FX
#define DOWN_SCALE_FX

#include "value.fx"

Texture2D<float4> OriginTex : register(t0);
RWTexture2D<float4> DownScaleTex : register(u0); // 1/4 이미지 해상도

#define DownScaleRS g_vec2_0

void DownScale4(uint2 CurPixel)
{
    if (CurPixel.y < DownScaleRS.y)
    {
        uint3 FullResPos = uint3(uint2(CurPixel * 4), 0.f);
        float4 DownScaled = float4(0.f, 0.f, 0.f, 0.f);

		[unroll]
        for (int i = 0; i < 4; ++i)
        {
			[unroll]
            for (int j = 0; j < 4; ++j)
            {
                DownScaled += OriginTex.Load(FullResPos, uint2(j, i));
            }
        }
        DownScaled /= 16.f;
        DownScaleTex[CurPixel.xy] = DownScaled;
        //DownScaleTex[CurPixel.xy] = OriginTex.Load(FullResPos);
        //DownScaleTex[CurPixel.xy] = OriginTex.Sample(g_sam_point_1, FullResPos);
    }
    GroupMemoryBarrierWithGroupSync();
}

[numthreads(1024, 1, 1)]
void CS_DownScale(uint3 GroupID : SV_GroupID, uint3 DispatchThreadID : SV_DispatchThreadID, uint GroupThreadID : SV_GroupThreadID)
{
    uint2 CurPixel = uint2(DispatchThreadID.x % DownScaleRS.x, DispatchThreadID.x / DownScaleRS.y);

	DownScale4(CurPixel);
}
#endif
