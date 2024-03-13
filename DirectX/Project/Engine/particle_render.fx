#ifndef _PARTICLE_RENDER
#define _PARTICLE_RENDER

#include "value.fx"
#include "struct.fx"


// ========================
// Particle Render Shader
// mesh : RectMesh

// Parameter
// g_int_0 : Particle Index
// =========================

StructuredBuffer<tParticle>         ParticleBuffer : register(t20);
StructuredBuffer<tParticleModule>   ParticleModuleData : register(t21);
#define ModuleData                  ParticleModuleData[0]

struct VS_IN
{
    float3 vPos : POSITION;    
    uint iInstID : SV_InstanceID;
};

struct VS_OUT
{
    float3 vPos : POSITION;
    uint iInstID : SV_InstanceID;
};

VS_OUT VS_ParticleRender(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;      
     
    output.vPos = _in.vPos;
    output.iInstID = _in.iInstID;
    
    return output;
}

struct GS_OUT
{
    float4  vPosition : SV_Position;
    float2  vUV : TEXCOORD;
    uint    iInstID : SV_InstanceID;
};

[maxvertexcount(6)]
void GS_ParticleRender (point VS_OUT _in[1], inout TriangleStream<GS_OUT> _outstream)
{    
    uint id = _in[0].iInstID;
    
    //if (0 == ParticleBuffer[id].Active)
    //    return;

    float3 vParticleViewPos = mul(float4(ParticleBuffer[id].vWorldPos.xyz, 1.f), g_matView).xyz;
    float3 vParticleScale = ParticleBuffer[id].vWorldScale.xyz * ParticleBuffer[id].ScaleFactor;
   
    float3 NewPos[4] =
    {
        float3(-vParticleScale.x / 2.f, +vParticleScale.y / 2.f, 0.f),
        float3(+vParticleScale.x / 2.f, +vParticleScale.y / 2.f, 0.f),
        float3(+vParticleScale.x / 2.f, -vParticleScale.y / 2.f, 0.f),
        float3(-vParticleScale.x / 2.f, -vParticleScale.y / 2.f, 0.f)
    };
    
    
    if (ModuleData.Render)
    {        
        if (ModuleData.VelocityScale)
        {
            // 현재 파티클의 속력을 알아낸다.
            float fCurSpeed = length(ParticleBuffer[id].vVelocity.xyz);            
            if (ModuleData.vMaxSpeed < fCurSpeed)
                fCurSpeed = ModuleData.vMaxSpeed;
            
            // 최대속도 대비 현재 속도의 비율을 구한다.
            float fRatio = saturate(fCurSpeed / ModuleData.vMaxSpeed);            
          
            // 비율에 맞는 크기변화량을 구한다.
            float3 vDefaultScale = float3(1.f, 1.f, 1.f);
            float3 fScale = vDefaultScale + (ModuleData.vMaxVelocityScale.xyz - vDefaultScale) * fRatio;
                      
            NewPos[0] = NewPos[0] * fScale;
            NewPos[3] = NewPos[3] * fScale;            
        }
        
        
        if (ModuleData.VelocityAlignment)
        {
            // 파티클 월드 기준 속도를 View 공간으로 변환
            float3 vVelocity = normalize(ParticleBuffer[id].vVelocity);
            vVelocity = mul(float4(vVelocity, 0.f), g_matView).xyz;
                       
            // 파티클 Right 방향과 이동 방향을 내적해서 둘 사이의 각도를 구한다.
            float3 vRight = float3(1.f, 0.f, 0.f);
            float fTheta = acos(dot(vRight, vVelocity));
            
            // 내적의 결과가 코사인 예각을 기준으로 하기 때문에, 2파이 에서 반대로 뒤집어 준다.
            if (vVelocity.y < vRight.y)
            {
                fTheta = (2.f * 3.1415926535f) - fTheta;
            }
            
            // 구한 각도로 Z 축 회전 행렬을 만든다.
            float3x3 matRotZ =
            {
                cos(fTheta),  sin(fTheta),      0,
                -sin(fTheta), cos(fTheta),      0,
                          0,            0,    1.f,
            };
            
            // 4개의 정점을 회전시킨다.
            for (int i = 0; i < 4; ++i)
            {
                NewPos[i] = mul(NewPos[i], matRotZ);
            }
        }        
    }
    
    
    GS_OUT output[4] = { (GS_OUT) 0.f, (GS_OUT) 0.f, (GS_OUT) 0.f, (GS_OUT) 0.f };
    
    output[0].vPosition = mul(float4(NewPos[0] + vParticleViewPos, 1.f), g_matProj);
    output[0].vUV = float2(0.f, 0.f);
    output[0].iInstID = id;
    
    output[1].vPosition = mul(float4(NewPos[1] + vParticleViewPos, 1.f), g_matProj);
    output[1].vUV = float2(1.f, 0.f);
    output[1].iInstID = id;
    
    output[2].vPosition = mul(float4(NewPos[2] + vParticleViewPos, 1.f), g_matProj);
    output[2].vUV = float2(1.f, 1.f);
    output[2].iInstID = id;
    
    output[3].vPosition = mul(float4(NewPos[3] + vParticleViewPos, 1.f), g_matProj);
    output[3].vUV = float2(0.f, 1.f);
    output[3].iInstID = id;
    
    
    // 정점 생성
    _outstream.Append(output[0]);
    _outstream.Append(output[1]);
    _outstream.Append(output[2]);
    _outstream.RestartStrip();
    
    _outstream.Append(output[0]);
    _outstream.Append(output[2]);
    _outstream.Append(output[3]);
    _outstream.RestartStrip();
}

