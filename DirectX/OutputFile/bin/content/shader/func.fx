#ifndef _FUNC
#define _FUNC

#include "value.fx"

bool PointIntersectRhombus(float2 pos, float2 scale, float2 otherPos)
{
    float2 vertex[4];
    float gradient[4];
    float intercept[4];
    uint i = 0;
    float2 direct[4] =
    {
        { -(scale.x / 2.f), 0 },
        { 0, -(scale.y / 2.f) },
        { (scale.x / 2.f), 0 },
        { 0, (scale.y / 2.f) }
    };
    
    for (i = 0; i < 4; ++i)
    {
        vertex[i].x = pos.x + direct[i].x;
        vertex[i].y = pos.y + direct[i].y;
    }
    for (i = 0; i < 4; ++i)
    {
        gradient[i] = ((vertex[i].y - vertex[(i + 1) % 4].y) / (vertex[i].x - vertex[(i + 1) % 4].x));
        intercept[i] = vertex[i].y - gradient[i] * vertex[i].x;
    }
    float _y = otherPos.y;
    float _x = otherPos.x;

    if (gradient[0] * _x + intercept[0] < _y
        && gradient[1] * _x + intercept[1] < _y
        && gradient[2] * _x + intercept[2] > _y
        && gradient[3] * _x + intercept[3] > _y)
    {
        return true;
    }
    else
        return false;
}

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

void CalcLight3D(float3 _vViewPos, float3 _vViewNormal, int _idx, inout tLightColor _vLightColor, inout float _SpecPow)
{
    tLightInfo lightInfo = g_Light3DBuffer[_idx];

    float fLightPow = 0.f;
    float fSpecPow = 0.f;
    float3 vViewLightDir = (float3) 0.f;

    if(0 == lightInfo.LightType)
    {
        // ViewSpace 에서의 광원의 방향
        vViewLightDir = normalize(mul(float4(normalize(lightInfo.vWorldDir.xyz), 0.f), g_matView)).xyz;
    
        // ViewSpace 에서의 노말벡터와 광원의 방향을 내적 (램버트 코사인 법칙)    
        fLightPow = saturate(dot(_vViewNormal, -vViewLightDir));
    
        // 반사광
        float3 vViewReflect = normalize(vViewLightDir + 2.f * (dot(-vViewLightDir, _vViewNormal)) * _vViewNormal);
        float3 vEye = normalize(_vViewPos);
   
        // 반사광의 세기 구하기
        fSpecPow = saturate(dot(vViewReflect, -vEye));
        fSpecPow = pow(fSpecPow, 20);
    }
    else if(1 == lightInfo.LightType)
    {
        float fDistPow = 1.f;
        
        // ViewSpace 에서의 광원의 위치
        float3 vLightViewPos = mul(float4(lightInfo.vWorldPos.xyz, 1.f), g_matView).xyz;
        
        // 광원으로부터 오는 빛의 방향 구하기
        vViewLightDir = normalize(_vViewPos - vLightViewPos);
        
        // 포인트 라이트로부터 거리체크
        float fDist = distance(_vViewPos, vLightViewPos);
        fDistPow = saturate(cos((fDist / lightInfo.Radius)));
                               
        // ViewSpace 에서의 노말벡터와 광원의 방향을 내적 (램버트 코사인 법칙)    
        fLightPow = saturate(dot(_vViewNormal, -vViewLightDir)) * fDistPow;
        
        // 반사광
        float3 vViewReflect = normalize(vViewLightDir + 2.f * (dot(-vViewLightDir, _vViewNormal)) * _vViewNormal);
        float3 vEye = normalize(_vViewPos);
        
        // 반사광의 세기 구하기
        fSpecPow = saturate(dot(vViewReflect, -vEye));
        fSpecPow = pow(fSpecPow, 20) * fDistPow;
    }
    else
    {
        float3 vViewLightPos = mul(float4(lightInfo.vWorldPos.xyz, 1.f), g_matView).xyz;
        float3 _vViewLightDir = normalize(mul(float4(lightInfo.vWorldDir.xyz, 0.f), g_matView));

        // 물체 방향
        float3 _vLookDir = normalize(vViewLightPos - _vViewPos);
        vViewLightDir = dot(_vViewLightDir, _vLookDir);

        //float3 _vSm = saturate(1.0f - vViewLightDir * vViewLightDir);
        float3 _vSm = vViewLightDir;

        // 각도에 따른 조명 효과 계산
        // 보정치를 곱해줘도됨.
        float _fSpotCone =cos(lightInfo.Angle);

        // 조명 감쇠
        // Atten = 1 / ( att0i + att1i * d + att2i )
        float _fSpotAtt = saturate((_vSm - _fSpotCone) / (1.0 - _fSpotCone));
        float fDistPow = 0.f;
        float dist = distance(_vViewPos, vViewLightPos);
        fDistPow = saturate(1.f - (dist / lightInfo.Radius));
        
        // ViewSpace 에서의 노말벡터와 광원의 방향을 내적 (램버트 코사인 법칙)    

        // sat안쓰고 임의의 임계값
        // sat쓰면 훨씬 부드러워짐.
        fLightPow = saturate(dot(_vViewNormal, -vViewLightDir)) * fDistPow * _fSpotAtt;
        

        // 반사광
        float3 vViewReflect = normalize(vViewLightDir + 2.f * (dot(-vViewLightDir, _vViewNormal)) * _vViewNormal);
        //float3 vViewReflect = reflect(-vViewLightDir, _vViewNormal);
        float3 vEye = normalize(_vViewPos);

        // 반사광의 세기 구하기
        fSpecPow = saturate(dot(vViewReflect, -vEye));
        fSpecPow = pow(fSpecPow, 20) * fDistPow;
        _vLightColor.vDiffuse += _fSpotAtt;
        _vLightColor.vAmbient += _fSpotAtt;

    }
    _vLightColor.vDiffuse += lightInfo.Color.vDiffuse * fLightPow;

    _vLightColor.vAmbient += lightInfo.Color.vAmbient;
    _SpecPow += fSpecPow;
}

