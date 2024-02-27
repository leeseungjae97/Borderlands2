#pragma once
#include "CComponent.h"

#include "CFrustum.h"

class CTexture;

class CCamera :
    public CComponent
{
private:
    CFrustum    m_Frustum;

    float       m_fAspectRatio;
    float       m_fScale;           // Orthograpic 에서 사용하는 카메라 배율

    PROJ_TYPE   m_ProjType;

    Matrix      m_matView;
    Matrix      m_matViewInv;

    Matrix      m_matProj;
    Matrix      m_matProjInv;

    UINT        m_iLayerMask;

    int         m_iCamIdx;          // 카메라 우선순위

    float       m_NearZ;
    float       m_FarZ;

    float       m_FOV;

    float       m_OrthoWidth;       // Orthgraphic 에서의 가로 투영 범위
    float       m_OrthoHeight;      // OrthGraphic 에서의 세로 투영 범위

    bool        m_bESM;

    float       m_fT[4];

    tRay        m_ray;

    //bool        m_HDR;
    //bool        m_Bloom;
    //bool        m_Blur;
    //float       f_blurFactor;
    //float       f_blurFactor1;
    //float       f_blurFactor2;

    //Vec4        vScaleColor;
    //Vec4        vImpactColor;
    map<ULONG64, vector<tInstObj>>		m_mapInstGroup_D;	    // Deferred
    map<ULONG64, vector<tInstObj>>		m_mapInstGroup_F;	    // Foward ( Opaque, Mask )	
    map<INT_PTR, vector<tInstObj>>		m_mapSingleObj;		    // Single Object

    //vector<CGameObject*>    m_vecDeferred;
    vector<CGameObject*>    m_vecDeferredDecal;

    vector<CGameObject*>    m_vecOpaque;
    vector<CGameObject*>    m_vecMask;
    vector<CGameObject*>    m_vecDecal;
    vector<CGameObject*>    m_vecTransparent;    
    vector<CGameObject*>    m_vecUI;
    vector<CGameObject*>    m_vecPost;

    vector<CGameObject*>    m_vecShadow;

public:
    void SetProjType(PROJ_TYPE _Type) { m_ProjType = _Type; }
    PROJ_TYPE GetProjType() { return m_ProjType; }

    void SetScale(float _fScale) { m_fScale = _fScale; }
    float GetScale() { return m_fScale; }

    void SetLayerMask(int _iLayer, bool _Visible);
    void SetLayerMaskAll(bool _Visible);

    void SetFOV(float _Radian) { m_FOV = _Radian; }
    float GetFOV() { return m_FOV; }

    void SetOrthoWidth(float _width) { m_OrthoWidth = _width; }
    void SetOrthoHeight(float _height) { m_OrthoHeight = _height; }

    float GetorthoWidth() { return m_OrthoWidth; }
    float GetOrthoHeight() { return m_OrthoHeight; }

    void SetCameraIndex(int _idx);

    const Matrix& GetViewMat() { return m_matView; }
    const Matrix& GetProjMat() { return m_matProj; }

    void SetNearZ(float _NearZ) { m_NearZ = _NearZ; }
    void SetFarZ(float _FarZ) { m_FarZ = _FarZ; }

    float GetNearZ() { return m_NearZ; }
    float GetFarZ() { return m_FarZ; }

    Matrix GetProjInvMat() { return m_matProjInv; }
    Matrix GetViewInvMat() { return m_matViewInv; }

    void SetESM(bool _ESM) { m_bESM = _ESM; }
    bool GetESM() { return m_bESM; }

    void SetFloatConstant(int _Index, float _Value) { m_fT[_Index] = _Value; }
    float GetFloatConstant(int _Index) { return m_fT[_Index]; }

    const tRay& GetRay() { return m_ray; }

    //bool IsHDR() { return m_HDR; }
    //void SetHDR(bool _HDR) { m_HDR = _HDR; }

    //void SetBloom(bool _Bloom) { m_Bloom = _Bloom; }
    //void SetBlur(bool blur) { m_Blur = blur; }

    //void SetFactor(float factor) { f_blurFactor = factor; }
    //float GetFactor() { return f_blurFactor; }


    //void SetFactor1(float factor) { f_blurFactor1 = factor; }
    //float GetFactor1() { return f_blurFactor1; }


    //void SetFactor2(float factor) { f_blurFactor2 = factor; }
    //float GetFactor2() { return f_blurFactor2; }

    //void SetColor(Vec4 Color) { vScaleColor = Color; }
    //Vec4 GetColor() { return vScaleColor; }

    //void SetImpact(Vec4 Color) { vImpactColor = Color; }
    //Vec4 GetImpact() { return vImpactColor; }

protected:
    void CalRay();

public:
    void SortObject();
    void SortObject_Shadow();

    void render();
    void render_shadowmap();

    void FixedTransform();

public:
    virtual void begin() override;
    virtual void finaltick() override;


private:
    void clear();
    void clear_shadow();

    void render_deferred();
    void render_forward();
    void render_decal();
    void render_transparent();
    void render_postprocess();
    void render_ui();


    void CalcViewMat();
    void CalcProjMat();
    void UpdateMatrix();

    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;

    CLONE(CCamera);
public:    
    CCamera();
    CCamera(const CCamera& _Other);
    ~CCamera();
};

