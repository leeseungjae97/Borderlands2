#ifndef _LIGHT
#define _LIGHT

#include "value.fx"
#include "func.fx"

struct VS_IN
{
    float3 vPos : POSITION;

    float4 vWeights : BLENDWEIGHT;
    float4 vIndices : BLENDINDICES;
};

struct VS_OUT
{
    float4 vPosition : SV_Position;
};

struct PS_OUT
{
    float4 vDiffuse : SV_Target0;
    float4 vSpecular : SV_Target1;
    float4 vShadow : SV_Target2;
    float4 vEmissive : SV_Target3;
};

#define NormalTargetTex     g_tex_0
#define PositionTargetTex   g_tex_1
#define ShadowMapTargetTex  g_tex_2
#define TangentTargetTex    g_tex_3

#define LightIdx            g_int_0
#define LightVP             g_mat_0
#define IsShadow            g_int_1
#define SetGaussian         g_int_2
#define DepthCoeff          g_float_0
#define FloatCoeff1         g_float_1
#define FloatCoeff2         g_float_2
#define FloatCoeff3         g_float_3
// =====================================

VS_OUT VS_DirLightShader(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    output.vPosition = float4(_in.vPos.xyz * 2.f, 1.f);
    
    return output;
}


PS_OUT PS_DirLightShader(VS_OUT _in)
{
    PS_OUT output = (PS_OUT) 0.f;
    
    float2 vScreenUV = _in.vPosition.xy / g_Resolution.xy;
    float3 vViewPos = PositionTargetTex.Sample(g_sam_anti_0, vScreenUV).xyz;
    float3 vViewNormal = NormalTargetTex.Sample(g_sam_anti_0, vScreenUV).xyz;
    
    if (vViewPos.x == 0.f && vViewPos.y == 0.f && vViewPos.z == 0.f)
    {
        discard;
    }
    
    tLightColor LightColor = (tLightColor) 0.f;
    float fSpecPow = 0.f;

    //vViewNormal = mul(float4(vViewNormal, 1.f), g_matView).xyz;
    //vViewPos = mul(float4(vViewPos, 1.f), g_matView).xyz;

    CalcLight3D(vViewPos, vViewNormal, LightIdx, LightColor, fSpecPow);
    
    float fShadowCoeff = 0.f;
    float3 vWorldPos = mul(float4(vViewPos, 1.f), g_matViewInv).xyz;
    float4 vLightProj = mul(float4(vWorldPos, 1.f), LightVP);
    float2 vShadowMapUV = vLightProj.xy / vLightProj.w;
    vShadowMapUV.x = vShadowMapUV.x / 2.f + 0.5f;
    vShadowMapUV.y = (1.f - vShadowMapUV.y / 2.f) - 0.5f;

    //output.vDiffuse = LightColor.vDiffuse + LightColor.vAmbient;
    //output.vSpecular = g_Light3DBuffer[LightIdx].Color.vDiffuse * fSpecPow;
    //output.vShadow = fShadowCoeff;

    if (vShadowMapUV.x < 0.f || 1.f < vShadowMapUV.x ||
        vShadowMapUV.y < 0.f || 1.f < vShadowMapUV.y)
    {
        fShadowCoeff = 0.f;
    }
    else
    {
        float fDepth = vLightProj.z / vLightProj.w;
        //float fDepth = vLightProj.z;
        //float fDepth = ShadowMapTargetTex.Sample(g_sam_anti_0, vShadowMapUV);
        fDepth -= DepthCoeff;
        float SMAP_SIZE = FloatCoeff1;
        float SMAP_DX = 1.0f / SMAP_SIZE;

        const float dx = SMAP_DX;

        const float2 offsets[9] =
        {
            float2(-dx, -dx), float2(0.0f, -dx), float2(dx, -dx),
	        float2(-dx, 0.0f), float2(0.0f, 0.0f), float2(dx, 0.0f),
	        float2(-dx, +dx), float2(0.0f, +dx), float2(dx, +dx)
        };

        [unroll]
        for (int i = 0; i < 9; ++i)
        {
            fShadowCoeff += ShadowMapTargetTex.SampleCmpLevelZero(g_shadow_sampler,
            vShadowMapUV.xy + offsets[i], fDepth).r;
        }
        fShadowCoeff /= FloatCoeff2;
        //fShadowCoeff = PCF(ShadowMapTargetTex, vShadowMapUV, fDepth - DepthCoeff, FloatCoeff1, float2(FloatCoeff2, FloatCoeff3));
        
        //if (fShadowDepth != 0.f && (fDepth > fShadowDepth + 0.00001f))
        //{
        //    output.vDiffuse *= 0.3f;
        //    output.vSpecular = 0.f;
        //    fShadowCoeff = 0.9f;
        //}
    }
    output.vDiffuse += LightColor.vDiffuse + LightColor.vAmbient;
    output.vSpecular += g_Light3DBuffer[LightIdx].Color.vDiffuse * fSpecPow;
    output.vShadow = fShadowCoeff;

    output.vDiffuse.a = 1.f;
    output.vSpecular.a = 1.f;
    
    return output;
}

