//#ifndef _LANDSCAPE_PATH
//#define _LANDSCAPE_PATH

//#include "value.fx"
//#include "struct.fx"

//RWStructuredBuffer<tNode> Paths : register(u0);

//#define WIDTH               g_int_0
//#define HEIGHT              g_int_1

//#define WIDTH_MESH_COUNT    g_int_2
//#define HEIGHT_MESH_COUNT   g_int_3

//#define vLandScapePosition  g_vec4_0
//[numthreads(32, 32, 1)]
//void CS_MakePath(int3 _iThreadID : SV_DispatchThreadID)
//{
//    //if (WIDTH_MESH_COUNT >= _iThreadID.x || WIDTH_MESH_COUNT >= _iThreadID.y)
//    //{
//    //    return;
//    //}

//    //float width     = (float) WIDTH / WIDTH_MESH_COUNT;
//    //float height    = (float) HEIGHT / HEIGHT_MESH_COUNT;

//    //float centerPosX = vLandScapePosition + (width * _iThreadID.x) + width / 2.f;
//    //float centerPosZ = vLandScapePosition + (height * _iThreadID.y) + height / 2.f;

//    //int idx = _iThreadID.xy;

//    //Paths[idx].iCoordX = _iThreadID.x;
//    //Paths[idx].iCoordY = _iThreadID.y;
//    //Paths[idx].vPos = float3(centerPosX, vLandScapePosition.y, centerPosZ);
//}

//#endif