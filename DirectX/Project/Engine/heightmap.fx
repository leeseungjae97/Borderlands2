#ifndef _HEIGHTMAP
#define _HEIGHTMAP

#include "value.fx"
#include "struct.fx"

RWTexture2D<float> HEIGHT_MAP : register(u0); // unordered acess
StructuredBuffer<tRaycastOut> LOCATION : register(t16); // 브러쉬 위치(좌상단 기준)

#define WIDTH       g_int_0
#define HEIGHT      g_int_1

//#define BRUSH_TEX   g_texarr_0
#define BRUSH_TEX   g_tex_0
#define SCALE       g_vec2_0 
#define BRUSH_IDX   g_int_2  
#define POP_UP      g_int_3    

[numthreads(32, 32, 1)]
void CS_HeightMap(int3 _iThreadID : SV_DispatchThreadID)
{
    if (WIDTH <= _iThreadID.x || HEIGHT <= _iThreadID.y || !LOCATION[0].success)
    {
        return;
    }

    int2 vCenterPos = float2(WIDTH, HEIGHT) * LOCATION[0].vUV;
    int2 vScale = float2(WIDTH, HEIGHT) * SCALE;

    if (_iThreadID.x < vCenterPos.x - (vScale.x / 2) || vCenterPos.x + (vScale.x / 2) < _iThreadID.x
        || _iThreadID.y < vCenterPos.y - (vScale.y / 2) || vCenterPos.y + (vScale.y / 2) < _iThreadID.y)
    {
        return;
    }

    // brush texture 에서 샘플링 할 UV 계산
    int2 vLTPos = vCenterPos - (vScale / 2);
    float2 vUV = float2(_iThreadID.xy - vLTPos) / float2(vScale);

    // 브러쉬로 부터 알파값 샘플링
    //float4 vBrushColor = BRUSH_TEX.SampleLevel(g_sam_anti_0, float3(vUV, BRUSH_IDX), 0);
    float4 vBrushColor = BRUSH_TEX.SampleLevel(g_sam_anti_0, vUV, 0);
    float fAlpha = g_DT * vBrushColor.a * 0.2f;
    if (POP_UP == false)
        fAlpha *= -1.0f;
    
    HEIGHT_MAP[_iThreadID.xy] += fAlpha;

    // cos 그래프로 높이 설정
    //float vDist = (distance(vCenterPos, _iThreadID.xy) / vScale) * 3.1415926535f;
    //HEIGHT_MAP[_iThreadID.xy] += saturate(g_DT * cos(vDist) * 0.2f);
}


#endif