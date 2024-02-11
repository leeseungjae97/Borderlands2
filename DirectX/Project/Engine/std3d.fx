#ifndef _STD2D
#define _STD2D

#include "value.fx"
#include "func.fx"

struct VS_IN
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
    
    float3 vTangent : TANGENT;
    float3 vNormal : NORMAL;
    float3 vBinormal : BINORMAL;
};

struct VS_OUT
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;

    float3 vViewPos : POSITION;
    float3 vViewNormal : NORMAL;
    float3 vViewTangent : TANGENT;
    float3 vViewBinormal : BINORMAL;
};

static float3 g_vLightPos = float3(0.f, 0.f, 0.f);
static float3 g_vLightDir = float3(1.f, 1.f, 1.f);

static float3 g_vLightColor = float3(1.f, 1.f, 1.f);
static float3 g_vLightSpec = float3(0.3f, 0.3f, 0.3f);
static float  g_fLightSpecCoeff = 0.3f;
static float3 g_vLightAmb = float3(0.15f, 0.15f, 0.15f);

#define SPEC_COEFF      saturate(g_float_0) // 반사 계수
#define IS_SKYBOX_ENV   g_btexcube_0
#define SKYBOX_ENV_TEX  g_cube_0

#define bAnimUse        g_int_0
#define SpriteLeftTop   g_vec2_0
#define SpriteSize      g_vec2_1
#define SpriteOffset    g_vec2_2
#define SheetSize       g_vec2_3

VS_OUT VS_Std3D(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;

    output.vViewPos = mul(float4(_in.vPos, 1.f), g_matWV);
    output.vViewNormal = normalize(mul(float4(_in.vNormal, 0.f), g_matWV)).xyz;
    output.vViewTangent = normalize(mul(float4(_in.vTangent, 0.f), g_matWV)).xyz;
    output.vViewBinormal = normalize(mul(float4(_in.vBinormal, 0.f), g_matWV)).xyz;

    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    output.vUV = _in.vUV;

    return output;
}
float4 PS_Std3D(VS_OUT _in) : SV_Target
{
    float4 vOutColor = float4(1.f, 1.f, 1.f, 1.f);

    float3 vViewNormal = _in.vViewNormal;

    if (g_btex_0)
    {
        if (bAnimUse)
        {
            float2 diff = (SheetSize - SpriteSize) / 2.0f;
            float2 UV = (SpriteLeftTop - diff - SpriteOffset)
                + (SheetSize * _in.vUV);

            if (UV.x < SpriteLeftTop.x || UV.x > SpriteLeftTop.x + SpriteSize.x
            || UV.y < SpriteLeftTop.y || UV.y > SpriteLeftTop.y + SpriteSize.y)
                discard;

            vOutColor = g_tex_0.Sample(g_sam_0, UV);
        }else
        {
            vOutColor = g_tex_0.Sample(g_sam_0, _in.vUV);
        }
        
    }
    
    if (g_btex_1)
    {
        float3 vNormal = g_tex_1.Sample(g_sam_0, _in.vUV).xyz;
        
        // 0 ~ 1 범위의 값을 -1 ~ 1 로 확장        
        vNormal = vNormal * 2.f - 1.f;
        
        float3x3 vRotateMat =
        {
            _in.vViewTangent,
            -_in.vViewBinormal,
            _in.vViewNormal        
        };
        
        vViewNormal = normalize(mul(vNormal, vRotateMat));
    }

    tLightColor lightColor = (tLightColor) 0.f;
    float fSpecPow = 0.f;
    
    for (uint i = 0; i < g_Light3DCount; ++i )
    {
        CalcLight3D(_in.vViewPos, _in.vViewNormal, i, lightColor, fSpecPow);
    }
       
    vOutColor.xyz = (vOutColor.xyz * lightColor.vDiffuse.xyz)
                    + (vOutColor.xyz * lightColor.vAmbient.xyz)
                    + saturate(g_Light3DBuffer[0].Color.vDiffuse) * fSpecPow * SPEC_COEFF;

    if(IS_SKYBOX_ENV)
    {
        float3 vEye = normalize(_in.vViewPos);
        //float3 vViewReflect = normalize(vViewLightDir + 2.f * (dot(-vViewLightDir, _vViewNormal)) * _vViewNormal);
        float3 vEyeReflect = normalize(reflect(vEye, vViewNormal));

        vEyeReflect = normalize(mul(float4(vEyeReflect, 0.f), g_matViewInv));

        vOutColor += SKYBOX_ENV_TEX.Sample(g_sam_0, vEyeReflect);
    }

    return vOutColor;
}

#endif