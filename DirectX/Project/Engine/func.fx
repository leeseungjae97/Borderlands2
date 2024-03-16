#ifndef _FUNC
#define _FUNC

#include "value.fx"

//bool PointIntersectRhombus(float2 pos, float2 scale, float2 otherPos)
//{
//    float2 vertex[4];
//    float gradient[4];
//    float intercept[4];
//    uint i = 0;
//    float2 direct[4] =
//    {
//        { -(scale.x / 2.f), 0 },
//        { 0, -(scale.y / 2.f) },
//        { (scale.x / 2.f), 0 },
//        { 0, (scale.y / 2.f) }
//    };
    
//    for (i = 0; i < 4; ++i)
//    {
//        vertex[i].x = pos.x + direct[i].x;
//        vertex[i].y = pos.y + direct[i].y;
//    }
//    for (i = 0; i < 4; ++i)
//    {
//        gradient[i] = ((vertex[i].y - vertex[(i + 1) % 4].y) / (vertex[i].x - vertex[(i + 1) % 4].x));
//        intercept[i] = vertex[i].y - gradient[i] * vertex[i].x;
//    }
//    float _y = otherPos.y;
//    float _x = otherPos.x;

//    if (gradient[0] * _x + intercept[0] < _y
//        && gradient[1] * _x + intercept[1] < _y
//        && gradient[2] * _x + intercept[2] > _y
//        && gradient[3] * _x + intercept[3] > _y)
//    {
//        return true;
//    }
//    else
//        return false;
//}

void CalcLight2D(float3 _vWorldPos, inout tLightColor _Light)
{
    for (uint i = 0; i < g_Light2DCount; ++i)
    {
        if (g_Light2DBuffer[i].LightType == 0)
        {
            _Light.vDiffuse.rgb += g_Light2DBuffer[i].Color.vDiffuse.rgb;
            _Light.vAmbient.rgb += g_Light2DBuffer[i].Color.vAmbient.rgb;
        }
        else if (g_Light2DBuffer[i].LightType == 1)
        {
            float3 vLightWorldPos = float3(g_Light2DBuffer[i].vWorldPos.xy, 0.f);
            float3 vWorldPos = float3(_vWorldPos.xy, 0.f);

            float fDistance = abs(distance(vWorldPos, vLightWorldPos));
            float fPow = saturate(1.f - (fDistance / g_Light2DBuffer[i].Radius));
        
            _Light.vDiffuse.rgb += g_Light2DBuffer[i].Color.vDiffuse.rgb * fPow;
        }
        else if (g_Light2DBuffer[i].LightType == 2)
        {
            
        }
    }
}

void CalcLight2D(float3 _vWorldPos, float3 _vWorldDir, inout tLightColor _Light)
{
    for (uint i = 0; i < g_Light2DCount; ++i)
    {
        if (g_Light2DBuffer[i].LightType == 0)
        {
            float fDiffusePow = saturate(dot(-g_Light2DBuffer[i].vWorldDir.xyz, _vWorldDir));
            _Light.vDiffuse.rgb += g_Light2DBuffer[i].Color.vDiffuse.rgb * fDiffusePow;
            _Light.vAmbient.rgb += g_Light2DBuffer[i].Color.vAmbient.rgb;
        }
        else if (g_Light2DBuffer[i].LightType == 1)
        {
            float3 vLightWorldPos = float3(g_Light2DBuffer[i].vWorldPos.xy, 0.f);
            float3 vWorldPos = float3(_vWorldPos.xy, 0.f);

            // 광원 중심에서 물체를 향하는 방향
            float3 vLight = normalize(vWorldPos - vLightWorldPos);
            float fDiffusePow = saturate(dot(-vLight, _vWorldDir));
            
            float fDistance = abs(distance(vWorldPos, vLightWorldPos));
            float fDistPow = saturate(1.f - (fDistance / g_Light2DBuffer[i].Radius));
        
            _Light.vDiffuse.rgb += g_Light2DBuffer[i].Color.vDiffuse.rgb * fDiffusePow * fDistPow;
        }
        else if (g_Light2DBuffer[i].LightType == 2)
        {
            
        }
    }
}
float rand(float2 co)
{
    return frac(sin(dot(co.xy, float2(12.9898, 78.233))) * 43758.5453);
}

float hermite(float t)
{
    return t * t * (3.0 - 2.0 * t);
}

