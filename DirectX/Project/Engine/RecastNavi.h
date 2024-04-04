#pragma once

#include "RecastBuildContext.h"
#include "PerfTimer.h"
#include "Recast.h"
#include "DetourAlloc.h"
#include "DetourAssert.h"
#include "DetourNavMesh.h"
#include "DetourNavMeshQuery.h"
#include "DetourNavMeshBuilder.h"
#include "DetourCommon.h"
#include "DetourDebugDraw.h"
#include "InputGeom.h"
#include "ChunkyTriMesh.h"

enum PolyAreas
{
	POLYAREA_GROUND,
	POLYAREA_WATER,
	POLYAREA_ROAD,
	POLYAREA_DOOR,
	POLYAREA_GRASS,
	POLYAREA_JUMP,
};
enum PolyFlags
{
	POLYFLAGS_WALK = 0x01,		// Ability to walk (ground, grass, road)
	POLYFLAGS_SWIM = 0x02,		// Ability to swim (water).
	POLYFLAGS_DOOR = 0x04,		// Ability to move through doors.
	POLYFLAGS_JUMP = 0x08,		// Ability to jump.
	POLYFLAGS_DISABLED = 0x10,	// Disabled polygon
	POLYFLAGS_ALL = 0xffff		// All abilities.

};
//
//struct RecastTool
//{
//	virtual ~RecastTool();
//	virtual int type() = 0;
//	virtual void init(class Sample* sample) = 0;
//	virtual void reset() = 0;
//	virtual void handleMenu() = 0;
//	virtual void handleClick(const float* s, const float* p, bool shift) = 0;
//	virtual void handleRender() = 0;
//	virtual void handleRenderOverlay(double* proj, double* model, int* view) = 0;
//	virtual void handleToggle() = 0;
//	virtual void handleStep() = 0;
//	virtual void handleUpdate(const float dt) = 0;
//};
//
//struct RecastToolState
//{
//	virtual ~RecastToolState();
//	virtual void init(class Sample* sample) = 0;
//	virtual void reset() = 0;
//	virtual void handleRender() = 0;
//	virtual void handleRenderOverlay(double* proj, double* model, int* view) = 0;
//	virtual void handleUpdate(const float dt) = 0;
//};

struct NavMeshSetHeader
{
	int magic;
	int version;
	int numTiles;
	dtNavMeshParams params;
};

struct NavMeshTileHeader
{
	dtTileRef tileRef;
	int dataSize;
};

static const int NAVMESHSET_MAGIC = 'M' << 24 | 'S' << 16 | 'E' << 8 | 'T'; //'MSET';
static const int NAVMESHSET_VERSION = 1;

class RecastNavi
{
protected:
	InputGeom* m_geom;
	dtNavMesh* m_navMesh;
	class dtNavMeshQuery* m_navQuery;
	//class dtCrowd* m_crowd;
	//class duDebugDraw* m_dd;

	unsigned char m_navMeshDrawFlags;

	float m_cellSize;
	float m_cellHeight;
	float m_agentHeight;
	float m_agentRadius;
	float m_agentMaxClimb;
	float m_agentMaxSlope;
	float m_regionMinSize;
	float m_regionMergeSize;
	float m_edgeMaxLen;
	float m_edgeMaxError;
	float m_vertsPerPoly;
	float m_detailSampleDist;
	float m_detailSampleMaxError;

	//int m_partitionType;

	RecastBuildContext* m_BuildContext;

	unsigned char* m_triareas;
	rcHeightfield* m_solid;
	rcCompactHeightfield* m_chf;
	rcContourSet* m_cset;
	rcPolyMesh* m_pmesh;
	rcConfig m_cfg;
	rcPolyMeshDetail* m_dmesh;

	static const int MAX_SMOOTH = 2048;
	static const int MAX_POLYS = 256;

	dtPolyRef m_startRef;
	dtPolyRef m_endRef;
	float m_startPos[3];
	float m_endPos[3];

	dtQueryFilter m_filter;
	//float m_smoothPath[MAX_SMOOTH * 3];
	vector<Vec3> m_smoothPath;

	float m_polyPickExt[3];

	dtPolyRef m_polys[256];
	int m_npolys;
	int m_nsmoothPath;

	unsigned char m_straightPathFlags[256];
	dtPolyRef m_straightPathPolys[256];
	float m_straightPath[MAX_POLYS * 3];
	vector<Vec3> m_vecstraightPath;
	int m_nstraightPath;

	vector<int> m_vecIndis;
	vector<float> m_vecVerts;

	vector<wstring> debugMeshNames;
private:
	void clean_up();
public:
	virtual dtNavMesh* getNavMesh() { return m_navMesh; }
	virtual dtNavMeshQuery* getNavMeshQuery() { return m_navQuery; }
	
	virtual float getAgentRadius() { return m_agentRadius; }
	virtual float getAgentHeight() { return m_agentHeight; }
	virtual float getAgentClimb() { return m_agentMaxClimb; }
	unsigned char getNavMeshDrawFlags() const { return m_navMeshDrawFlags; }
	void setNavMeshDrawFlags(unsigned char flags) { m_navMeshDrawFlags = flags; }

	bool getSteerTarget(dtNavMeshQuery* navQuery, const float* startPos, const float* endPos,
		const float minTargetDist,
		const dtPolyRef* path, const int pathSize,
		float* steerPos, unsigned char& steerPosFlag, dtPolyRef& steerPosRef,
		float* outPoints = 0, int* outPointCount = 0);
	int fixupCorridor(dtPolyRef* path, const int npath, const int maxPath,
		const dtPolyRef* visited, const int nvisited);
	int fixupShortcuts(dtPolyRef* path, int npath, dtNavMeshQuery* navQuery);
	inline bool inRange(const float* v1, const float* v2, const float r, const float h);
	const dtNavMesh* GetNavMeshc() { return m_navMesh; }
	//const float* GetFindPath() { return m_smoothPath; }
	vector<Vec3>& GetFindPath() { return m_smoothPath; }
	vector<Vec3>& GetStraightPath();

	void MakeQuery(dtNavMeshQuery* _Query);
	//float* GetPath() { return m_smoothPath; }
	int GetPathSize() { return m_nsmoothPath; }
	int GetStraightPathSize() { return m_nstraightPath; }
public:
	void HandleBuild(CGameObject* _Obj, const wstring& _wsMeshName);
	bool DoQuery(Vec3 vStartPos, Vec3 vEndPos);
	bool Raycast(Vec3 vRayStart, Vec3 vRayEnd, float* hitTime);
	Vec3 DoFindRandomId(bool bStart);
	//void LoadObj(const char* path);

public:
	RecastNavi();
	virtual ~RecastNavi();
};