// ======
// Random
// ======
static float GaussianFilter[5][5] =
{
    0.003f,  0.0133f, 0.0219f, 0.0133f, 0.003f,
    0.0133f, 0.0596f, 0.0983f, 0.0596f, 0.0133f,
    0.0219f, 0.0983f, 0.1621f, 0.0983f, 0.0219f,
    0.0133f, 0.0596f, 0.0983f, 0.0596f, 0.0133f,
    0.003f,  0.0133f, 0.0219f, 0.0133f, 0.003f,
};

#define NUM_TAPS 12

static const float2 fTaps_Poisson[NUM_TAPS] =
{
    { -.326, -.406 },
    { -.840, -.074 },
    { -.696, .457 },
    { -.203, .621 },
    { .962, -.195 },
    { .473, -.480 },
    { .519, .767 },
    { .185, -.893 },
    { .507, .064 },
    { .896, .412 },
    { -.322, -.933 },
    { -.792, -.598 }
};

static const float KernelOffsets[3] = { 0.0f, 1.3846153846f, 3.2307692308f };
static const float BlurWeights[3] = { 0.2270270270f, 0.3162162162f, 0.0702702703f };

float2 blurOffsets[9] =
{
    float2(-1.0, -1.0), float2(0.0, -1.0), float2(1.0, -1.0),
    float2(-1.0, 0.0), float2(0.0, 0.0), float2(1.0, 0.0),
    float2(-1.0, 1.0), float2(0.0, 1.0), float2(1.0, 1.0)
};

float blurWeights[9] =
{
    1.0, 2.0, 1.0,
    2.0, 4.0, 2.0,
    1.0, 2.0, 1.0
};

float4 Blur2(Texture2D _Tex, SamplerState _Sampler, float2 texCoords, float blurAmount, float2 texelSize)
{
    float4 sum = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float total = 16.0f; // Normalization factor

    for (int i = 0; i < 9; ++i)
    {
        float2 offset = texelSize * blurOffsets[i] * blurAmount;
        float4 depth = _Tex.SampleLevel(_Sampler, texCoords + offset, 0.0f);
        sum += depth * blurWeights[i];
    }

    return sum / total;
}

float4 PSSimpleBlurV(Texture2D tex, float2 input)
{
    uint loopCount = 5;
    float2 textureDimensions = (float2) 1.f;
    float blurXOffset = 0.005f;
    float3 textureColor = float3(1.0f, 0.0f, 0.0f);
    float2 uv = input.xy;
    if (uv.x > (blurXOffset + 0.005f))
    {
        textureColor = tex.Sample(g_sam_0, uv).xyz * BlurWeights[0];
        for (int i = 1; i < 3; i++)
        {
            float2 normalizedOffset = float2(0.0f, KernelOffsets[i]) / textureDimensions.y;
            textureColor += tex.Sample(g_sam_0, uv + normalizedOffset).xyz * BlurWeights[i];
            textureColor += tex.Sample(g_sam_0, uv - normalizedOffset).xyz * BlurWeights[i];
        }
    }
    else if (uv.x <= (blurXOffset - 0.005f))
    {
        textureColor = tex.Sample(g_sam_1, uv).xyz;
    }

    // Artificially increase the workload to simulate a more complex shader.
    const float3 textureColorOrig = textureColor;
    for (uint i = 0; i < loopCount; i++)
    {
        textureColor += textureColorOrig;
    }

    if (loopCount > 0)
    {
        textureColor /= loopCount + 1;
    }

    return float4(textureColor, 1.0);
}

