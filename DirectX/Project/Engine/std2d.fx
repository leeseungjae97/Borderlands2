#ifndef _STD2D
#define _STD2D

#include "value.fx"
#include "func.fx"

struct VS_IN
{
    float3 vLocalPos : POSITION;    
    float2 vUV : TEXCOORD;
};

struct VS_OUT
{
    float4 vPosition : SV_Position;
    float4 vColor : COLOR;
    float2 vUV : TEXCOORD;
};

// ============================
// Std2DShader
// RasterizerState      : None
// BlendState           : Mask
// DepthStencilState    : Less
//
// Parameter
// g_int_0              : AnimUse
// g_vec2_0             : AnimAtlas LeftTop
// g_vec2_1             : AnimAtlas Slice
//
// g_tex_0              : Output Texture
// ============================
VS_OUT VS_Std2D(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    output.vPosition = mul(float4(_in.vLocalPos, 1.f), g_matWVP);
    output.vUV = _in.vUV;
        
    return output;
}

// 레스터라이저 스테이트
float4 PS_Std2D(VS_OUT _in) : SV_Target
{
    float4 vOutColor = (float4) 0.f;            
        
    if (g_btex_0)
    {
        vOutColor = g_tex_0.Sample(g_sam_anti_0, _in.vUV);
    }

    if (0.f == vOutColor.a)
        discard;

    if(g_int_2 == 1)
        vOutColor = float4(1.f, 0.f, 0.f, 1.f);
    
    return vOutColor;
}

#define UVY g_float_0
#define Reverse g_int_0
#define IsCustomAlpha g_int_1
#define Alpha g_float_1
float4 PS_AdjustStd2D(VS_OUT _in) : SV_Target
{
    float4 vOutColor = (float4) 0.f;
        
    if (g_btex_0)
    {
        vOutColor = g_tex_0.Sample(g_sam_anti_0, _in.vUV);
    }

    if (0.f == vOutColor.a)
        discard;

    if (Reverse)
    {
        if (_in.vUV.x < UVY)
            discard;
    }else
    {
        if (_in.vUV.x > 1.f - UVY)
            discard;
    }
    if (IsCustomAlpha)
        vOutColor.a = Alpha;
    
    return vOutColor;
}

#define IsCustomAlpha g_int_1
#define Alpha g_float_1
float4 PS_SpriteFontStd2D(float4 color : COLOR0, float2 texCoord : TEXCOORD0) : SV_Target0
{
    float4 vOutColor = (float4) 0.f;
        
    if (g_btex_0)
    {
        vOutColor = g_tex_0.Sample(g_sam_anti_0, texCoord);
    }

    if (0.f == vOutColor.a)
        discard;

    //if (Reverse)
    //{
    //    if (texCoord.x < UVY)
    //        discard;
    //}
    //else
    //{
    //    if (texCoord.x > 1.f - UVY)
    //        discard;
    //}
    if (IsCustomAlpha)
        vOutColor.a = Alpha;
    
    return vOutColor;
}






// ======================================
// Std2DLightShader
// RasterizerState      : None
// BlendState           : Mask
// DepthStencilState    : Less
//
// Parameter
#define bAnimUse        g_int_0
#define SpriteLeftTop   g_vec2_0
#define SpriteSize      g_vec2_1
#define SpriteOffset    g_vec2_2
#define SheetSize       g_vec2_3


//float2 SpriteLeftTop;
//float2 SpriteSize;
//float2 SpriteOffset;
//float2 AtlasSize;
//float2 SpriteOffsetOfCenterPos;
    
//uint animationType;
//float mAlpha;

// g_tex_0              : Output Texture
// g_tex_1              : Nomal Texture
// ======================================

struct VS_Light_IN
{
    float3 vLocalPos : POSITION;
    float2 vUV : TEXCOORD;
};

struct VS_Light_OUT
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;
    float3 vWorldPos : POSITION;
};



VS_Light_OUT VS_Std2DLight(VS_Light_IN _in)
{
    VS_Light_OUT output = (VS_Light_OUT) 0.f;
    
    output.vPosition = mul(float4(_in.vLocalPos, 1.f), g_matWVP);
    output.vUV = _in.vUV;
    output.vWorldPos = mul(float4(_in.vLocalPos, 1.f), g_matWorld).xyz;    
        
    return output;
}


float4 PS_Std2DLight(VS_Light_OUT _in) : SV_Target
{
    float4 vOutColor = (float4) 0.f;
       
    if(g_vec4_0.x == 3.14f)
    {
        return float4(1.f, 0.f, 0.f, 1.f);
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
            vOutColor = g_tex_0.Sample(g_sam_anti_0, _in.vUV);
        }        
    }
    else
    {
        vOutColor = float4(1.f, 0.f, 1.f, 1.f);
    }
        
    float3 vNormal = (float3)0.f;
    if(g_btex_1)
    {
        // Normal 값 추출
        vNormal = g_tex_1.Sample(g_sam_anti_0, _in.vUV).xyz;
        
        // 0 ~ 1 범위를 -1 ~ 1 로 변경
        vNormal = (vNormal * 2.f) - 1.f;
        
        // NormalTexture 좌표계는 y축과 z 축이 반대로 되어있다.
        float f = vNormal.y;
        vNormal.y = vNormal.z;               
        vNormal.z = f;
        
        // Texture 에서 추출한 Normal 방향을 월드로 변환시킨다.
        vNormal = normalize(mul(float4(vNormal, 0.f), g_matWorld)).xyz;        
    }
    
    
    
    if (0.f == vOutColor.a)
    {
        return float4(1.f, 0.f, 1.f, 1.f);
        //discard;
    }
     
    
    // Lighting 처리
    tLightColor LightColor = (tLightColor) 0.f;
    
    // vNormal 이 Zero Vector 라면
    if (dot(vNormal, vNormal) == 0.f)
    {
        CalcLight2D(_in.vWorldPos, LightColor);
    }
    else
    {
        CalcLight2D(_in.vWorldPos, vNormal, LightColor);
    }    
        
    vOutColor.rgb *= (LightColor.vDiffuse.rgb + LightColor.vAmbient.rgb);
    
    return vOutColor;
}






#endif