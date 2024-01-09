#pragma once

#include "CComponent.h"

#include "ptr.h"

class CMesh;
class CMaterial;

class CLight3D :
    public CComponent
{
private:
    tLightInfo      m_LightInfo;

    Ptr<CMesh>      m_Mesh;
    Ptr<CMaterial>  m_Mtrl;

    UINT            m_LightIdx;
    bool            m_bShowRange;

    CGameObject*    m_pCamObj;
    bool            m_bShadow;
    bool            m_bGaus;
    float           m_fLightDepthCoeff;

    float           m_f[3];

public:
    void SetLightColor(Vec3 _Color) { m_LightInfo.Color.vDiffuse = _Color; }
    void SetLightAmbient(Vec3 _Amb) { m_LightInfo.Color.vAmbient = _Amb; }
    void SetLightType(LIGHT_TYPE _type);
    void SetRadius(float _fRadius);
    void SetAngle(float _Angle) { m_LightInfo.Angle = _Angle; }

    Vec3 GetLightColor() { return m_LightInfo.Color.vDiffuse; }
    Vec3 GetLightAmbient() { return m_LightInfo.Color.vAmbient; }
    LIGHT_TYPE GetLightType() { return (LIGHT_TYPE)m_LightInfo.LightType; }
    float GetRadius() { return m_LightInfo.Radius; }
    float GetAngle() { return m_LightInfo.Angle; }

    void SetShadow(bool _bShadow) { m_bShadow = _bShadow; }
    bool GetShadow() { return m_bShadow; }

    void SetGaus(bool _bGaus) { m_bGaus= _bGaus; }
    bool GetGaus() { return m_bGaus; }

    void SetLightDepthCoeff(float _LightDepthCoeff) { m_fLightDepthCoeff= _LightDepthCoeff; }
    float GetLightDepthCoeff() { return m_fLightDepthCoeff; }

    void SetFloatConstant(int _Index, float _Value) { m_f[_Index] = _Value; }
    float GetFloatConstant(int _Index) { return m_f[_Index]; }

public:
    virtual void finaltick() override;
    void render();
    void render_shadowmap();

    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;

    CLONE(CLight3D)
public:
    CLight3D();
    CLight3D(const CLight3D& _Origin);
    ~CLight3D();

};

