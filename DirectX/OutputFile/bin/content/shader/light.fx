#ifndef _LIGHT
#define _LIGHT

#include "value.fx"
#include "func.fx"

struct VS_IN
{
    float3 vPos : POSITION;
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
};

// =====================================
// DirLightShader
// MRT              : LIGHT
// Mesh             : RectMesh
// Rasterizer       : CULL_BACK
// DepthStencil     : NO_TEST_NO_WRITE
// Blend            : ONE_ONE

// Parameter
#define NormalTargetTex     g_tex_0
#define PositionTargetTex   g_tex_1
#define ShadowMapTargetTex  g_tex_2
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
    
    // 사용하는 메쉬가 RectMesh(로컬 스페이스에서 반지름 0.5 짜리 정사각형)
    // 따라서 2배로 키워서 화면 전체가 픽셀쉐이더가 호출될 수 있게 한다.
    output.vPosition = float4(_in.vPos.xyz * 2.f, 1.f);
    
    return output;
}


PS_OUT PS_DirLightShader(VS_OUT _in)
{
    PS_OUT output = (PS_OUT) 0.f;
    
    float2 vScreenUV = _in.vPosition.xy / g_Resolution.xy;
    float3 vViewPos = PositionTargetTex.Sample(g_sam_0, vScreenUV).xyz;
    float3 vViewNormal = NormalTargetTex.Sample(g_sam_0, vScreenUV).xyz;
    
    // 픽셀 위치랑 동일한 UV 위치에서 Position 값을 가져왔는데, 해당 지점에 기록된 물체가 없다.
    if (vViewPos.x == 0.f && vViewPos.y == 0.f && vViewPos.z == 0.f)
    {
        discard;
    }
    
    // 광원 계산 결과를 받을 변수 선언
    tLightColor LightColor = (tLightColor) 0.f;
    float fSpecPow = 0.f;
    
    CalcLight3D(vViewPos, vViewNormal, LightIdx, LightColor, fSpecPow);
    
    float fShadowCoeff = 0.f;
    float3 vWorldPos = mul(float4(vViewPos, 1.f), g_matViewInv).xyz;
    float4 vLightProj = mul(float4(vWorldPos, 1.f), LightVP);
    float2 vShadowMapUV = vLightProj.xy / vLightProj.w;
    vShadowMapUV.x = vShadowMapUV.x / 2.f + 0.5f;
    vShadowMapUV.y = (1.f - vShadowMapUV.y / 2.f) - 0.5f;
    
    if (vShadowMapUV.x < 0.f || 1.f < vShadowMapUV.x ||
        vShadowMapUV.y < 0.f || 1.f < vShadowMapUV.y)
    {
        fShadowCoeff = 0.f;
    }
    else
    {
        float fDepth = vLightProj.z / vLightProj.w;
        float fLightDepth2 = ShadowMapTargetTex.Sample(g_sam_2, vShadowMapUV);
        float fLightDepth = PCF(ShadowMapTargetTex, vShadowMapUV, fLightDepth2 - DepthCoeff, FloatCoeff1, float2(FloatCoeff3, FloatCoeff3));
        //fDepth = fDepth - 0.005f;

        if (IsShadow)
        {
            fShadowCoeff = PCF(ShadowMapTargetTex, vShadowMapUV, fDepth - DepthCoeff, FloatCoeff1, float2(2000.f, 2000.f));
            fShadowCoeff += GaussianBlur(ShadowMapTargetTex, vShadowMapUV);
        }
        else
        {
            // fShadowCoeff = 1.f;
        }

        if (fLightDepth2 + 0.002f <= fDepth)
        {
            //fSpecPow = 0.f;
            //fShadowCoeff -= FloatCoeff3;
            //if (SetGaussian)
            //    fShadowCoeff = GaussianBlur(ShadowMapTargetTex, vShadowMapUV);    
            //if (SetGaussian)
        }
    }
    // 계산된 최종 광원의 세기를 각 타겟(Diffuse, Specular) 에 출력
    output.vDiffuse = LightColor.vDiffuse + LightColor.vAmbient;
    output.vSpecular = g_Light3DBuffer[LightIdx].Color.vDiffuse * fSpecPow;
    output.vShadow = fShadowCoeff;

    output.vDiffuse.a = 1.f;
    output.vSpecular.a = 1.f;
    
    return output;
}