VS_OUT VS_PointLightShader(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    
    return output;
}


PS_OUT PS_PointLightShader(VS_OUT _in)
{
    PS_OUT output = (PS_OUT) 0.f;
    
    float2 vScreenUV = _in.vPosition.xy / g_Resolution.xy;
    
    float3 vViewPos = PositionTargetTex.Sample(g_sam_anti_0, vScreenUV).xyz;
    float3 vViewNormal = NormalTargetTex.Sample(g_sam_anti_0, vScreenUV).xyz;

    if (vViewPos.x == 0.f && vViewPos.y == 0.f && vViewPos.z == 0.f)
    {
        discard;
    }
    
    float3 vWorldPos = mul(float4(vViewPos, 1.f), g_matViewInv).xyz;
    float3 vLocal = mul(float4(vWorldPos, 1.f), g_matWorldInv).xyz;
    float fDist = length(vLocal.xyz);

    if (0.5f < fDist)
    {
        discard;
    }
        
    tLightColor LightColor = (tLightColor) 0.f;
    float fSpecPow = 0.f;
    
    CalcLight3D(vViewPos, vViewNormal, LightIdx, LightColor, fSpecPow);

    output.vDiffuse += LightColor.vDiffuse + LightColor.vAmbient;
    output.vSpecular += g_Light3DBuffer[LightIdx].Color.vDiffuse * fSpecPow;
    //output.vShadow = 1.0f;
    //output.vEmissive = (LightColor.vDiffuse + LightColor.vAmbient) * fSpecPow;
    output.vDiffuse.a = 1.f;
    output.vSpecular.a = 1.f;

    return output;
}

#define NormalTargetTex     g_tex_0
#define PositionTargetTex  g_tex_1
#define LightIdx            g_int_0
// =====================================

VS_OUT VS_SpotLightShader(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    
    return output;
}


