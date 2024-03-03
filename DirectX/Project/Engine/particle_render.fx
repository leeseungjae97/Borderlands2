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

PS_OUT PS_ParticleRender(GS_OUT _in)
{   
    PS_OUT vOutColor = (PS_OUT)0.f;
    
    if(g_btex_0)
    {
        vOutColor.vColor = g_tex_0.Sample(g_sam_anti_0, _in.vUV);        
        vOutColor.vColor.xyz = ParticleBuffer[_in.iInstID].vColor.rgb;
    }
    if (g_btex_1)
    {
        //vOutColor.vEmissive += g_tex_0.Sample(g_sam_anti_0, _in.vUV);
        vOutColor.vEmissive += ParticleBuffer[_in.iInstID].vColor;
    }
    //vOutColor.vNormal = float4(vViewNormal, 1.f);
    //vOutColor.vPosition = float4(_in.vViewPos, 1.f);
    //vOutColor.vData = float4(0.f, 0.f, 0.f, 1.f);
    return vOutColor;
}


#endif