float noise(float2 co, float frequency)
{
    float2 v = float2(co.x * frequency, co.y * frequency);

    float ix1 = floor(v.x);
    float iy1 = floor(v.y);
    float ix2 = floor(v.x + 1.0);
    float iy2 = floor(v.y + 1.0);

    float fx = hermite(frac(v.x));
    float fy = hermite(frac(v.y));

    float fade1 = lerp(rand(float2(ix1, iy1)), rand(float2(ix2, iy1)), fx);
    float fade2 = lerp(rand(float2(ix1, iy2)), rand(float2(ix2, iy2)), fx);

    return lerp(fade1, fade2, fy);
}

float pnoise(float2 co, float freq, int steps, float persistence)
{
    float value = 0.0;
    float ampl = 1.0;
    float sum = 0.0;
    for (int i = 0; i < steps; i++)
    {
        sum += ampl;
        value += noise(co, freq) * ampl;
        freq *= 2.0;
        ampl *= persistence;
    }
    return value / sum;
}

//https://www.shadertoy.com/view/3tcBzH
void PerlinNoiseFireWide(out float4 fragColor, in float2 fragCoord)
{
    float2 uv = fragCoord.xy;
    float gradient = uv.y;
    float gradientStep = 0.2;
    
    float2 pos = fragCoord.xy;
    pos.y += g_AccTime * 0.3125;
    //pos.x += g_AccTime * 0.3125;
    
    float4 brighterColor = float4(1.0, 0.65, 0.1, 1.f);
    float4 darkerColor = float4(1.0, 0.0, 0.15, 0.0f);
    float4 middleColor = lerp(brighterColor, darkerColor, 0.5f);

    float noiseTexel = pnoise(pos, 10.0, 5, 0.5);
    
    float firstStep = smoothstep(0.0, noiseTexel, gradient);
    float darkerColorStep = smoothstep(0.0, noiseTexel, gradient - gradientStep);
    float darkerColorPath = firstStep - darkerColorStep;
    float4 color = lerp(brighterColor, darkerColor, darkerColorPath);

    float middleColorStep = smoothstep(0.0, noiseTexel, gradient - 0.2 * 2.0);
    
    color = lerp(color, middleColor, darkerColorStep - middleColorStep);
    color = lerp((float4) 0.0, color, firstStep);
    fragColor = color;
}


float2 hash(float2 p)
{
    p = float2(dot(p, float2(127.1, 311.7)),
			 dot(p, float2(269.5, 183.3)));
    return -1.0 + 2.0 * frac(sin(p) * 43758.5453123);
}

float noise(in float2 p)
{
    const float K1 = 0.366025404; // (sqrt(3)-1)/2;
    const float K2 = 0.211324865; // (3-sqrt(3))/6;
	
    float2 i = floor(p + (p.x + p.y) * K1);
	
    float2 a = p - i + (i.x + i.y) * K2;
    float2 o = (a.x > a.y) ? float2(1.0, 0.0) : float2(0.0, 1.0);
    float2 b = a - o + K2;
    float2 c = a - 1.0 + 2.0 * K2;
	
    float3 h = max(0.5 - float3(dot(a, a), dot(b, b), dot(c, c)), 0.0);
	
    float3 n = h * h * h * h * float3(dot(a, hash(i + 0.0)), dot(b, hash(i + o)), dot(c, hash(i + 1.0)));
	
    return dot(n, (float3)70.0);
}

float fbm(float2 uv)
{
    float f;
    float2x2 m = float2x2(1.6, 1.2, -1.2, 1.6);
    f = 0.5000 * noise(uv);
    uv = mul(m, uv);
    f += 0.2500 * noise(uv);
    uv = mul(m, uv);
    f += 0.1250 * noise(uv);
    uv = mul(m, uv);
    f += 0.0625 * noise(uv);
    uv = mul(m, uv);
    f = 0.5 + 0.5 * f;
    return f;
}
//https://www.shadertoy.com/view/XsXSWS#
void PerlinNoiseFire(out float4 fragColor, in float2 fragCoord)
{
    float2 uv = fragCoord.xy;
    float2 q = float2(uv.x, 1.f - uv.y);
    q.x *= 1.;
    q.y *= 2.;
    float strength = floor(2.);
    float T3 = max(3., 1.25 * strength) * g_AccTime;
    q.x -= 0.5;
    q.y -= 0.6;
    float n = fbm(strength * q - float2(0, T3));
    float c = 1. - 16. * pow(max(0., length(q * float2(1.8 + q.y * 1.5, .75)) - n * max(0., q.y + .25)), 1.2);
	//float c1 = n * c * (0.5 + pow(uv.y,4.));
    float c1 = n * c * (0.5 + pow(1.25 * uv.y, 4.));
    c1 = clamp(c1, 0., 1.);

    float3 col = float3(1.5 * c1, 1.5 * c1 * c1 * c1, c1 * c1 * c1 * c1 * c1 * c1);

    //float a1 = c * (1. - pow(uv.y, 3.));
    float a = c * uv.y;
    fragColor = float4(lerp((float3) 0.0f, col, a), a);
}
float3 Multiply(const float3 m[3], const float3 v)
{
    float x = m[0][0] * v[0] + m[0][1] * v[1] + m[0][2] * v[2];
    float y = m[1][0] * v[1] + m[1][1] * v[1] + m[1][2] * v[2];
    float z = m[2][0] * v[1] + m[2][1] * v[1] + m[2][2] * v[2];
    return float3(x, y, z);
}

