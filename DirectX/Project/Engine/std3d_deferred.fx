#ifndef _STD3D_DEFERRED
#define _STD3D_DEFERRED

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

struct VTX_IN_INST
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;

    float3 vTangent : TANGENT;
    float3 vNormal : NORMAL;
    float3 vBinormal : BINORMAL;

    float4 vWeights : BLENDWEIGHT;
    float4 vIndices : BLENDINDICES;

    // Per Instance Data
    row_major matrix matWorld : WORLD;
    row_major matrix matWV : WV;
    row_major matrix matWVP : WVP;
    uint iRowIndex : ROWINDEX;

    //uint iInstID : SV_InstanceID;
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


VS_OUT VS_Std3D_Deferred(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;

    if (g_iAnim)
    {
        Skinning(_in.vPos, _in.vTangent, _in.vBinormal, _in.vNormal, _in.vWeights, _in.vIndices, 0);
    }

    // 로컬에서의 Normal 방향을 월드로 이동
    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    output.vUV = _in.vUV;

    output.vViewPos = mul(float4(_in.vPos, 1.f), g_matWV);
    output.vViewNormal = normalize(mul(float4(_in.vNormal, 0.f), g_matWV)).xyz;
    output.vViewTangent = normalize(mul(float4(_in.vTangent, 0.f), g_matWV)).xyz;
    output.vViewBinormal = normalize(mul(float4(_in.vBinormal, 0.f), g_matWV)).xyz;

    return output;
}

VS_OUT VS_Std3D_Deferred_Inst(VTX_IN_INST _in)
{
    VS_OUT output = (VS_OUT) 0.f;

    if (g_iAnim)
    {
        Skinning(_in.vPos, _in.vTangent, _in.vBinormal, _in.vNormal, _in.vWeights, _in.vIndices, _in.iRowIndex);
    }

    output.vPosition = mul(float4(_in.vPos, 1.f), _in.matWVP);
    output.vUV = _in.vUV;

    output.vViewPos = mul(float4(_in.vPos, 1.f), _in.matWV);
    output.vViewNormal = normalize(mul(float4(_in.vNormal, 0.f), _in.matWV)).xyz;
    output.vViewTangent = normalize(mul(float4(_in.vTangent, 0.f), _in.matWV)).xyz;
    output.vViewBinormal = normalize(mul(float4(_in.vBinormal, 0.f), _in.matWV)).xyz;

    return output;
}

struct PS_OUT
{
    float4 vColor : SV_Target0;
    float4 vNormal : SV_Target1;
    float4 vTangent : SV_Target2;
    float4 vPosition : SV_Target3;
    float4 vEmissive : SV_Target4;
    float4 vData : SV_Target5;
    //float4 vDiffuse     : SV_Target5;
    //float4 vSpecular    : SV_Target6;
};

#define bPerlinNoiseUse    g_int_1

