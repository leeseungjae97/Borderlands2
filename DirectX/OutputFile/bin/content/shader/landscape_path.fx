#ifndef _LANDSCAPE_PATH
#define _LANDSCAPE_PATH

#include "value.fx"
#include "struct.fx"

RWStructuredBuffer<tNode> Paths : register(u0);

#define WIDTH               g_int_0
#define HEIGHT              g_int_1

#define WIDTH_MESH_COUNT    g_int_2
#define HEIGHT_MESH_COUNT   g_int_3

#define vLandScapePosition  g_vec4_0
[numthreads(32, 32, 1)]
void CS_AddHeightToPath(int3 _iThreadID : SV_DispatchThreadID)
{
    // HeightMap으로 부터 y값을 가져옴.

    if (WIDTH <= _iThreadID.x || HEIGHT <= _iThreadID.y)
    {
        return;
    }
    // 1024개의 노드가 있다고 할 때
    // Width와 Height로 노드를 나눔
    // 나누어진 노드가 UV단위

    // 순서대로 오는지에 대한 여부.
    //int2 vLTPos = vCenterPos - (vScale / 2);
    //float2 vUV = float2(_iThreadID.xy - vLTPos) / float2(vScale);


}

#endif