float3 rtt_and_odt_fit(float3 v)
{
    float3 a = v * (v + 0.0245786f) - 0.000090537f;
    float3 b = v * (0.983729f * v + 0.4329510f) + 0.238081f;
    return a / b;
}

// sRGB => XYZ => D65_2_D60 => AP1 => RRT_SAT
static float3 aces_input_matrix[3] =
{
    float3(0.59719f, 0.35458f, 0.04823f),
	float3(0.07600f, 0.90834f, 0.01566f),
	float3(0.02840f, 0.13383f, 0.83777f)
};
// ODT_SAT => XYZ => D60_2_D65 => sRGB
static float3 aces_output_matrix[3] =
{
    float3(1.60475f, -0.53108f, -0.07367f),
	float3(-0.10208f, 1.10813f, -0.00605f),
	float3(-0.00327f, -0.07276f, 1.07602f)
};
//https://blog.naver.com/hblee4119/222403726749
float3 ACESToneMapping(float3 colorIn)
{
    colorIn = Multiply(aces_input_matrix, colorIn);
    colorIn = rtt_and_odt_fit(colorIn);
    colorIn = Multiply(aces_output_matrix, colorIn);
    
    //return colorIn;
    return saturate(colorIn);
}
float4 PaperBurn(float4 vColor, float2 vUV, Texture2D BurnTex)
{
    float SinPaperAcc = sin(paperAcc);

    float4 vOut = (float4) 0.f;

    float4 vBurnColor = BurnTex.Sample(g_sam_linear_2, vUV);

    if (vColor.a == 0.0f)
        return vColor;

    vOut = vColor;

    if (SinPaperAcc >= vBurnColor.r)
    {
        clip(-1);
    }

    if (SinPaperAcc - 0.05f <= vBurnColor.r && vBurnColor.r <= SinPaperAcc + 0.05f)
    {
        vOut = float4(1, 0, 0, 1);
        vOut += vOut;

    }

    if (SinPaperAcc - 0.03f <= vBurnColor.r && vBurnColor.r <= SinPaperAcc + 0.03f)
    {
        vOut = float4(1, 1, 0, 1);
        vOut += vOut;
    }

    if (SinPaperAcc - 0.025f <= vBurnColor.r && vBurnColor.r <= SinPaperAcc + 0.025f)
    {
        vOut = float4(1, 1, 1, 1);
        vOut += vOut;
    }

    vOut.a *= vColor.a;

    return vOut;
}
//https://strange-cpp.tistory.com/59
float4 PaperBurn(float4 vColor, float2 vUV, Texture2D BurnTex, inout float4 vEmissive)
{
    float SinPaperAcc = sin(paperAcc);

    float4 vOut = (float4) 0.f;

    float4 vBurnColor = BurnTex.Sample(g_sam_linear_2, vUV);

    if (vColor.a == 0.0f)
        return vColor;

    vOut = vColor;

    if (SinPaperAcc >= vBurnColor.r)
    {
        clip(-1);
    }

    if (SinPaperAcc - 0.05f <= vBurnColor.r && vBurnColor.r <= SinPaperAcc + 0.05f)
    {
        vOut = float4(1, 0, 0, 1);
        vEmissive += vOut;

    }

    if (SinPaperAcc - 0.03f <= vBurnColor.r && vBurnColor.r <= SinPaperAcc + 0.03f)
    {
        vOut = float4(1, 1, 0, 1);
        vEmissive += vOut;
    }

    if (SinPaperAcc - 0.025f <= vBurnColor.r && vBurnColor.r <= SinPaperAcc + 0.025f)
    {
        vOut = float4(1, 1, 1, 1);
        vEmissive += vOut;
    }

    vOut.a *= vColor.a;

    return vOut;
}

