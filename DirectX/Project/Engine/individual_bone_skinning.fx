#ifndef ANIMATION_VERTEX_FX
#define ANIMATION_VERTEX_FX

#include "value.fx"
#include "func.fx"

//#define VertexPosition g_vec4_0
//#define VertexIndices g_vec4_1
//#define VertexWeight g_vec4_2

#define BoneIdx g_int_0
//#define IsBoneSkinning g_int_1

RWStructuredBuffer<matrix> animationBoneMatrix : register(u0);
RWStructuredBuffer<matrix> boneMat : register(u1);

[numthreads(1, 1, 1)]
void CS_IndividualBone(int3 _iThreadIdx : SV_DispatchThreadID)
{
    //float4 pos = VertexPosition;
    //float4 weight = VertexWeight;
    //float4 indices = VertexIndices;

    int boneIdx = BoneIdx;

    // animation중인 vertex의 위치를 가져오고 싶을때 사용
  //  if (IsBoneSkinning)
  //      pos = mul(pos, boneMat[boneIdx]);
  //  else
	//  VertexSkinning(pos, weight, indices, boneMat);

    animationBoneMatrix[0] = boneMat[boneIdx];
}


#endif
