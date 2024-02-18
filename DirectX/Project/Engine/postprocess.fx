#ifndef _POSTPROCESS
#define _POSTPROCESS

#include "value.fx"
#include "func.fx"

struct VS_IN
{
	float3 vLocalPos : POSITION;
	float2 vUV : TEXCOORD;	
};

struct VS_OUT
{
	float4 vPosition : SV_Position; // System Value
	float2 vUV : TEXCOORD;
};


// ============================
// GrayShader
// mesh : RectMesh
// Domain : DOMAIN_POSTPROCESS
// g_tex_0 : RederTarget Copy Texture
// ============================
VS_OUT VS_GrayShader(VS_IN _in)
{
	VS_OUT output = (VS_OUT)0.f;

	//output.vPosition = float4(_in.vLocalPos * 2.f, 1.f);	
    output.vPosition = mul(float4(_in.vLocalPos, 1.f), g_matWVP);	
	output.vUV = _in.vUV;

	return output;
}

float4 PS_GrayShader(VS_OUT _in) : SV_Target
{
	// 픽셀 좌표
    // _in.vPosition.xy;
    float2 vScreenUV = _in.vPosition.xy / g_Resolution;
    
    //float4 vColor = g_tex_0.Sample(g_sam_anti_0, _in.vUV);	
    float4 vColor = g_tex_0.Sample(g_sam_anti_0, vScreenUV);
	
    float vAver = (vColor.r + vColor.g + vColor.b) / 3.f;

    vColor = float4(vAver, vAver, vAver, 1.f);
  
	return vColor;
}


// ============================
// Distortion Shader
// mesh : RectMesh
// Domain : DOMAIN_POSTPROCESS
// g_tex_0 : RenderTarget Copy Texture
// g_tex_1 : Noise Texture
// ============================
VS_OUT VS_Distortion(VS_IN _in)
{
	VS_OUT output = (VS_OUT)0.f;

	output.vPosition = mul(float4(_in.vLocalPos, 1.f), g_matWVP);	
	output.vUV = _in.vUV;

	return output;
}

//float4 PS_Distortion(VS_OUT _in) : SV_Target
//{
//    float2 vUV = _in.vPosition.xy / g_Resolution;
		
	
//    float fChange = cos(((vUV.x - g_AccTime * 0.05f) / 0.15f) * 2 * 3.1415926535f) * 0.05f;

//    vUV.y += fChange;

//    float4 vColor = g_tex_0.Sample(g_sam_anti_0, vUV);
//	//vColor.r *= 2.f;

//    return vColor;
//}

float4 PS_Distortion(VS_OUT _in) : SV_Target
{
    float2 vUV = _in.vPosition.xy / g_Resolution;
	
	// Noise Texture 가 세팅이 되어 있다면
    if (g_btex_1)
    {
        float2 vNoiseUV = float2(_in.vUV.x - (g_AccTime * 0.2f), _in.vUV.y);
        float4 vNoise = g_tex_1.Sample(g_sam_anti_0, vNoiseUV);

        vNoise = (vNoise - 0.5f) * 0.02f;

        vUV += vNoise.r;
    }

    float4 vColor = g_tex_0.Sample(g_sam_anti_0, vUV);

    //vColor.r *= 2.f;
	
    return vColor;
}


VS_OUT VS_BloomCurve(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;

    output.vPosition = mul(float4(_in.vLocalPos, 1.f), g_matWVP);
    output.vUV = _in.vUV;

    return output;
}

float4 PS_BloomCurve(VS_IN _in) : SV_Target
{
    float3 color = g_tex_0.Sample(g_sam_linear_2, _in.vUV).xyz;
    float its = dot(color, float3(0.3f, 0.3f, 0.3f));
    float bloomIts = BloomCurve(its);
    float3 bloomColor = color * bloomIts / its;

    return float4(bloomColor, 1.0f);
}


struct VertexPosTexCoordIn
{
    float4 position : POSITION;
    float2 tex : TEXCOORD;
};

struct VertexPosTexCoordOut
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD;
};

#define Tex0 g_tex_0
#define Tex1 g_tex_1

#define Coeff g_float_0

// vertex shader
VertexPosTexCoordOut VSMain(VertexPosTexCoordIn v)
{
    VertexPosTexCoordOut output;
    output.position = v.position;
    output.tex = v.tex;

    return output;
}

// pixel shader
float4 PSMain(VertexPosTexCoordOut p) : SV_TARGET
{
    // output: tex0 + coefficient * tex1
    return mad(Coeff, Tex0.Sample(g_sam_linear_2, p.tex), Tex1.Sample(g_sam_linear_2, p.tex));
}



#endif