void CalcLight3D(float3 _vViewPos, float3 _vViewNormal, int _idx, inout tLightColor _vLightColor, inout float _SpecPow)
{
    tLightInfo lightInfo = g_Light3DBuffer[_idx];

    float fLightPow = 0.f;
    float fSpecPow = 0.f;
    float3 vViewLightDir = (float3) 0.f;

    if (0 == lightInfo.LightType)
    {
        // ViewSpace 에서의 광원의 방향
        vViewLightDir = normalize(mul(float4(normalize(lightInfo.vWorldDir.xyz), 0.f), g_matView)).xyz;
        //float3 vWorldDir = normalize(lightInfo.vWorldDir.xyz);
    
        // ViewSpace 에서의 노말벡터와 광원의 방향을 내적 (램버트 코사인 법칙)    
        fLightPow = saturate(dot(_vViewNormal, -vViewLightDir));
        //fLightPow = saturate(dot(_vViewNormal, vWorldDir));
    
        // 반사광
        float3 vViewReflect = normalize(vViewLightDir + 2.f * (dot(-vViewLightDir, _vViewNormal)) * _vViewNormal);
        float3 vEye = normalize(vViewLightDir);
   
        // 반사광의 세기 구하기
        fSpecPow = saturate(dot(vViewReflect, -vEye));
        fSpecPow = pow(fSpecPow, 60);

        _vLightColor.vAmbient = lightInfo.Color.vAmbient;
    }
    if (1 == lightInfo.LightType)
    {
        float fDistPow = 1.f;
			
        // ViewSpace 에서의 광원의 위치
        float3 vLightViewPos = mul(float4(lightInfo.vWorldPos.xyz, 1.f), g_matView).xyz;
        
        // 광원으로부터 오는 빛의 방향 구하기
        vViewLightDir = normalize(_vViewPos - vLightViewPos);

        // 포인트 라이트로부터 거리체크
        float fDist = distance(_vViewPos, vLightViewPos);
        //float fDist = 0.4f;
        fDistPow = saturate(cos((fDist / lightInfo.Radius) * (PI / 2.f)));
        //fDistPow = saturate(1.f - (fDist / lightInfo.Radius));
                               
        // ViewSpace 에서의 노말벡터와 광원의 방향을 내적 (램버트 코사인 법칙)    
        fLightPow = saturate(dot(_vViewNormal, -vViewLightDir)) * fDistPow;
        
        // 반사광
        float3 vViewReflect = normalize(vViewLightDir + 2.f * (dot(-vViewLightDir, _vViewNormal)) * _vViewNormal);
        float3 vEye = normalize(_vViewPos);
        
        // 반사광의 세기 구하기
        fSpecPow = saturate(dot(vViewReflect, -vEye));
        fSpecPow = pow(fSpecPow, 60) * fDistPow;

        _vLightColor.vAmbient = lightInfo.Color.vAmbient * fLightPow;
    }
    if (2 == lightInfo.LightType)
    {
        //float3 vViewLightPos = mul(float4(lightInfo.vWorldPos.xyz, 1.f), g_matView).xyz;
        //float3 _vViewLightDir = normalize(mul(float4(lightInfo.vWorldDir.xyz, 0.f), g_matView));

        //// 물체 방향
        //float3 _vLookDir = normalize(vViewLightPos - _vViewPos);
        //vViewLightDir = dot(_vViewLightDir, _vLookDir);

        ////float3 _vSm = saturate(1.0f - vViewLightDir * vViewLightDir);
        //float3 _vSm = vViewLightDir;

        //// 각도에 따른 조명 효과 계산
        //// 보정치를 곱해줘도됨.
        //float _fSpotCone = cos(lightInfo.Angle);

        //// 조명 감쇠
        //// Atten = 1 / ( att0i + att1i * d + att2i )
        //float _fSpotAtt = saturate((_vSm - _fSpotCone) / (1.0 - _fSpotCone));
        //float fDistPow = 0.f;
        //float dist = distance(_vViewPos, vViewLightPos);
        //fDistPow = saturate(1.f - (dist / lightInfo.Radius));
        
        //// ViewSpace 에서의 노말벡터와 광원의 방향을 내적 (램버트 코사인 법칙)    

        //// sat안쓰고 임의의 임계값
        //// sat쓰면 훨씬 부드러워짐.
        //fLightPow = saturate(dot(_vViewNormal, -vViewLightDir)) * fDistPow * _fSpotAtt;
        

        //// 반사광
        //float3 vViewReflect = normalize(vViewLightDir + 2.f * (dot(-vViewLightDir, _vViewNormal)) * _vViewNormal);
        ////float3 vViewReflect = reflect(-vViewLightDir, _vViewNormal);
        //float3 vEye = normalize(_vViewPos);

        //// 반사광의 세기 구하기
        //fSpecPow = saturate(dot(vViewReflect, -vEye));
        //fSpecPow = pow(fSpecPow, 20) * fDistPow;
        //_vLightColor.vDiffuse += _fSpotAtt;
        //_vLightColor.vAmbient += _fSpotAtt;

    }

    _vLightColor.vDiffuse = lightInfo.Color.vDiffuse * fLightPow;
    
    _SpecPow += fSpecPow;
}

