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
    // HeightMap���� ���� y���� ������.

    if (WIDTH <= _iThreadID.x || HEIGHT <= _iThreadID.y)
    {
        return;
    }
    // 1024���� ��尡 �ִٰ� �� ��
    // Width�� Height�� ��带 ����
    // �������� ��尡 UV����

    // ������� �������� ���� ����.
    //int2 vLTPos = vCenterPos - (vScale / 2);
    //float2 vUV = float2(_iThreadID.xy - vLTPos) / float2(vScale);


}

#endif