PS_OUT PS_SpotLightShader(VS_OUT _in)
{
    PS_OUT output = (PS_OUT) 0.f;
    
    //float2 vScreenUV = _in.vPosition.xy / g_Resolution.xy;
    //float3 vViewPos = PositionTargetTex.Sample(g_sam_anti_0, vScreenUV).xyz;
    //float3 vViewNormal = NormalTargetTex.Sample(g_sam_anti_0, vScreenUV).xyz;
    
    //// 픽셀 위치랑 동일한 UV 위치에서 Position 값을 가져왔는데, 해당 지점에 기록된 물체가 없다.
    //if (vViewPos.x == 0.f && vViewPos.y == 0.f && vViewPos.z == 0.f)
    //{
    //    discard;
    //}

    //float fShadowCoeff = 0.f;
    
    //float3 vWorldPos = mul(float4(vViewPos, 1.f), g_matViewInv).xyz;
    //float3 vLocalPos = mul(float4(vWorldPos, 1.f), g_matWorldInv).xyz;
    //float4 vLightProj = mul(float4(vWorldPos, 1.f), LightVP);
    //float2 vShadowMapUV = vLightProj.xy / vLightProj.w;
    //vShadowMapUV.x = vShadowMapUV.x / 2.f + 0.5f;
    //vShadowMapUV.y = (1.f - vShadowMapUV.y / 2.f) - 0.5f;
    //if (vShadowMapUV.x < 0.f || 1.f < vShadowMapUV.x ||
    //    vShadowMapUV.y < 0.f || 1.f < vShadowMapUV.y)
    //{
    //    fShadowCoeff = 0.f;
    //}
    //else
    //{

    //}
    //// 광원 계산 결과를 받을 변수 선언
    //tLightColor LightColor = (tLightColor) 0.f;
    //float fSpecPow = 0.f;
    
    //CalcLight3D(vViewPos, vViewNormal, LightIdx, LightColor, fSpecPow);
    
    //output.vDiffuse = LightColor.vDiffuse + LightColor.vAmbient;
    //output.vSpecular = g_Light3DBuffer[LightIdx].Color.vDiffuse * fSpecPow;
    //output.vShadow = 0.0f;
    //output.vDiffuse.a = 1.f;
    //output.vSpecular.a = 1.f;
    
    return output;
}

#define ColorTargetTex    g_tex_0
#define DiffuseTargetTex  g_tex_1
#define SpecularTargetTex g_tex_2
#define EmissiveTargetTex g_tex_3
#define ShadowTargetTex   g_tex_4
// =====================================
VS_OUT VS_MergeShader(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;

    output.vPosition = float4(_in.vPos.xyz * 2.f, 1.f);
    
    return output;
}

float4 PS_MergeShader(VS_OUT _in) : SV_Target
{
    float4 vOutColor = (float4) 0.f;
    
    float2 vScreenUV = _in.vPosition.xy / g_Resolution.xy;

    float4 vColor = ColorTargetTex.Sample(g_sam_anti_0, vScreenUV);
    float4 vDiffuse = DiffuseTargetTex.Sample(g_sam_anti_0, vScreenUV);
    float4 vEmissive = EmissiveTargetTex.Sample(g_sam_anti_0, vScreenUV);
    float4 vSpecular = SpecularTargetTex.Sample(g_sam_anti_0, vScreenUV);
    float fShadowCoeff = ShadowTargetTex.Sample(g_sam_anti_0, vScreenUV).x;

    float3 vColorDiffuse = vColor.xyz * vDiffuse.xyz;
    vColorDiffuse *= fShadowCoeff;
    float3 vColorSpecular = vSpecular.xyz * vColor.a;
    vColorSpecular *= fShadowCoeff;

    //float3 vColorDiffuseShadow = vColorDiffuse * fShadowCoeff;
    //vColorDiffuseShadow = (vColorDiffuseShadow + vColorDiffuse / 2.f);

    vOutColor.xyz = vColorDiffuse + vColorSpecular + vEmissive.xyz;
    vOutColor.a = 1.f;

    return vOutColor;
}

VS_OUT VS_MShader(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    output.vPosition = float4(_in.vPos.xyz * 2.f, 1.f);
    
    return output;
}


float4 PS_MShader(VS_OUT _in) : SV_Target
{
    return _in.vPosition;
}

struct VS_SHADOW_OUT
{
    float4 vPosition : SV_Position;
    float4 vProjPos : POSITION;
};

VS_SHADOW_OUT VS_ShadowMap(VS_IN _in)
{
    VS_SHADOW_OUT output = (VS_SHADOW_OUT) 0.f;

    if (g_iAnim)
    {
        ShadowSkinning(_in.vPos, _in.vWeights, _in.vIndices, 0);
    }

    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    
    output.vProjPos = output.vPosition;
    output.vProjPos.xyz /= output.vProjPos.w;
            
    return output;
}

float4 PS_ShadowMap(VS_SHADOW_OUT _in) : SV_Target
{
    return float4(_in.vProjPos.z, 0.f, 0.f, 0.f);
}
#endif