// ======
// Random
// ======
static float GaussianFilter[5][5] =
{
    0.003f, 0.0133f, 0.0219f, 0.0133f, 0.003f,
    0.0133f, 0.0596f, 0.0983f, 0.0596f, 0.0133f,
    0.0219f, 0.0983f, 0.1621f, 0.0983f, 0.0219f,
    0.0133f, 0.0596f, 0.0983f, 0.0596f, 0.0133f,
    0.003f, 0.0133f, 0.0219f, 0.0133f, 0.003f,
};

float2 texOffset(int x, int y, float2 shadowMapSize)
{
    return float2(x / shadowMapSize.x, y / shadowMapSize.y);
}

float PCF(Texture2D _Tex, float2 texCoords, float compareDepth, float coeff, float2 shadowMapSize)
{
    float shadow = 0.0;
    float x, y;
    //for (y = -1.5f; y <= 1.5f; y += 1.0)
    for (y = -0.5f; y <= 0.5f; y += 0.1)
    {
        //for (x = -1.5f; x <= 1.5f; x += 1.0)
        for (x = -0.5f; x <= 0.5f; x += 0.1)
        {
            shadow += _Tex.SampleCmpLevelZero(g_shadow_sampler, texCoords + texOffset(x, y, shadowMapSize), compareDepth).r;
        }
    }
    return shadow / coeff;
}

float4 GaussianBlur2(Texture2D _Tex, SamplerState _Sampler, float2 _Scale, float2 _PixelUV)
{
    // _Scale 1280 720
    float2 PixelSize = float2(1.0f / _Scale.x, 1.0f / _Scale.y);
    float2 StartUV = _PixelUV.xy + (-PixelSize * 3.0f);
    float2 CurUV = StartUV;
    float4 Result = (float4) 0.0f;
 
    for (int Y = 0; Y < 5; ++Y)
    {
        for (int X = 0; X < 5; ++X)
        {
            Result += _Tex.Sample(_Sampler, CurUV) * GaussianFilter[Y][X];
            CurUV.x += PixelSize.x;
        }
        
        CurUV.x = StartUV.x;
        CurUV.y += PixelSize.y;
    }
    
    return Result;
}

float4 GaussianBlur(Texture2D _Tex, float2 UV)
{
    float4 Out = (float4) 0.0f;
    
    if (1.f < UV.x)
        UV.x = frac(UV.x);
    else if (UV.x < 0.0f)
        UV.x = 1.0f + frac(UV.x);
        
    if (1.f < UV.y)
        UV.y = frac(UV.y);
    else if (UV.y < 0.0f)
        UV.y = 1.0f + frac(UV.y);
    
    int2 iUV = UV;
    iUV -= int2(2, 2);
    
    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            int2 idx = int2(iUV.y + i, iUV.x + j);
            // int2 idx = texOffset(iUV.x + i, iUV.y + i, float2(4000.f, 4000.f));
            Out += _Tex[idx] * GaussianFilter[i][j];
        }
    }
    
    return Out;
}

void GaussianSample(in Texture2D _NoiseTex, float2 _vResolution, float _NomalizedThreadID, out float3 _vOut)
{
    float2 vUV = float2(_NomalizedThreadID, 0.5f);
    
    vUV.x += g_AccTime * 0.5f;
    
    // sin 그래프로 텍스쳐의 샘플링 위치 UV 를 계산
    vUV.y -= (sin((_NomalizedThreadID - (g_AccTime /*그래프 우측 이동 속도*/)) * 2.f * 3.1415926535f * 10.f /*반복주기*/) / 2.f);
    
    if (1.f < vUV.x)
        vUV.x = frac(vUV.x);
    else if (vUV.x < 0.f)
        vUV.x = 1.f + frac(vUV.x);
    
    if (1.f < vUV.y)
        vUV.y = frac(vUV.y);
    else if (vUV.y < 0.f)
        vUV.y = 1.f + frac(vUV.y);
        
    int2 pixel = vUV * _vResolution;
    int2 offset = int2(-2, -2);
    float3 vOut = (float3) 0.f;
    
    for (int i = 0; i < 5; ++i)
    {
        for (int j = 0; j < 5; ++j)
        {
            vOut += _NoiseTex[pixel + offset + int2(j, i)].xyz * GaussianFilter[i][j];
        }
    }
    
    _vOut = vOut;
}

