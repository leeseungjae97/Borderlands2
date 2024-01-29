#pragma once
#include "CHeightMapShader.h"
#include "CRayCastShader.h"
#include "CColorMapShader.h"
#include "CWeightMapShader.h"

#include "ptr.h"

#include "CRenderComponent.h"
#include "RecastNavi.h"

enum class LANDSCAPE_MOD
{
    HEIGHT_MAP,
    SPLAT,
    COLOR_MAP,
    NONE,
};

struct tWeight_4
{
    float arrWeight[4];
};

class CLandScape :
    public CRenderComponent
{
private:
    UINT                    m_iFaceX;
    UINT                    m_iFaceZ;

    Vec2                    m_vBrushScale;      // �귯�� ũ��(��ü ������� ũ�� ������)
    Ptr<CTexture>           m_pBrushTex;        // �귯���� �ؽ���

    Ptr<CRayCastShader>     m_pCSRaycast;       // ��ŷ ���̴�
    CStructuredBuffer*      m_pCrossBuffer;	    // ���콺 ��ŷ�Ǵ� ���� ���� �޴� ����

    Ptr<CHeightMapShader>   m_pCSHeightMap;     // ���̸� ���̴�
    Ptr<CColorMapShader>    m_pCSColorMap;

    Ptr<CTexture>           m_HeightMap;
    Ptr<CTexture>           m_ColorMap;
    wstring                 m_HeightMapName;
    wstring                 m_ColorMapName;

    UINT                    m_TessEdge;
    UINT                    m_TessInside;

    CGameObject*            m_pBrushLineDecal;

    Ptr<CWeightMapShader>   m_pCSWeightMap;     // ����ġ ���̴�
    CStructuredBuffer*      m_pWeightMapBuffer; // ����ġ ���� ����
    UINT					m_iWeightWidth;		// ����ġ ���� ���μ��� ��� ��
    UINT					m_iWeightHeight;	// ����ġ ���� ���μ��� ��� ��
    UINT                    m_iWeightIdx;		// ���� ��ų ����ġ ����

    LANDSCAPE_MOD           m_eMod; 	        // ���� ����忡�� ���°�

    Ptr<CTexture>           m_pTileArrTex;      // Ÿ�� �迭 �ؽ���
    RecastNavi*             m_Recast;
    CStructuredBuffer*      m_pVertexBuffer;
    // ========================================================================
    vector<tNode>           m_vecNode;
    //CStructuredBuffer*      m_pNodesBuffer;


    //Ptr<CLandScapePathShader> m_pCSPathInit;
    // ========================================================================


public:
    void SetTessEdge(float _Edge) { m_TessEdge = _Edge; }
    float GetTessEdge() { return m_TessEdge; }

    void SetTessInside(float _Inside) { m_TessInside = _Inside; }
    float GetTessInside() { return m_TessInside; }

    void SetFaceX(UINT _FaceX);
    UINT GetFaceX() { return m_iFaceX; }

    void SetFaceZ(UINT _FaceZ);
    UINT GetFaceZ() { return m_iFaceZ; }

    void SetFace(UINT _FaceX, UINT _FaceZ);
    void SetHeightMap(Ptr<CTexture> _HeightMap) { m_HeightMap = _HeightMap; }
    void SetColorMap(Ptr<CTexture> _ColorMap) { m_ColorMap = _ColorMap; }

	Ptr<CTexture> GetHeightMap() { return m_HeightMap; }
	Ptr<CTexture> GetColorMap() { return m_ColorMap; }
    Ptr<CTexture> GetWeightTexture() { return m_pTileArrTex; }

    void SetBrush(Ptr<CTexture> _BrushTex);
	Ptr<CTexture> GetBrush() { return m_pBrushTex; }

    void SetHeightMapName(const wstring& name);
    const wstring& GetHeightMapName() { return m_HeightMapName; }

    void SetColorMapName(const wstring& name);
    const wstring& GetColorMapName() { return m_ColorMapName; }

    void SetBrushPopUp(bool _PopUp) { m_pCSHeightMap->SetPopUp(_PopUp); }
    bool GetBrushPopUp() { return m_pCSHeightMap->GetPopUp(); }

    void SetBrushGuideLine(CGameObject* _Obj) { m_pBrushLineDecal = _Obj; }

    void SetBrushScale(Vec2 _Scale) { m_vBrushScale = _Scale; }
    Vec2 GetBrushScale() { return m_vBrushScale; }

    int GetWeightIdx() { return m_iWeightIdx; }

    tRaycastOut GetLandScapeRayCastOut();

    virtual void finaltick() override;
    virtual void render() override;
    void render(UINT _iSubset) override;

    void Bake();

    void SaveCurMesh();

    void SetLandScapeMesh(Ptr<CMesh> _Mesh);

public:
    void init();
    void CreateMesh();
    void CreateComputeShader();
    void CreateTexture();
    void AddHeightMapToMesh();

    void CreateHeightMap();
    void CreateColorMap();
    void CreateWeightMap();

    void RayCasting();

    void LoadFromLevelFile(FILE* _FILE) override;
    void SaveToLevelFile(FILE* _FILE) override;

public:
    CLONE(CLandScape);

public:
    CLandScape();
    ~CLandScape();
};

