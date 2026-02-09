#ifndef _ANIMATION
#define _ANIMATION

#include "value.fx"
#include "func.fx"

StructuredBuffer<tFrameTrans> g_blendFrameTrans : register(t15);
StructuredBuffer<tFrameTrans> g_arrFrameTrans : register(t16);
StructuredBuffer<matrix> g_arrOffset : register(t17);
RWStructuredBuffer<matrix> g_arrFinalMat : register(u0);
RWStructuredBuffer<matrix> g_arrFinalMatPos : register(u1);
RWStructuredBuffer<matrix> g_arrFinalMatRot : register(u2);
//RWStructuredBuffer<matrix> g_arrBoneIndividualMat : register(u1);

#define BoneCount       g_int_0
#define CurIdx          g_int_1
#define NextIdx         g_int_2
#define IsBlend         g_int_3
#define NextClipIdx     g_int_4
#define IsTransOnly     g_int_5
#define IsRotateOnly    g_int_6

#define Ratio           g_float_0
#define NextClipRatio   g_float_1

[numthreads(1024, 1, 1)]
void CS_Animation3D(int3 _iThreadIdx : SV_DispatchThreadID)
{
    if (BoneCount <= _iThreadIdx.x)
        return;
    
    float4 vQZero = float4(0.f, 0.f, 0.f, 1.f);
    matrix matPosRotBone = (matrix) 0.f;
    matrix matPosBone = (matrix) 0.f;
    matrix matRotBone = (matrix) 0.f;
    
    const uint iFrameDataIndex      = BoneCount * CurIdx + _iThreadIdx.x;
    const uint iNextFrameDataIdx    = BoneCount * NextIdx + _iThreadIdx.x;
    
    float4 vScale   = lerp(g_arrFrameTrans[iFrameDataIndex].vScale, g_arrFrameTrans[iNextFrameDataIdx].vScale, Ratio);
	float4 vTrans   = lerp(g_arrFrameTrans[iFrameDataIndex].vTranslate, g_arrFrameTrans[iNextFrameDataIdx].vTranslate, Ratio);
    float4 qRot = QuternionLerp(g_arrFrameTrans[iFrameDataIndex].qRot, g_arrFrameTrans[iNextFrameDataIdx].qRot, Ratio);
    
    if (IsBlend)
    {
        const uint nextClipFrameDataIndex = BoneCount * NextClipIdx + _iThreadIdx.x;

        vScale = lerp(vScale, g_blendFrameTrans[nextClipFrameDataIndex].vScale, NextClipRatio);
        vTrans = lerp(vTrans, g_blendFrameTrans[nextClipFrameDataIndex].vTranslate, NextClipRatio);
        qRot = QuternionLerp(qRot, g_blendFrameTrans[nextClipFrameDataIndex].qRot, NextClipRatio);
    }

    MatrixAffineTranslate(vTrans, matPosBone);
    g_arrFinalMatPos[_iThreadIdx.x] = matPosBone;

    MatrixAffineRotate(vScale, vQZero, qRot, matRotBone);
    g_arrFinalMatRot[_iThreadIdx.x] = matRotBone;

    MatrixAffineTransformation(vScale, vQZero, qRot, vTrans, matPosRotBone);
    matrix matOffset = transpose(g_arrOffset[_iThreadIdx.x]);
    g_arrFinalMat[_iThreadIdx.x] = mul(matOffset, matPosRotBone);
}


#endif