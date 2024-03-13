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

    float4 vWeights : BLENDWEIGHT;
    float4 vIndices : BLENDINDICES;
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

#define SPEC_COEFF      saturate(g_float_0) // 반사 계수
#define IS_SKYBOX_ENV   g_btexcube_0
#define SKYBOX_ENV_TEX  g_cube_0

#define bAnimUse        g_int_0
#define bPerlinNoiseUse    g_int_1

#define SpriteLeftTop   g_vec2_0
#define SpriteSize      g_vec2_1
#define SpriteOffset    g_vec2_2
#define SheetSize       g_vec2_3

VS_OUT VS_Std3D(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;

    if (g_iAnim)
    {
        Skinning(_in.vPos, _in.vTangent, _in.vBinormal, _in.vNormal, _in.vWeights, _in.vIndices, 0);
    }
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
    float4 vEmissiveCoeff = (float4) 0.f;
    float2 moveUV = _in.vUV;
    
    //moveUV.y = _in.vUV.y + g_AccTime * 0.5f;
    //moveUV.x = _in.vUV.x - g_AccTime * 0.3f;

    if (g_iTexAnim)
    {
        if (g_vTexFlowDir.x != 0.0f)
        {
            moveUV.x = _in.vUV.x + (g_AccTime * g_fTexFlowSpeed) * g_vTexFlowDir.x;
        }
        if (g_vTexFlowDir.y != 0.0f)
        {
            moveUV.y = _in.vUV.y + (g_AccTime * g_fTexFlowSpeed) * g_vTexFlowDir.y;
        }
        if (g_vTexFlowDir.y == 0.0f && g_vTexFlowDir.x == 0.0f)
        {
            if (g_fTexFlowSpeed != 0.0f)
                moveUV.x = _in.vUV.x - g_AccTime * g_fTexFlowSpeed;
            else
                moveUV.x = _in.vUV.x - g_AccTime * 0.5f;
        }
    }

    if (g_btex_7)
    {
        float2 vNoiseUV = float2(_in.vUV.x - (g_AccTime * 0.2f), _in.vUV.y);
        float4 vNoise = g_tex_7.Sample(g_sam_anti_0, vNoiseUV);

        vNoise = (vNoise - 0.5f) * 0.02f;

        moveUV += vNoise.r;
    }

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

            vOutColor = g_tex_0.Sample(g_sam_anti_0, UV);
        }else
        {
            if (g_btex_0_flow)
            {
                vOutColor = g_tex_0.Sample(g_sam_anti_0, moveUV);
            }
            else
				vOutColor = g_tex_0.Sample(g_sam_anti_0, _in.vUV);
        }
        if (vOutColor.a <= 0.0f)
            discard;
    }
    
    if (g_btex_1)
    {
        float3 vNormal = (float3) 0.f;
        if (g_btex_1_flow)
        {
            vNormal = g_tex_1.Sample(g_sam_anti_0, moveUV).xyz;
        }
        else
        {
        	vNormal = g_tex_1.Sample(g_sam_anti_0, _in.vUV).xyz;
        }
        
        
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
    if (g_btex_2)
    {
        float4 vEmis = (float4) 0.f;
        if (g_btex_2_flow)
        {
            vEmis = g_tex_2.Sample(g_sam_anti_0, moveUV);
        }
        else
            vEmis = g_tex_2.Sample(g_sam_anti_0, _in.vUV);

        
        if (vEmis.r > 0.0f)
        {
            vEmissiveCoeff += vEmis.r * 2.f;
        }
        if (vEmis.g > 0.0f)
        {
            vEmissiveCoeff += vEmis.g * 10.f;
        }
        if (vEmis.b > 0.0f)
        {
            vEmissiveCoeff += vEmis.b * 10.f;
        }

    }
    if (g_btex_3)
    {
        if (g_btex_3_flow)
        {
            vEmissiveCoeff += g_tex_3.Sample(g_sam_anti_0, moveUV);
        }
        else
            vEmissiveCoeff += g_tex_3.Sample(g_sam_anti_0, _in.vUV);
    }

    if (g_btex_4)
    {
        if (g_btex_4_flow)
        {
            if (g_btex_4_emis)
				vEmissiveCoeff += g_tex_4.Sample(g_sam_anti_0, moveUV);
            else
                vOutColor += g_tex_4.Sample(g_sam_anti_0, moveUV);
        }
        else
        {
            if (g_btex_4_emis)
                vEmissiveCoeff += g_tex_4.Sample(g_sam_anti_0, _in.vUV);
            else
                vOutColor += g_tex_4.Sample(g_sam_anti_0, _in.vUV);
        }
    }

    if (g_btex_5)
    {
        if (g_btex_5_flow)
        {
            if (g_btex_5_emis)
                vEmissiveCoeff += g_tex_5.Sample(g_sam_anti_0, moveUV);
            else
                vOutColor += g_tex_5.Sample(g_sam_anti_0, moveUV);
        }
        else
        {
            if (g_btex_5_emis)
                vEmissiveCoeff += g_tex_5.Sample(g_sam_anti_0, _in.vUV);
            else
                vOutColor += g_tex_5.Sample(g_sam_anti_0, _in.vUV);
        }
    }

    //if (g_btex_6)
    //{
    //    if (g_btex_6_flow)
    //    {
    //        if (g_btex_6_emis)
    //            vEmissiveCoeff += g_tex_6.Sample(g_sam_anti_0, moveUV);
    //        else
    //            vOutColor += g_tex_6.Sample(g_sam_anti_0, moveUV);
    //    }
    //    else
    //    {
    //        if (g_btex_6_emis)
    //            vEmissiveCoeff += g_tex_6.Sample(g_sam_anti_0, _in.vUV);
    //        else
    //            vOutColor += g_tex_6.Sample(g_sam_anti_0, _in.vUV);
    //    }
    //}
    tLightColor lightColor = (tLightColor) 0.f;
    float fSpecPow = 0.f;
    
    for (uint i = 0; i < g_Light3DCount; ++i )
    {
        CalcLight3D(_in.vViewPos, _in.vViewNormal, i, lightColor, fSpecPow);
    }
    if (bPerlinNoiseUse)
    {
        float4 vPerlineNoiseColor = (float4) 0.f;
        PerlinNoiseFire(vOutColor, _in.vUV);
        //vOutColor.xyz = vPerlineNoiseColor.xyz - vOutColor.xyz;


        vEmissiveCoeff += vOutColor * 2.f;

        if (g_btex_5_emis)
            vEmissiveCoeff += g_tex_5.Sample(g_sam_anti_0, _in.vUV);
        else
            vOutColor += g_tex_5.Sample(g_sam_anti_0, _in.vUV);

        vOutColor.xyz = (vOutColor.xyz * lightColor.vDiffuse.xyz)
                    + (vOutColor.xyz * lightColor.vAmbient.xyz)
                    + vOutColor * fSpecPow * SPEC_COEFF
					+ vEmissiveCoeff.xyz * g_fEmisCoeff;
 //+= vPerlineNoiseColor;
    }else
    {
        vOutColor.xyz = (vOutColor.xyz * lightColor.vDiffuse.xyz)
                    + (vOutColor.xyz * lightColor.vAmbient.xyz)
                    + saturate(g_Light3DBuffer[0].Color.vDiffuse) * 0.3f * fSpecPow * SPEC_COEFF
					+ vEmissiveCoeff.xyz * g_fEmisCoeff;
    }



    if(IS_SKYBOX_ENV)
    {
        float3 vEye = normalize(_in.vViewPos);
        //float3 vViewReflect = normalize(vViewLightDir + 2.f * (dot(-vViewLightDir, _vViewNormal)) * _vViewNormal);
        float3 vEyeReflect = normalize(reflect(vEye, vViewNormal));

        vEyeReflect = normalize(mul(float4(vEyeReflect, 0.f), g_matViewInv));

        vOutColor += SKYBOX_ENV_TEX.Sample(g_sam_anti_0, vEyeReflect);
    }
    if (paperBurn)
    {
        vOutColor = PaperBurn(vOutColor, _in.vUV, g_tex_6);
    }

    return vOutColor;
}

#endif