matrix GetBoneMat(int _iBoneIdx, int _iRowIdx)
{
    return g_arrBoneMat[(g_iBoneCount * _iRowIdx) + _iBoneIdx];
}
void Skinning(inout float3 _vPos, inout float3 _vTangent, inout float3 _vBinormal, inout float3 _vNormal
    , inout float4 _vWeight, inout float4 _vIndices
    , int _iRowIdx)
{
    tSkinningInfo info = (tSkinningInfo) 0.f;

    if (_iRowIdx == -1)
        return;

    for (int i = 0; i < 4; ++i)
    {
        if (0.f == _vWeight[i])
            continue;

        matrix matBone = GetBoneMat((int) _vIndices[i], _iRowIdx);

        info.vPos += (mul(float4(_vPos, 1.f), matBone) * _vWeight[i]).xyz;
        info.vTangent += (mul(float4(_vTangent, 0.f), matBone) * _vWeight[i]).xyz;
        info.vBinormal += (mul(float4(_vBinormal, 0.f), matBone) * _vWeight[i]).xyz;
        info.vNormal += (mul(float4(_vNormal, 0.f), matBone) * _vWeight[i]).xyz;
    }

    _vPos = info.vPos;
    _vTangent = normalize(info.vTangent);
    _vBinormal = normalize(info.vBinormal);
    _vNormal = normalize(info.vNormal);
}
void BoneSkinning(inout float4 _vPos, in matrix matBone)
{
    _vPos = mul(_vPos, matBone);
}

void VertexSkinning(inout float4 _vPos, inout float4 _vWeight, inout float4 _vIndices, in RWStructuredBuffer<matrix> mats)
{
    float4 vPos = (float4) 0.f;
    
    for (int i = 0; i < 4; ++i)
    {
        if (0.f == _vWeight[i])
            continue;

        matrix matBone = mats[(int) _vIndices[i]];

        vPos += (mul(_vPos, matBone) * _vWeight[i]);
    }

    _vPos = vPos;
}

void ShadowSkinning(inout float3 _vPos, inout float4 _vWeight, inout float4 _vIndices, int _iRowIdx)
{
    float3 vPos = (float3) 0.f;

    if (_iRowIdx == -1)
        return;

    for (int i = 0; i < 4; ++i)
    {
        if (0.f == _vWeight[i])
            continue;

        matrix matBone = GetBoneMat((int) _vIndices[i], _iRowIdx);

        vPos += (mul(float4(_vPos, 1.f), matBone) * _vWeight[i]).xyz;
    }

    _vPos = vPos;
}

float4 VectorLess(float4 _vQ1, float4 _vQ2)
{
    float4 vReturn =
    {
        (_vQ1[0] < _vQ2[0]) ? asfloat((uint) 0xFFFFFFFF) : 0.f,
        (_vQ1[1] < _vQ2[1]) ? asfloat((uint) 0xFFFFFFFF) : 0.f,
        (_vQ1[2] < _vQ2[2]) ? asfloat((uint) 0xFFFFFFFF) : 0.f,
        (_vQ1[3] < _vQ2[3]) ? asfloat((uint) 0xFFFFFFFF) : 0.f
    };

    return vReturn;
}

void VectorPermute(uint PermuteX, uint PermuteY, uint PermuteZ, uint PermuteW
    , in float4 V1, in float4 V2
    , out float4 _vOut)
{
    float4 aPtr[2] = { V1, V2 };
    float4 Result = (float4) 0.f;

    const uint i0 = PermuteX & 3;
    const uint vi0 = PermuteX >> 2;
    Result[0] = aPtr[vi0][i0];

    const uint i1 = PermuteY & 3;
    const uint vi1 = PermuteY >> 2;
    Result[1] = aPtr[vi1][i1];

    const uint i2 = PermuteZ & 3;
    const uint vi2 = PermuteZ >> 2;
    Result[2] = aPtr[vi2][i2];

    const uint i3 = PermuteW & 3;
    const uint vi3 = PermuteW >> 2;
    Result[3] = aPtr[vi3][i3];

    _vOut = Result;
}


float4 VectorShiftLeft(in float4 _V1, in float4 _V2, uint _Elements)
{
    float4 vOut = (float4) 0.f;

    VectorPermute(_Elements, ((_Elements) + 1), ((_Elements) + 2), ((_Elements) + 3), _V1, _V2, vOut);

    return vOut;
}

float4 VectorSelect(float4 _vQ1, float4 _vQ2, float4 _vControl)
{
    uint4 iQ1 = asuint(_vQ1);
    uint4 iQ2 = asuint(_vQ2);
    uint4 iControl = asuint(_vControl);

    int4 iReturn =
    {
        (iQ1[0] & ~iControl[0]) | (iQ2[0] & iControl[0]),
        (iQ1[1] & ~iControl[1]) | (iQ2[1] & iControl[1]),
        (iQ1[2] & ~iControl[2]) | (iQ2[2] & iControl[2]),
        (iQ1[3] & ~iControl[3]) | (iQ2[3] & iControl[3]),
    };

    return asfloat(iReturn);
}

