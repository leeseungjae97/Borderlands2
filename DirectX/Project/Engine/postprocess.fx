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
	// ÇÈ¼¿ ÁÂÇ¥
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

    if (g_btex_1)
    {

        float2 vNoiseUV = _in.vUV;
        if(g_vTexFlowDir.x != 0.0f)
        {
            vNoiseUV.x = _in.vUV.x + (g_AccTime * g_vTexFlowDir.x);
        }
        if (g_vTexFlowDir.y != 0.0f)
        {
            vNoiseUV.y = _in.vUV.y + (g_AccTime * g_vTexFlowDir.y);
        }

        float4 vNoise = g_tex_1.Sample(g_sam_anti_0, vNoiseUV);

        vNoise = (vNoise - 0.5f) * 0.02f;

        vUV += vNoise.r;
    }

    float4 vColor = g_tex_0.Sample(g_sam_anti_0, vUV);

    //vColor.r *= 2.f;
	
    return vColor;
}


static const float Weight[24] =
{
    0.0243, 0.0469, 0.0797, 0.1216, 0.1693, 0.2188,
    0.2663, 0.3074, 0.3381, 0.3544, 0.3544, 0.3381,
    0.3074, 0.2663, 0.2188, 0.1693, 0.1216, 0.0797,
    0.0469, 0.0243, 0.0120, 0.0061, 0.0031, 0.0016
};
struct VS_SCREEN_IN
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;

};
struct VS_SCREEN_OUT
{
    float4 vPos : SV_Position;
    float2 vUV : TEXCOORD;
};
struct PS_BLUR_OUT
{
    float4 vEmissive : SV_Target0;
    //float4 vDiffuse: SV_Target1;
};
struct PS_BLOOM_OUT
{
	
};

VS_SCREEN_OUT VS_Screen(VS_SCREEN_IN _In)
{
    VS_SCREEN_OUT _Out = (VS_SCREEN_OUT) 0.f;
    _Out.vPos = float4(_In.vPos.xyz * 2.f, 1.f);
    _Out.vUV = _In.vUV;
    return _Out;
}

#define EmissiveTargetTex g_tex_0
//#define DiffuseTargetTex g_tex_1
//#define SpecularTargetTex g_tex_1

float4 PS_BlurX(VS_SCREEN_OUT _In) : SV_Target
{
    float4 vOut = (float4) 0.f;

    float2 t = _In.vUV;
    float2 uv = 0;
    float tu = 1.f / g_Resolution.x;
    for (int i = -12; i < 12; ++i)
    {
        uv = t + float2(tu * i, 0);
        vOut += Weight[12 + i] * EmissiveTargetTex.Sample(g_sam_anti_0, uv);
        //vOut.vDiffuse += Weight[12 + i] * DiffuseTargetTex.Sample(g_sam_anti_0, uv);
        //vOut.vSpecular += Weight[6 + i] * SpecularTargetTex.Sample(g_sam_anti_0, uv);
    }
    vOut /= 6.2108f;
    //vOut.vDiffuse /= 6.2108f;
    //vOut.vSpecular /= 6.2108;

    return vOut;

}
float4 PS_BlurY(VS_SCREEN_OUT _In) : SV_Target
{
    float4 vOut = (float4) 0.f;

    float2 t = _In.vUV;
    float2 uv = 0;
    float tu = 1.f / (g_Resolution.y / 2.f);
    for (int i = -12; i < 12; ++i)
    {
        uv = t + float2(0, tu * i);
        vOut += Weight[12 + i] * EmissiveTargetTex.Sample(g_sam_anti_0, uv);
        //vOut.vDiffuse += Weight[12 + i] * DiffuseTargetTex.Sample(g_sam_anti_0, uv);
        //vOut.vSpecular += Weight[6 + i] * SpecularTargetTex.Sample(g_sam_anti_0, uv);
    }
    vOut /= 6.2108f;
    //vOut.vDiffuse /= 6.2108f;
    //vOut.vSpecular /= 6.2108;

    return vOut;
}

PS_BLUR_OUT PS_GaussianBlur(VS_SCREEN_OUT _In)
{
    PS_BLUR_OUT vOut = (PS_BLUR_OUT) 0.f;
    
    vOut.vEmissive = GaussianBlur(EmissiveTargetTex, _In.vUV);

    return vOut;
}

#define HDRTargetTex g_tex_0
#define BlurredTargetTex g_tex_1
#define OriginTargetTex g_tex_2

float4 PS_Bloom(VS_SCREEN_OUT _In) : SV_Target
{
    float4 vOut = (float4)0.0f;

    float4 vHDRColor = HDRTargetTex.Sample(g_sam_anti_0, _In.vUV);
    float4 vBlurred = BlurredTargetTex.Sample(g_sam_anti_0, _In.vUV);
    float4 vOrigin = OriginTargetTex.Sample(g_sam_anti_0, _In.vUV);

    float4 vBloom = pow(pow(abs(vBlurred), 2.2f) + pow(abs(vOrigin), 2.2f), 1.f / 2.2f);

    vOut = pow(abs(vHDRColor), 2.2f);
    vBloom = pow(abs(vBloom), 2.2f);

    vOut += vBloom;
    //vOut =lerp(vOut, vBloom, 0.5f);

    return pow(abs(vOut), 1.f / 2.2f);
}

#define BloomedHDRTargetTex g_tex_0
float4 PS_ToneMapping(VS_SCREEN_OUT _In) : SV_Target
{
    float4 vHDRColor = BloomedHDRTargetTex.Sample(g_sam_anti_0, _In.vUV);
    float3 vToneMappedColor = ACESToneMapping(vHDRColor.xyz);

    //return float4(vToneMappedColor, 1.f);
    return float4(pow(vToneMappedColor, 1 / 2.2f), vHDRColor.a);
}

static float mask[9] =
{
    -1, -1, -1,
    -1, 8, -1,
    -1, -1, -1
};
static float coord[3] = { -1, 0, 1 };

#define DiffuseTargetTex g_tex_0
#define HDRTargetTex g_tex_1

float4 PS_Laplacian(VS_SCREEN_OUT _In) : SV_Target
{
    float4 vColor = (float4) 0.f;
    float4 vRet = HDRTargetTex.Sample(g_sam_anti_0, _In.vUV);

    for (int i = 0; i < 9; ++i)
    {
        vColor -= mask[i] * (DiffuseTargetTex.Sample(g_sam_anti_0, _In.vUV + float2(coord[i % 3] / g_Resolution.x, coord[i / 3] / g_Resolution.y)));
    }
    float gray = dot(vColor, float4(1.0f, 1.0f, 1.0f, 1.0f));

    float threshold = 0.1f;
    float outlineIntensity = 10.f;
    
    float4 vImpactColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    float4 outline = vImpactColor * saturate((gray - threshold) * outlineIntensity);
    
    vRet.rgb = lerp(vRet.rgb, outline.rgb, outline.a);
    vRet.a = 1.f;

    return vRet;
}
#endif