struct PS_OUT
{
    float4 vColor : SV_Target0;
    float4 vNormal : SV_Target1;
    float4 vPosition : SV_Target2;
    float4 vEmissive : SV_Target3;
    float4 vData    : SV_Target4;
    float4 vDiffuse : SV_Target5;
    float4 vSpecular : SV_Target6;
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

#include "value.fx"
#include "func.fx"

float4 PS_ParticleRender(GS_OUT _in) : SV_Target
{   
    
    float4 vOutColor = float4(1.f, 1.f, 1.f, 1.f);
    
    if(g_btex_0)
    {
        vOutColor = g_tex_0.Sample(g_sam_anti_0, _in.vUV);        
        //vOutColor.xyz *= ParticleBuffer[_in.iInstID].vColor.rgb;
    }
    if (g_btex_1)
    {
        //vOutColor.vEmissive += g_tex_0.Sample(g_sam_anti_0, _in.vUV);
        //vOutColor.vEmissive += ParticleBuffer[_in.iInstID].vColor;
    }
    //vOutColor.vNormal = float4(vViewNormal, 1.f);
    //vOutColor.vPosition = float4(_in.vViewPos, 1.f);
    //vOutColor.vData = float4(0.f, 0.f, 0.f, 1.f);
    
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
        }
        else
        {
            if (g_btex_0_flow)
            {
                vOutColor = g_tex_0.Sample(g_sam_anti_0, moveUV);
            }
            else
                vOutColor = g_tex_0.Sample(g_sam_anti_0, _in.vUV);
        }
        
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
    tLightColor lightColor = (tLightColor) 0.f;
    float fSpecPow = 0.f;

    if (bPerlinNoiseUse)
    {
        float4 vPerlineNoiseColor = (float4) 0.f;
        PerlinNoiseFire(vPerlineNoiseColor, _in.vUV);
        vOutColor.xyz = vPerlineNoiseColor.xyz - vOutColor.xyz;

        vEmissiveCoeff += vOutColor;
    }

    vOutColor.xyz = (vOutColor.xyz * lightColor.vDiffuse.xyz)
                    + (vOutColor.xyz * lightColor.vAmbient.xyz)
                    + saturate(g_Light3DBuffer[0].Color.vDiffuse) * fSpecPow * SPEC_COEFF
					+ vEmissiveCoeff.xyz;

    if (paperBurn)
    {
        vOutColor = PaperBurn(vOutColor, _in.vUV, g_tex_6);
    }

    return vOutColor;
}


#endif
