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

            // ���� �߽ɿ��� ��ü�� ���ϴ� ����
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
        // ViewSpace ������ ������ ����
        vViewLightDir = normalize(mul(float4(normalize(lightInfo.vWorldPos.xyz), 0.f), g_matView)).xyz;
    
        // ViewSpace ������ �븻���Ϳ� ������ ������ ���� (����Ʈ �ڻ��� ��Ģ)    
        fLightPow = saturate(dot(_vViewNormal, -vViewLightDir));
    
        // �ݻ籤
        float3 vViewReflect = normalize(vViewLightDir + 2.f * (dot(-vViewLightDir, _vViewNormal)) * _vViewNormal);
        //float3 vViewReflect = reflect(-vViewLightDir, _vViewNormal);
        float3 vEye = normalize(_vViewPos);
   
        // �ݻ籤�� ���� ���ϱ�
        fSpecPow = saturate(dot(vViewReflect, -vEye));
        fSpecPow = pow(fSpecPow, 20);

    }
    else if(1 == lightInfo.LightType)
    {
        float fDistPow = 1.f;
        
        // ViewSpace ������ ������ ��ġ
        float3 vLightViewPos = mul(float4(lightInfo.vWorldPos.xyz, 1.f), g_matView).xyz;
        
        // �������κ��� ���� ���� ���� ���ϱ�
        vViewLightDir = normalize(_vViewPos - vLightViewPos);
        
        // ����Ʈ ����Ʈ�κ��� �Ÿ�üũ
        float fDist = distance(_vViewPos, vLightViewPos);
        fDistPow = saturate(cos((fDist / lightInfo.Radius)));
                               
        // ViewSpace ������ �븻���Ϳ� ������ ������ ���� (����Ʈ �ڻ��� ��Ģ)    
        fLightPow = saturate(dot(_vViewNormal, -vViewLightDir)) * fDistPow;
        
        // �ݻ籤
        float3 vViewReflect = normalize(vViewLightDir + 2.f * (dot(-vViewLightDir, _vViewNormal)) * _vViewNormal);
        float3 vEye = normalize(_vViewPos);
        
        // �ݻ籤�� ���� ���ϱ�
        fSpecPow = saturate(dot(vViewReflect, -vEye));
        fSpecPow = pow(fSpecPow, 20) * fDistPow;
    }
    else
    {
        float3 vViewLightPos = mul(float4(lightInfo.vWorldPos.xyz, 1.f), g_matView).xyz;
        float3 _vViewLightDir = normalize(mul(float4(lightInfo.vWorldDir.xyz, 0.f), g_matView));

        // ��ü ����
        float3 _vLookDir = normalize(vViewLightPos - _vViewPos);
        vViewLightDir = dot(_vViewLightDir, _vLookDir);

        //float3 _vSm = saturate(1.0f - vViewLightDir * vViewLightDir);
        float3 _vSm = vViewLightDir;

        // ������ ���� ���� ȿ�� ���
        // ����ġ�� �����൵��.
        float _fSpotCone =cos(lightInfo.Angle);

        // ���� ����
        // Atten = 1 / ( att0i + att1i * d + att2i )
        float _fSpotAtt = saturate((_vSm - _fSpotCone) / (1.0 - _fSpotCone));
        float fDistPow = 0.f;
        float dist = distance(_vViewPos, vViewLightPos);
        fDistPow = saturate(1.f - (dist / lightInfo.Radius));
        
        // ViewSpace ������ �븻���Ϳ� ������ ������ ���� (����Ʈ �ڻ��� ��Ģ)    

        // sat�Ⱦ��� ������ �Ӱ谪
        // sat���� �ξ� �ε巯����.
        fLightPow = saturate(dot(_vViewNormal, -vViewLightDir)) * fDistPow * _fSpotAtt;
        

        // �ݻ籤
        float3 vViewReflect = normalize(vViewLightDir + 2.f * (dot(-vViewLightDir, _vViewNormal)) * _vViewNormal);
        //float3 vViewReflect = reflect(-vViewLightDir, _vViewNormal);
        float3 vEye = normalize(_vViewPos);

        // �ݻ籤�� ���� ���ϱ�
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

void GaussianSample(in Texture2D _NoiseTex, float2 _vResolution, float _NomalizedThreadID, out float3 _vOut)
{
    float2 vUV = float2(_NomalizedThreadID, 0.5f);       
    
    vUV.x += g_AccTime * 0.5f;
    
    // sin �׷����� �ؽ����� ���ø� ��ġ UV �� ���
    vUV.y -= (sin((_NomalizedThreadID - (g_AccTime/*�׷��� ���� �̵� �ӵ�*/)) * 2.f * 3.1415926535f * 10.f/*�ݺ��ֱ�*/) / 2.f);
    
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