// =====================================
// PointLightShader
// MRT              : LIGHT
// Mesh             : SphereMesh
// Rasterizer       : CULL_FRONT
// DepthStencil     : NO_TEST_NO_WRITE
// Blend            : ONE_ONE

// Parameter
#define NormalTargetTex     g_tex_0
#define PositionTargetTex   g_tex_1
#define LightIdx            g_int_0
// =====================================

VS_OUT VS_PointLightShader(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    // 사용하는 메쉬가 RectMesh(로컬 스페이스에서 반지름 0.5 짜리 정사각형)
    // 따라서 2배로 키워서 화면 전체가 픽셀쉐이더가 호출될 수 있게 한다.
    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    
    return output;
}


PS_OUT PS_PointLightShader(VS_OUT _in)
{
	PS_OUT output = (PS_OUT) 0.f;
    
    float2 vScreenUV = _in.vPosition.xy / g_Resolution.xy;
    float3 vViewPos = PositionTargetTex.Sample(g_sam_0, vScreenUV).xyz;
    float3 vViewNormal = NormalTargetTex.Sample(g_sam_0, vScreenUV).xyz;
    
    // 픽셀 위치랑 동일한 UV 위치에서 Position 값을 가져왔는데, 해당 지점에 기록된 물체가 없다.
    if (vViewPos.x == 0.f && vViewPos.y == 0.f && vViewPos.z == 0.f)
    {
        discard;
    }

    float3 vWorldPos = mul(float4(vViewPos, 1.f), g_matViewInv).xyz;
    float3 vLocalPos = mul(float4(vWorldPos, 1.f), g_matWorldInv).xyz;
    float fDist = length(vLocalPos.xyz);

    if (fDist > 0.5f)
    {
        discard;
    }
    
    // 광원 계산 결과를 받을 변수 선언
    tLightColor LightColor = (tLightColor) 0.f;
    float fSpecPow = 0.f;
    
    // 광원 본인의 정보가 LightBuffer 내에서 어디에 있는지 인덱스를 입력
    // 빛을 넣어줄 물체의 위치값과 노말값을 타겟 텍스쳐에서 가져와서 입력
    CalcLight3D(vViewPos, vViewNormal, LightIdx, LightColor, fSpecPow);

    // 계산된 최종 광원의 세기를 각 타겟(Diffuse, Specular) 에 출력
    output.vDiffuse = LightColor.vDiffuse + LightColor.vAmbient;
    output.vSpecular = g_Light3DBuffer[LightIdx].Color.vDiffuse * fSpecPow;
        
    output.vDiffuse.a = 1.f;
    output.vSpecular.a = 1.f;
    
    return output;
}

// =====================================
// PointLightShader
// MRT              : LIGHT
// Mesh             : SphereMesh
// Rasterizer       : CULL_FRONT
// DepthStencil     : NO_TEST_NO_WRITE
// Blend            : ONE_ONE

// Parameter
#define NormalTargetTex     g_tex_0
#define PositionTargetTex  g_tex_1
#define LightIdx            g_int_0
// =====================================

VS_OUT VS_SpotLightShader(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    // 사용하는 메쉬가 RectMesh(로컬 스페이스에서 반지름 0.5 짜리 정사각형)
    // 따라서 2배로 키워서 화면 전체가 픽셀쉐이더가 호출될 수 있게 한다.
    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    
    return output;
}


