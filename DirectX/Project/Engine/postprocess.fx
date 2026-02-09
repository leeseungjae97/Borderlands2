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
    VS_OUT output = (VS_OUT) 0.f;

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
    VS_OUT output = (VS_OUT) 0.f;

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
    //float2 vUV = _in.vUV;

    if (g_btex_1)
    {

        float2 vNoiseUV = _in.vUV;
        if (g_vTexFlowDir.x != 0.0f)
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
static const float Weight2[24] =
{
    0.0033, 0.0074, 0.0152, 0.0273, 0.0447, 0.0668,
    0.0917, 0.1159, 0.1345, 0.1432, 0.1391, 0.1230, 
    0.0993, 0.0735, 0.0509, 0.0335, 0.0206, 0.0120,
    0.0067, 0.0034, 0.0016, 0.0007, 0.0003, 0.0001

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
    float4 vEmissiveX : SV_Target0;
    float4 vEmissiveY : SV_Target1;
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


float4 PS_BlurX(VS_SCREEN_OUT _In) : SV_Target
{
    float4 vOut = (float4) 0.f;

    float2 t = _In.vUV;
    float2 uv = 0;
    //float tu = 1.f / (g_Resolution.x / 5.f);
    float tu = 1.f / (g_Resolution.x / 2.f);
    float weightSum = 0.f;
    for (int i = -12; i < 12; ++i)
    {
        uv = t + float2(tu * i, 0);
        vOut += Weight[12 + i] * EmissiveTargetTex.Sample(g_sam_anti_0, uv);
        weightSum += Weight[12 + i];

    }
    //vOut /= weightSum;
    //vOut /= 4;
    vOut /= 6.2108f;
    //vOut /= 2;

    return vOut;
}

#define BlurHTargetTex g_tex_1

float4 PS_BlurY(VS_SCREEN_OUT _In) : SV_Target
{
    float4 vOut = (float4) 0.f;

    float2 t = _In.vUV;
    float2 uv = 0;
    float tu = 1.f / (g_Resolution.y / 2.f);
    //float tu = 1.f / (g_Resolution.y / 5.f);
    float weightSum = 0.f;
    for (int i = -12; i < 12; ++i)
    {
        uv = t + float2(0, tu * i);
        vOut += Weight[12 + i] * BlurHTargetTex.Sample(g_sam_anti_0, uv);
        weightSum += Weight[12 + i];
    }
    vOut /= 6.2108f;
    //vOut /= weightSum;
    //vOut /= 4;
    //vOut += BlurHTargetTex.Sample(g_sam_anti_0, uv);

    return vOut;
}

PS_BLUR_OUT PS_GaussianBlur(VS_SCREEN_OUT _In)
{
    PS_BLUR_OUT vOut = (PS_BLUR_OUT) 0.f;
    
    //vOut.vEmissive = GaussianBlur(EmissiveTargetTex, _In.vUV);

    return vOut;
}

#define TargetTex g_tex_0
#define BlurredTargetTex g_tex_1
#define OriginTargetTex g_tex_2

float4 PS_Bloom(VS_SCREEN_OUT _In) : SV_Target
{
    float4 vOut = (float4) 0.0f;

    // Linear
    float4 vHDRColor = TargetTex.Sample(g_sam_anti_0, _In.vUV);
    // Linear
    float4 vBlurred = BlurredTargetTex.Sample(g_sam_anti_0, _In.vUV);
    // Linear
    float4 vOrigin = OriginTargetTex.Sample(g_sam_anti_0, _In.vUV);
    
    float fGamma = 2.2f;
    float fRGamma = 2.2f;
    
    //float4 vBloom = pow(pow(abs(vBlurred), 2.2f) + pow(abs(vOrigin), 2.2f), 1.f / 2.2f);
    
    //float4 vBloom = pow(abs(vBlurred), fGamma) + vOrigin;
    //float4 vBloom = pow(pow(abs(vBlurred), fGamma) + vOrigin, 1.f / fRGamma);
    
    float4 vBloom = pow(pow(abs(vBlurred), 2.2f) + pow(abs(vOrigin), 2.2f), 1.f / 2.2f);
    // Linear
    //float4 vBloom = vBlurred + vOrigin;
    //float4 vBloom = pow(abs(vBlurred), 1 / 2.2f) + pow(abs(vOrigin), 1 / 2.2f);
    //float4 vBloom = pow(vBloom, fGamma);
    //vBloom = pow(vBloom, 1 / fRGamma);
    
    vOut = vHDRColor;
    // Linear에 
    vOut += vBloom;
    return vOut;
}

#define BloomedHDRTargetTex g_tex_0
#define UseToneMapping g_int_0
float4 PS_ToneMapping(VS_SCREEN_OUT _In) : SV_Target
{
    float4 vHDRColor = BloomedHDRTargetTex.Sample(g_sam_anti_0, _In.vUV);
    // 1. Exposure 조절 (너무 어둡다면 이 값을 올리세요)
    float3 vColor = vHDRColor.xyz * 1.0f;
    // 2. ACESToneMapping
    float3 vToneMappedColor = ACESToneMapping(vColor);
    
    // 3. Gamma Correction (필수: 모니터 출력 규격에 맞춤)
    vToneMappedColor = pow(vToneMappedColor, 1.0f / 2.2f);
    return float4(vToneMappedColor, vHDRColor.a);
}

#define Fade g_float_0

#define OriginTex g_tex_0
float4 PS_Fade(VS_SCREEN_OUT _In) : SV_Target
{
    float4 vOutColor = OriginTex.Sample(g_sam_anti_0, _In.vUV);
    //vOutColor += 0.1f;
    //vOutColor.a = Fade;

    if (vOutColor.r > 0.0f)
        vOutColor.r -= Fade;

    if (vOutColor.g > 0.0f)
        vOutColor.g -= Fade;

    if (vOutColor.b > 0.0f)
        vOutColor.b -= Fade;

    return vOutColor;
}

static float mask[9] =
{
    -1, -1, -1,
    -1, 8, -1,
    -1, -1, -1
};
static float coord[3] = { -2, 0, 2 };
static float mapCoord[3] = { -5, 0, 5 };

#define NormalTargetTex g_tex_0
#define TargetTex g_tex_1

#define MapLaplacian g_int_0
float4 PS_Laplacian(VS_SCREEN_OUT _In) : SV_Target
{
    float4 vColor = (float4) 0.f;
    float4 vRet = (float4) 0.f;

    if (MapLaplacian != 1)
    {
        vRet = TargetTex.Sample(g_sam_anti_0, _In.vUV);
    }

    for (int i = 0; i < 9; ++i)
    {
        if (MapLaplacian == 1)
            vColor -= mask[i] * (NormalTargetTex.Sample(g_sam_anti_0, _In.vUV + float2(mapCoord[i % 3]
            / g_Resolution.x, mapCoord[i / 3] / g_Resolution.y)));
        else
            vColor -= mask[i] * (NormalTargetTex.Sample(g_sam_anti_0, _In.vUV + float2(coord[i % 3]
			/ g_Resolution.x, coord[i / 3] / g_Resolution.y)));
    }
    float gray = 0.f;
    float threshold = 0.5f;
    
    float4 vImpactColor = (float4) 0.f;
    
    // 외곽선인 경우 vColor의 값이
    if (MapLaplacian == 1)
    {
        // vColor의 RGB값과 같아짐
        // gray는 vColor의 합
        gray = dot(vColor, float4(1.0f, 1.0f, 1.0f, 1.0f));
        // 외곽선의 색 지정
        vImpactColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
    }
    else
    {
        gray = dot(vColor, float4(1.0f, 1.0f, 1.0f, 1.0f));
        // 외곽선의 색 지정
        vImpactColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    }
    // 보정치 0.5
    float4 outline = vImpactColor * saturate(gray - threshold);
    if (MapLaplacian == 1)
    {
        // 선형 보간을 통해 색을 입혀줌
        // 맵의 경우 외곽선을 제외한 색이 필요
        vRet.rgb = float3(43.f / 255.f, 76.f / 255.f, 93.f / 255.f);
        vRet.rgb = lerp(vRet.rgb, outline.rgb, outline.a);
        vRet.a = 1.f;
    }
    else
    {
        vRet.rgb = lerp(vRet.rgb, outline.rgb, outline.a);
        vRet.a = 1.f;
    }

    return vRet;
}
#endif