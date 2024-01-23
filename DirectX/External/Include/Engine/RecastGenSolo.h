#pragma once
#include "RecastNavi.h"

class RecastGenSolo :
    public RecastNavi
{
private:
	bool m_keepInterResults;
	float m_totalBuildTimeMs;

	unsigned char* m_triareas;
	rcHeightfield* m_solid;
	rcCompactHeightfield* m_chf;
	rcContourSet* m_cset;
	rcPolyMesh* m_pmesh;
	rcConfig m_cfg;
	rcPolyMeshDetail* m_dmesh;

	void cleanup();
public:
	//void AddTriangle(int a, int b, int c, int& cap);
	//void AddVertex(float x, float y, float z, int& cap);

	virtual void handleSettings();
	virtual void handleTools();
	virtual void handleDebugMode();

	virtual void handleRender();
	virtual void handleRenderOverlay(double* proj, double* model, int* view);
	virtual void handleMeshChanged(class InputGeom* geom);
	virtual bool handleBuild(CGameObject* _Obj);

public:
    RecastGenSolo();
    virtual ~RecastGenSolo();
};