float4 VectorXorInt(float4 _V1, float4 _V2)
{
    uint4 iV1 = { asuint(_V1.x), asuint(_V1.y), asuint(_V1.z), asuint(_V1.w) };
    uint4 iV2 = { 2147483648, 0, 0, 0 };

    uint4 Result =
    {
        iV1[0] ^ iV2[0],
        iV1[1] ^ iV2[1],
        iV1[2] ^ iV2[2],
        iV1[3] ^ iV2[3]
    };

    return float4(asfloat(Result.x), asfloat(Result.y), asfloat(Result.z), asfloat(Result.w));
}


void MatrixRotationQuaternion(in float4 Quaternion, out matrix _outMat)
{
    float4 Constant1110 = float4(1.f, 1.f, 1.f, 0.f);

    float4 Q0 = Quaternion + Quaternion;
    float4 Q1 = Quaternion * Q0;

    float4 V0 = (float4) 0.f;
    VectorPermute(1, 0, 0, 7, Q1, Constant1110, V0);

    float4 V1 = (float4) 0.f;
    VectorPermute(2, 2, 1, 7, Q1, Constant1110, V1);

    float4 R0 = Constant1110 - V0;
    R0 = R0 - V1;

    V0 = float4(Quaternion[0], Quaternion[0], Quaternion[1], Quaternion[3]);
    V1 = float4(Q0[2], Q0[1], Q0[2], Q0[3]);
    V0 = V0 * V1;

    V1 = float4(Quaternion.w, Quaternion.w, Quaternion.w, Quaternion.w);
    float4 V2 = float4(Q0[1], Q0[2], Q0[0], Q0[3]);
    V1 = V1 * V2;

    float4 R1 = V0 + V1;
    float4 R2 = V0 - V1;

    VectorPermute(1, 4, 5, 2, R1, R2, V0);
    VectorPermute(0, 6, 0, 6, R1, R2, V1);

    matrix M = (matrix) 0.f;
    VectorPermute(0, 4, 5, 3, R0, V0, M._11_12_13_14);
    VectorPermute(6, 1, 7, 3, R0, V0, M._21_22_23_24);
    VectorPermute(4, 5, 2, 3, R0, V1, M._31_32_33_34);
    M._41_42_43_44 = float4(0.f, 0.f, 0.f, 1.f);
    _outMat = M;
}


void MatrixAffineTransformation(in float4 Scaling
    , in float4 RotationOrigin
    , in float4 RotationQuaternion
    , in float4 Translation
    , out matrix _outMat)
{
    matrix MScaling = (matrix) 0.f;
    MScaling._11_22_33 = Scaling.xyz;

    float4 VRotationOrigin = float4(RotationOrigin.xyz, 0.f);
    float4 VTranslation = float4(Translation.xyz, 0.f);

    matrix MRotation = (matrix) 0.f;
    MatrixRotationQuaternion(RotationQuaternion, MRotation);

    matrix M = MScaling;
    M._41_42_43_44 = M._41_42_43_44 - VRotationOrigin;
    M = mul(M, MRotation);
    M._41_42_43_44 = M._41_42_43_44 + VRotationOrigin;
    M._41_42_43_44 = M._41_42_43_44 + VTranslation;
    _outMat = M;
}

void MatrixAffineRotate(in float4 Scaling, in float4 RotationOrigin, in float4 RotationQuaternion
	, out matrix _outMat)
{
    matrix MScaling = (matrix) 0.f;
    MScaling._11_22_33 = Scaling.xyz;

    float4 VRotationOrigin = float4(RotationOrigin.xyz, 0.f);

    matrix MRotation = (matrix) 0.f;
    MatrixRotationQuaternion(RotationQuaternion, MRotation);

    matrix M = MScaling;
    //matrix M = (matrix) 1.f;
    M._41_42_43_44 = M._41_42_43_44 - VRotationOrigin;
    M = mul(M, MRotation);
    M._41_42_43_44 = M._41_42_43_44 + VRotationOrigin;
    _outMat = M;
}

void MatrixAffineTranslate(in float4 Translation, out matrix _outMat)
{
    float4 VTranslation = float4(Translation.xyz, 0.f);
    
    matrix M = (matrix) 0.f;
    M._41_42_43_44 = M._41_42_43_44 + VTranslation;

    _outMat = M;
}


