#ifndef _ANIMATION
#define _ANIMATION

#include "value.fx"
#include "func.fx"

StructuredBuffer<tFrameTrans> g_blendFrameTrans : register(t15);
StructuredBuffer<tFrameTrans> g_arrFrameTrans : register(t16);
StructuredBuffer<matrix> g_arrOffset : register(t17);
RWStructuredBuffer<matrix> g_arrFinelMat : register(u0);

#define BoneCount       g_int_0
#define CurFrame        g_int_1
#define IsBlend         g_int_2
#define NextIdx         g_int_3
#define Ratio           g_float_0
#define NextClipRatio   g_float_1

[numthreads(256, 1, 1)]
void CS_Animation3D(int3 _iThreadIdx : SV_DispatchThreadID)
{
    if (BoneCount <= _iThreadIdx.x)
        return;
    
    float4 vQZero = float4(0.f, 0.f, 0.f, 1.f);
    matrix matBone = (matrix) 0.f;
    
    const uint iFrameDataIndex      = BoneCount * CurFrame + _iThreadIdx.x;
    const uint iNextFrameDataIdx    = BoneCount * (CurFrame + 1) + _iThreadIdx.x;

    float4 vScale   = lerp(g_arrFrameTrans[iFrameDataIndex].vScale, g_arrFrameTrans[iNextFrameDataIdx].vScale, Ratio);
	float4 vTrans   = lerp(g_arrFrameTrans[iFrameDataIndex].vTranslate, g_arrFrameTrans[iNextFrameDataIdx].vTranslate, Ratio);
    float4 qRot     = QuternionLerp(g_arrFrameTrans[iFrameDataIndex].qRot, g_arrFrameTrans[iNextFrameDataIdx].qRot, Ratio);
    
    if (IsBlend)
    {
        const uint nextClipFrameDataIndex = BoneCount * NextIdx + _iThreadIdx.x;

        vScale  = lerp(vScale, g_blendFrameTrans[nextClipFrameDataIndex].vScale, NextClipRatio);
        vTrans  = lerp(vTrans, g_blendFrameTrans[nextClipFrameDataIndex].vTranslate, NextClipRatio);
        qRot    = QuternionLerp(qRot, g_blendFrameTrans[nextClipFrameDataIndex].qRot, NextClipRatio);
    }
    MatrixAffineTransformation(vScale, vQZero, qRot, vTrans, matBone);
    
    //MatrixAffineTransformation(g_arrFrameTrans[iFrameDataIndex].vScale, vQZero, g_arrFrameTrans[iFrameDataIndex].qRot, g_arrFrameTrans[iFrameDataIndex].vTranslate, matBone);

    matrix matOffset = transpose(g_arrOffset[_iThreadIdx.x]);
    
    g_arrFinelMat[_iThreadIdx.x] = mul(matOffset, matBone);
}


#endif