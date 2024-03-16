#ifndef _VALUE
#define _VALUE

#include "struct.fx"

cbuffer TRANSFORM : register(b0)
{
    row_major matrix g_matWorld;
    row_major matrix g_matWorldInv;

    row_major matrix g_matView;
    row_major matrix g_matViewInv;

    row_major matrix g_matProj;
    row_major matrix g_matProjInv;
    
    row_major matrix g_matWV;
    row_major matrix g_matWVP;
};

cbuffer MATERIAL : register(b1)
{
    float4 g_vDiff;
    float4 g_vSpec;
    float4 g_vAmb;
    float4 g_vEmv;

    int g_int_0;
    int g_int_1;
    int g_int_2;
    int g_int_3;

    int g_int_4;
    int g_int_5;
    int g_int_6;
    int g_int_7;
    
    float g_float_0;
    float g_float_1;
    float g_float_2;
    float g_float_3;

    float g_float_4;
    float g_float_5;
    float g_float_6;
    float g_float_7;

    float2 g_vec2_0;
    float2 g_vec2_1;

    float2 g_vec2_2;
    float2 g_vec2_3;

    float4 g_vec4_0;
    float4 g_vec4_1;
    float4 g_vec4_2;
    float4 g_vec4_3;
    
    row_major matrix g_mat_0;
    row_major matrix g_mat_1;
    row_major matrix g_mat_2;
    row_major matrix g_mat_3;
    
    int g_btex_0;
    int g_btex_1;
    int g_btex_2;
    int g_btex_3;

    int g_btex_4;
    int g_btex_5;
    int g_btex_6;
    int g_btex_7;

    int g_btexcube_0;
    int g_btexcube_1;
    int g_btexarr_0;
    int g_btexarr_1;

    int g_iAnim;
    int g_iBoneCount;
    int g_iTexAnim;
    float g_fTexFlowSpeed;

    float2 g_vTexFlowDir;
    float g_fEmisCoeff;

    int g_btex_0_flow;
    int g_btex_1_flow;
    int g_btex_2_flow;
    int g_btex_3_flow;

    int g_btex_4_flow;
    int g_btex_5_flow;
    int g_btex_6_flow;
    int g_btex_7_flow;

    int g_btex_8_flow;
    int g_btex_9_flow;
    int g_btex_10_flow;
    int g_btex_11_flow;

    int g_btex_0_emis;
    int g_btex_1_emis;
    int g_btex_2_emis;
    int g_btex_3_emis;

    int g_btex_4_emis;
    int g_btex_5_emis;
    int g_btex_6_emis;
    int g_btex_7_emis;

    int g_btex_8_emis;
    int g_btex_9_emis;
    int g_btex_10_emis;
    int g_btex_11_emis;
    
    int pad;
};

cbuffer GLOBAL : register(b2)
{
    float2 g_Resolution;
    float  g_DT;
    float  g_AccTime;
    uint   g_Light2DCount;
    uint   g_Light3DCount;
    int2   g_globalpadding;
}

cbuffer PAPER_BURN : register(b3)
{
    int paperBurn;
    float paperAcc;
    float paperTime;
    
    float2 vMtrlSize;
    int paperBurnEnd;

    float2 pad2;
}

Texture2D g_tex_0 : register(t0);
Texture2D g_tex_1 : register(t1);
Texture2D g_tex_2 : register(t2);
Texture2D g_tex_3 : register(t3);
Texture2D g_tex_4 : register(t4);
Texture2D g_tex_5 : register(t5);
Texture2D g_tex_6 : register(t6);
Texture2D g_tex_7 : register(t7);

TextureCube g_cube_0 : register(t8);
TextureCube g_cube_1 : register(t9);

Texture2DArray g_arrtex_0 : register(t10);
Texture2DArray g_arrtex_1 : register(t11);

StructuredBuffer<tLightInfo> g_Light2DBuffer : register(t12);
StructuredBuffer<tLightInfo> g_Light3DBuffer : register(t13);

// Animation3D Bone Matrix Buffer
StructuredBuffer<matrix> g_arrBoneMat : register(t30);

SamplerState g_sam_anti_0 : register(s0);
SamplerState g_sam_point_1 : register(s1);
SamplerState g_sam_linear_2 : register(s2);
SamplerComparisonState g_shadow_sampler : register(s3);

#define PI 3.1415926535897932384626433832795f
//#define PI 3.1415926535f

#endif