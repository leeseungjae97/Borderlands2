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

float2 texOffset(int u, int v, float2 shadowMapSize)
{
    return float2(u * 1.0f / shadowMapSize.x, v * 1.0f / shadowMapSize.y);
}


float PCF(Texture2D _Tex, float2 texCoords, float compareDepth, float coeff, float2 shadowMapSize)
{
    float shadow = 0.0;

    float x, y;
    for (y = -1.5f; y <= 1.5f; y += 1.0)
    {
        for (x = -1.5f; x <= 1.5f; x += 1.0)
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

#endif
