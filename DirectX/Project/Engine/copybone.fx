#ifndef _COPYBONE
#define _COPYBONE

#include "value.fx"

RWStructuredBuffer<matrix> g_arrDestMat : register(u0); // 복사받을 목적지 버퍼
StructuredBuffer<matrix> g_srcMat : register(t16); // 원본 버퍼

// ======================
// BoneMatrix 복사 쉐이더
#define BoneCount   g_int_0
#define RowIndex    g_int_1
// ======================


[numthreads(1024, 1, 1)]
void CS_CopyBoneMatrix(int3 _iThreadIdx : SV_DispatchThreadID)
{
    if (_iThreadIdx.x >= g_int_0)
        return;

    
    // 복사받을 버퍼는 Instancing에 넘겨줄 큰 버퍼, Instancing되는 Object들의 Matrix들이 모여있음.
    // 이건 Instancing 될 때 inst id로 구분되어 vertex shdaer에서 알아서 사용한다.
    // 그래서 우리는 각 Matrix들이 겹치지 않게 index 규칙을 두어 사용.
    // g_srcMat은 Matrix를 가지고있는 StructuredBuffer다.
    // g_srcMat자체가 Matrix 배열이다. StructuredBuffer<matrix>
    
    // 뼈 개수 * vector<StructuredBuffer<matrix>> 인덱스 + 현재 스레드 인덱스.
    // 로 모든 Object들의 StructuredBuffer로 부터 차곡차곡 Matrix정보를 담는다.
    g_arrDestMat[BoneCount * RowIndex + _iThreadIdx.x] = g_srcMat[_iThreadIdx.x];
}

#endif