// InstancingAnimation3D.fx
#ifndef _INST_ANIMATION
#define _INST_ANIMATION

#include "value.fx"
#include "func.fx"

// Է ۵
StructuredBuffer<tFrameTrans> g_arrFrameTrans : register(t16); //  ִϸ̼ Ű
StructuredBuffer<tFrameTrans> g_blendFrameTrans : register(t15); //  Ű
StructuredBuffer<matrix> g_arrOffset : register(t17); //   Offset (Mesh )

// [߰] Ŵ Ѱ  νϽ  
struct tAnimInstInfo
{
    int iBoneCount;
    int iCurIdx;
    int iNextIdx;
    int iIsBlend;
    int iNextClipIdx;
    float fRatio;
    float fNextClipRatio;
    int iOutBase;
    int iFrameBase;
    int iBlendFrameBase;
    int iOffsetBase;
    int iPad0;
};
StructuredBuffer<tAnimInstInfo> g_AnimInstBuffer : register(t18);

RWStructuredBuffer<matrix> g_arrFinalMat : register(u0);

[numthreads(32, 32, 1)]
void CS_InstancingAnimation3D(uint3 _iThreadIdx : SV_DispatchThreadID)
{
    uint iInstIdx = _iThreadIdx.x;
    uint iBoneIdx = _iThreadIdx.y;
    
    if (iInstIdx >= (uint) g_int_0)
        return;

    tAnimInstInfo info = g_AnimInstBuffer[iInstIdx];
    
    if (iBoneIdx >= (uint) info.iBoneCount)
        return;
    
    uint iFinalOutIdx = info.iOutBase + iBoneIdx;
    
    const uint iFrameDataIdx = info.iFrameBase + info.iBoneCount * info.iCurIdx + iBoneIdx;
    const uint iNextFrameDataIdx = info.iFrameBase + info.iBoneCount * info.iNextIdx + iBoneIdx;
    
    float4 vScale = lerp(g_arrFrameTrans[iFrameDataIdx].vScale, g_arrFrameTrans[iNextFrameDataIdx].vScale, info.fRatio);
    float4 vTrans = lerp(g_arrFrameTrans[iFrameDataIdx].vTranslate, g_arrFrameTrans[iNextFrameDataIdx].vTranslate, info.fRatio);
    float4 qRot = QuternionLerp(g_arrFrameTrans[iFrameDataIdx].qRot, g_arrFrameTrans[iNextFrameDataIdx].qRot, info.fRatio);
    
    if (info.iIsBlend)
    {
        const uint nextClipIdx = info.iBlendFrameBase + info.iBoneCount * info.iNextClipIdx + iBoneIdx;
        vScale = lerp(vScale, g_blendFrameTrans[nextClipIdx].vScale, info.fNextClipRatio);
        vTrans = lerp(vTrans, g_blendFrameTrans[nextClipIdx].vTranslate, info.fNextClipRatio);
        qRot = QuternionLerp(qRot, g_blendFrameTrans[nextClipIdx].qRot, info.fNextClipRatio);
    }
    
    matrix matPosRotBone;
    MatrixAffineTransformation(vScale, float4(0, 0, 0, 1), qRot, vTrans, matPosRotBone);
    
    matrix matOffset = transpose(g_arrOffset[info.iOffsetBase + iBoneIdx]);
    g_arrFinalMat[iFinalOutIdx] = mul(matOffset, matPosRotBone);
}
#endif