PS_OUT PS_Std3D_Deferred(VS_OUT _in)
{
    PS_OUT output = (PS_OUT) 0.f;
   
    output.vColor.a = 1.0f;
    
    float3 vViewNormal = _in.vViewNormal;

    float2 derivX = ddx(_in.vUV);
    float2 derivY = ddy(_in.vUV);
    float2 moveUV = _in.vUV;
    float4 vEmissiveCoeff = (float4) 0.f;

    if (g_iTexAnim)
    {
        if (g_vTexFlowDir.x > 0.0f)
        {
            moveUV.x = _in.vUV.x + (g_AccTime * g_fTexFlowSpeed) * g_vTexFlowDir.x;

        }
        if (g_vTexFlowDir.y > 0.0f)
        {
            moveUV.y = _in.vUV.y + (g_AccTime * g_fTexFlowSpeed) * g_vTexFlowDir.y;
        }

        if (g_btex_7)
        {
            float2 vNoiseUV = float2(_in.vUV);
            float4 vNoise = g_tex_7.Sample(g_sam_anti_0, vNoiseUV);

            vNoise = (vNoise - 0.5f) * 0.02f;

            moveUV += vNoise.r;
        }

        moveUV.x = moveUV.x - floor(moveUV.x);
        moveUV.y = moveUV.y - floor(moveUV.y);
    }

    if (g_btex_0)
    {

        if (g_btex_0_flow)
        {
            //output.vColor = g_tex_0.Sample(g_sam_anti_0, moveUV);
            output.vColor = g_tex_0.SampleGrad(g_sam_anti_0, float3(moveUV, 1.f), derivX, derivY);
        }
        else
        {
            output.vColor = g_tex_0.Sample(g_sam_anti_0, _in.vUV);
        }

        if (output.vColor.a <= 0.f)
            discard;
    }
    
    if (g_btex_1)
    {
        float3 vNormal = (float3) 0.f;
        if (g_btex_1_flow)
        {
            vNormal = g_tex_1.Sample(g_sam_anti_0, moveUV);
        }
        else
        {
            vNormal = g_tex_1.Sample(g_sam_anti_0, _in.vUV);
        }  
        vNormal = vNormal * 2.f - 1.f;
        
        float3x3 vRotateMat =
        {
            _in.vViewTangent,
            _in.vViewBinormal,
            _in.vViewNormal
        };
        
        vViewNormal = normalize(mul(vNormal, vRotateMat));
        output.vNormal = float4(vViewNormal, 1.f);
    }
    else
    {
        //float3x3 vRotateMat =
        //{
        //    _in.vViewTangent,
        //    _in.vViewBinormal,
        //    _in.vViewNormal
        //};

        //vViewNormal = normalize(mul(_in.vViewNormal, vRotateMat));
        vViewNormal = _in.vViewNormal;
        output.vNormal = float4(vViewNormal, 1.f);
    }

    if (g_btex_3)
    {
        if (g_btex_3_flow)
        {
            if (g_btex_3_emis)
                output.vEmissive += g_tex_3.Sample(g_sam_anti_0, moveUV);
            else
                output.vColor += g_tex_3.Sample(g_sam_anti_0, moveUV);
        }
        else
        {
            if (g_btex_3_emis)
                output.vEmissive += g_tex_3.Sample(g_sam_anti_0, _in.vUV);
            else
                output.vColor += g_tex_3.Sample(g_sam_anti_0, _in.vUV);
        }
    }

    if (g_btex_4)
    {
        if (g_btex_4_flow)
        {
            if (g_btex_4_emis)
                output.vEmissive += g_tex_4.Sample(g_sam_anti_0, moveUV);
            else
                output.vColor += g_tex_4.Sample(g_sam_anti_0, moveUV);
        }
        else
        {
            if (g_btex_4_emis)
                output.vEmissive += g_tex_4.Sample(g_sam_anti_0, _in.vUV);
            else
                output.vColor += g_tex_4.Sample(g_sam_anti_0, _in.vUV);
        }
    }

    if (g_btex_5)
    {
        if (g_btex_5_flow)
        {
            if (g_btex_5_emis)
                output.vEmissive += g_tex_5.Sample(g_sam_anti_0, moveUV);
            else
                output.vColor += g_tex_5.Sample(g_sam_anti_0, moveUV);
        }
        else
        {
            if (g_btex_5_emis)
                output.vEmissive += g_tex_5.Sample(g_sam_anti_0, _in.vUV);
            else
                output.vColor += g_tex_5.Sample(g_sam_anti_0, _in.vUV);
        }
    }

    if (g_btex_6)
    {
        //if (g_btex_6_flow)
        //{
        //    if (g_btex_6_emis)
        //        output.vEmissive += g_tex_6.Sample(g_sam_anti_0, moveUV);
        //    else
        //        output.vColor += g_tex_6.Sample(g_sam_anti_0, moveUV);
        //}
        //else
        //{
	        //if (g_btex_6_emis)
	        //    output.vEmissive += g_tex_6.Sample(g_sam_anti_0, _in.vUV);
	        //else
	        //    output.vColor += g_tex_6.Sample(g_sam_anti_0, _in.vUV);
        //}

        //if (paperBurn)
        //{
        //    output.vEmissive += g_tex_6.Sample(g_sam_anti_0, _in.vUV);
        //}
        //if (g_btex_6_emis)
            
    }
    if (g_btex_2)
    {
        if (g_btex_2_flow)
            vEmissiveCoeff = g_tex_2.Sample(g_sam_anti_0, moveUV);
        else
            vEmissiveCoeff = g_tex_2.Sample(g_sam_anti_0, _in.vUV);
        
        if (vEmissiveCoeff.r > 0.0f)
        {
            output.vEmissive.r += output.vColor * vEmissiveCoeff.r * g_fEmisCoeff;
        }
        if (vEmissiveCoeff.g > 0.0f)
        {
            output.vEmissive += vEmissiveCoeff.g * g_fEmisCoeff;
        }
        if (vEmissiveCoeff.b > 0.0f)
        {
            output.vEmissive += output.vColor * vEmissiveCoeff.b * g_fEmisCoeff;
        }
    }

    if (paperBurn)
    {
        output.vColor = PaperBurn(output.vColor, _in.vUV, g_tex_6, output.vEmissive);
    }
  //  if (g_fEmisCoeff)
		//output.vEmissive *= g_fEmisCoeff;

    
    output.vPosition = float4(_in.vViewPos, 1.f);
    output.vData = float4(0.f, 0.f, 0.f, 1.f);
    return output;
}

#endif