float4 QuternionLerp(in float4 _vQ1, in float4 _vQ2, float _fRatio)
{
    float4 vT = float4(_fRatio, _fRatio, _fRatio, _fRatio);

    // Result = Q1 * sin((1.0 - t) * Omega) / sin(Omega) + Q2 * sin(t * Omega) / sin(Omega)
    const float4 OneMinusEpsilon = { 1.0f - 0.00001f, 1.0f - 0.00001f, 1.0f - 0.00001f, 1.0f - 0.00001f };

    float fQDot = dot(_vQ1, _vQ2);
    float4 CosOmega = float4(fQDot, fQDot, fQDot, fQDot);

    const float4 Zero = (float4) 0.f;
    float4 Control = VectorLess(CosOmega, Zero);
    float4 Sign = VectorSelect(float4(1.f, 1.f, 1.f, 1.f), float4(-1.f, -1.f, -1.f, -1.f), Control);

    CosOmega = CosOmega * Sign;
    Control = VectorLess(CosOmega, OneMinusEpsilon);

    float4 SinOmega = float4(1.f, 1.f, 1.f, 1.f) - (CosOmega * CosOmega);
    SinOmega = float4(sqrt(SinOmega.x), sqrt(SinOmega.y), sqrt(SinOmega.z), sqrt(SinOmega.w));

    float4 Omega = float4(atan2(SinOmega.x, CosOmega.x)
        , atan2(SinOmega.y, CosOmega.y)
        , atan2(SinOmega.z, CosOmega.z)
        , atan2(SinOmega.w, CosOmega.w));

    float4 SignMask = float4(asfloat(0x80000000U), asfloat(0x80000000U), asfloat(0x80000000U), asfloat(0x80000000U));
    float4 V01 = VectorShiftLeft(vT, Zero, 2);
    SignMask = VectorShiftLeft(SignMask, Zero, 3);

    V01 = VectorXorInt(V01, SignMask);
    V01 = float4(1.0f, 0.0f, 0.0f, 0.0f) + V01;

    float4 InvSinOmega = float4(1.f, 1.f, 1.f, 1.f) / SinOmega;

    float4 S0 = V01 * Omega;
    S0 = float4(sin(S0.x), sin(S0.y), sin(S0.z), sin(S0.w));
    S0 = S0 * InvSinOmega;
    S0 = VectorSelect(V01, S0, Control);

    float4 S1 = float4(S0.y, S0.y, S0.y, S0.y);
    S0 = float4(S0.x, S0.x, S0.x, S0.x);

    S1 = S1 * Sign;

    float4 Result = _vQ1 * S0;
    Result = (_vQ2 * S1) + Result;

    return Result;
}


int IntersectsLay(float3 _vertices[3], float3 _vStart, float3 _vDir, out float3 _vCrossPoint, out float _fResult)
{
    float3 edge[2] = { (float3) 0.f, (float3) 0.f };
    edge[0] = _vertices[1].xyz - _vertices[0].xyz;
    edge[1] = _vertices[2].xyz - _vertices[0].xyz;

    float3 normal = normalize(cross(edge[0], edge[1]));
    float b = dot(normal, _vDir);

    // 시작지점과 점과의 거리
    float3 w0 = _vStart - _vertices[0].xyz;
    // edge0과 edge1을 외적해서 나온 노말과
    // 시작지점과의 거리 내적
    // 나온 값 반대
    float a = -dot(normal, w0);

    // b는 노말과 방향을 내적했음
    float t = a / b;

    _fResult = t;

    float3 p = _vStart + t * _vDir;

    _vCrossPoint = p;

    // 충돌했는지 확인
    float uu, uv, vv, wu, wv, inverseD;
    uu = dot(edge[0], edge[0]);
    uv = dot(edge[0], edge[1]);
    vv = dot(edge[1], edge[1]);

    float3 w = p - _vertices[0].xyz;
    wu = dot(w, edge[0]);
    wv = dot(w, edge[1]);
    inverseD = uv * uv - uu * vv;
    inverseD = 1.0f / inverseD;

    float u = (uv * wv - vv * wu) * inverseD;
    if (u < 0.0f || u > 1.0f)
    {
        return 0;
    }

    float v = (uv * wu - uu * wv) * inverseD;
    if (v < 0.0f || (u + v) > 1.0f)
    {
        return 0;
    }

    return 1;
}

float GetTessFactor(float _Length, int _iMinLevel, int _iMaxLevel, float _MinDistance, float _MaxDistance)
{
    if (_MaxDistance < _Length)
    {
        return 0.f;
    }
    else if (_Length < _MinDistance)
    {
        return _iMaxLevel;
    }
    else
    {
        float fLevel = _iMaxLevel - (_iMaxLevel - _iMinLevel) * ((_Length - _MinDistance) / (_MaxDistance - _MinDistance));

        return fLevel;
    }
}

#endif