PS_OUT PS_SpotLightShader(VS_OUT _in)
{
    PS_OUT output = (PS_OUT) 0.f;
    
    float2 vScreenUV = _in.vPosition.xy / g_Resolution.xy;
    float3 vViewPos = PositionTargetTex.Sample(g_sam_0, vScreenUV).xyz;
    float3 vViewNormal = NormalTargetTex.Sample(g_sam_0, vScreenUV).xyz;
    
    // 픽셀 위치랑 동일한 UV 위치에서 Position 값을 가져왔는데, 해당 지점에 기록된 물체가 없다.
    if (vViewPos.x == 0.f && vViewPos.y == 0.f && vViewPos.z == 0.f)
    {
        discard;
    }

    float3 vWorldPos = mul(float4(vViewPos, 1.f), g_matViewInv).xyz;
    float3 vLocalPos = mul(float4(vWorldPos, 1.f), g_matWorldInv).xyz;
    
    // 광원 계산 결과를 받을 변수 선언
    tLightColor LightColor = (tLightColor) 0.f;
    float fSpecPow = 0.f;
    
    // 광원 본인의 정보가 LightBuffer 내에서 어디에 있는지 인덱스를 입력
    // 빛을 넣어줄 물체의 위치값과 노말값을 타겟 텍스쳐에서 가져와서 입력
    CalcLight3D(vViewPos, vViewNormal, LightIdx, LightColor, fSpecPow);

    // 계산된 최종 광원의 세기를 각 타겟(Diffuse, Specular) 에 출력
    output.vDiffuse = LightColor.vDiffuse + LightColor.vAmbient;
    output.vSpecular = g_Light3DBuffer[LightIdx].Color.vDiffuse * fSpecPow;
        
    output.vDiffuse.a = 1.f;
    output.vSpecular.a = 1.f;
    
    return output;
}

// =====================================
// MergeShader
// MRT              : SwapChain
// Domain           : DOMAIN_LIGHT
// Mesh             : RectMesh
// Rasterizer       : CULL_BACK
// DepthStencil     : NO_TEST_NO_WRITE
// Blend            : Default

// Parameter
#define ColorTargetTex    g_tex_0
#define DiffuseTargetTex  g_tex_1
#define SpecularTargetTex g_tex_2
#define EmissiveTargetTex g_tex_3
#define ShadowTargetTex   g_tex_4
#define FloatCoeff        g_float_0
// =====================================
VS_OUT VS_MergeShader(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    // 사용하는 메쉬가 RectMesh(로컬 스페이스에서 반지름 0.5 짜리 정사각형)
    // 따라서 2배로 키워서 화면 전체가 픽셀쉐이더가 호출될 수 있게 한다.
    output.vPosition = float4(_in.vPos.xyz * 2.f, 1.f);
    
    return output;
}


float4 PS_MergeShader(VS_OUT _in) : SV_Target
{
    float4 vOutColor = float4(0.f, 0.f, 0.f, 1.f);
    
    float2 vScreenUV = _in.vPosition.xy / g_Resolution.xy;

    float4 vColor = ColorTargetTex.Sample(g_sam_0, vScreenUV);
    float4 vDiffuse = DiffuseTargetTex.Sample(g_sam_0, vScreenUV);
    float4 vSpecular = SpecularTargetTex.Sample(g_sam_0, vScreenUV);
    float4 vEmissive = EmissiveTargetTex.Sample(g_sam_0, vScreenUV);
    float fShadowCoeff = ShadowTargetTex.Sample(g_sam_1, vScreenUV).r;

    // float fShadowCoeff = GaussianBlur(ShadowTargetTex, _in.vPosition.xy);

    float3 vC = vColor.xyz * vDiffuse.xyz;

    if (0.5f < fShadowCoeff)
    {
        vSpecular.x = 0.f;
        vSpecular.y = 0.f;
        vSpecular.z = 0.f;

    }

    float3 vS = vSpecular.xyz * vColor.a;

    if (0.f < fShadowCoeff)
    {
        vC *= fShadowCoeff;
        vS *= fShadowCoeff;
    }
    
    vOutColor.xyz = vC + vS + vEmissive.xyz;

    //vOutColor.xyz = vColor.xyz * vDiffuse.xyz * (1.f - fShadowCoeff)
				//  + (vSpecular.xyz * vColor.a) * (1.f - fShadowCoeff)
				//  + vEmissive.xyz;

    //vOutColor.xyz = vColor.xyz * vDiffuse.xyz
				//  + (vSpecular.xyz * vColor.a)
				//  + vEmissive.xyz;

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
    
    // g_matWVP
    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
        
    output.vProjPos = output.vPosition;
    output.vProjPos.xyz /= output.vProjPos.w;
            
    return output;
}

float4 PS_ShadowMap(VS_SHADOW_OUT _in) : SV_Target
{
    return float4(_in.vProjPos.z, 0.f, 0.f, 0.f);
}

float4 PS_ShadowMapExp(VS_SHADOW_OUT _in) : SV_Target
{
    return float4(exp(_in.vProjPos.z), 0.f, 0.f, 0.f);
}
#endif