float4 BlurDepth2(Texture2D _ShaderTexture, SamplerState _SampleType, float2 _TexCoord, float _TexelSize)
{
    float weight0, weight1, weight2, weight3, weight4;
    float normalization;
    float4 color;

    float _TexCoord1 = _TexCoord + float2(_TexelSize * -4.0f, 0.0f);
    float _TexCoord2 = _TexCoord + float2(_TexelSize * -3.0f, 0.0f);
    float _TexCoord3 = _TexCoord + float2(_TexelSize * -2.0f, 0.0f);
    float _TexCoord4 = _TexCoord + float2(_TexelSize * -1.0f, 0.0f);
    float _TexCoord5 = _TexCoord + float2(_TexelSize * 0.0f, 0.0f);
    float _TexCoord6 = _TexCoord + float2(_TexelSize * 1.0f, 0.0f);
    float _TexCoord7 = _TexCoord + float2(_TexelSize * 2.0f, 0.0f);
    float _TexCoord8 = _TexCoord + float2(_TexelSize * 3.0f, 0.0f);
    float _TexCoord9 = _TexCoord + float2(_TexelSize * 4.0f, 0.0f);

    weight0 = 1.0f;
    weight1 = 0.9f;
    weight2 = 0.55f;
    weight3 = 0.18f;
    weight4 = 0.1f;

    // 가중치들을 살짝 평균내어 정규화 값을 만듭니다.
    normalization = (weight0 + 2.0f * (weight1 + weight2 + weight3 + weight4));

    // 가중치들을 정규화합니다.
    weight0 = weight0 / normalization;
    weight1 = weight1 / normalization;
    weight2 = weight2 / normalization;
    weight3 = weight3 / normalization;
    weight4 = weight4 / normalization;

    // 색깔을 검정색으로 초기화합니다.
    color = float4(0.0f, 0.0f, 0.0f, 0.0f);

    // 수평선상의 아홉 픽셀값들을 가중치를 곱해 더합니다.
    color += _ShaderTexture.Sample(_SampleType, _TexCoord1) * weight4;
    color += _ShaderTexture.Sample(_SampleType, _TexCoord2) * weight3;
    color += _ShaderTexture.Sample(_SampleType, _TexCoord3) * weight2;
    color += _ShaderTexture.Sample(_SampleType, _TexCoord4) * weight1;
    color += _ShaderTexture.Sample(_SampleType, _TexCoord5) * weight0;
    color += _ShaderTexture.Sample(_SampleType, _TexCoord6) * weight1;
    color += _ShaderTexture.Sample(_SampleType, _TexCoord7) * weight2;
    color += _ShaderTexture.Sample(_SampleType, _TexCoord8) * weight3;
    color += _ShaderTexture.Sample(_SampleType, _TexCoord9) * weight4;

    color.a = 1.0f;

    return color;
}

float4 BlurDepth(Texture2D shadowMapTexture, SamplerComparisonState shadowSampler, float2 texelSize, float2 texCoord)
{
    float blurAmount = 0.005; // 블러 양 조정

    // 가우시안 블러 커널 및 가중치
    const int kernelSize = 5;
    float weights[kernelSize] = { 0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162 };

    float blurValue = 0.0f;

    // 수평 블러 적용
    for (int i = -kernelSize / 2; i <= kernelSize / 2; ++i)
    {
        blurValue += shadowMapTexture.SampleCmp(shadowSampler, texCoord + float2(texelSize.x * i, 0), blurAmount) * weights[i + kernelSize / 2];
    }

    // 수직 블러 적용
    for (int i = -kernelSize / 2; i <= kernelSize / 2; ++i)
    {
        blurValue += shadowMapTexture.SampleCmp(shadowSampler, texCoord + float2(0, texelSize.y * i), blurAmount) * weights[i + kernelSize / 2];
    }

    return float4(blurValue, blurValue, blurValue, 1.0f); // 블러된 값을 반환 (여기서는 흑백으로 표시)
}


void GaussianSample(in Texture2D _NoiseTex, float2 _vResolution, float _NomalizedThreadID, out float3 _vOut)
{
    float2 vUV = float2(_NomalizedThreadID, 0.5f);       
    
    vUV.x += g_AccTime * 0.5f;
    
    // sin 그래프로 텍스쳐의 샘플링 위치 UV 를 계산
    vUV.y -= (sin((_NomalizedThreadID - (g_AccTime/*그래프 우측 이동 속도*/)) * 2.f * 3.1415926535f * 10.f/*반복주기*/) / 2.f);
    
    if( 1.f < vUV.x)
        vUV.x = frac(vUV.x);
    else if(vUV.x < 0.f)
        vUV.x = 1.f + frac(vUV.x);
    
    if( 1.f < vUV.y)
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

#endif
