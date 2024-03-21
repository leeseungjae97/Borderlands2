#include "pch.h"
#include "RecastNavi.h"

#include "CGameObject.h"
#include "CLandScape.h"
#include "CMeshRender.h"
#include "CResMgr.h"
#include "CTransform.h"
#include "ptr.h"

static float frand()
{
	//	return ((float)(rand() & 0xffff)/(float)0xffff);
	return (float)rand() / (float)RAND_MAX;
}

RecastNavi::RecastNavi()
	: m_navMesh(nullptr)
	, m_navQuery(nullptr)
	, m_navMeshDrawFlags(0)
	, m_cellSize(20.0f), m_cellHeight(1.5f)
	, m_agentHeight(50.f), m_agentRadius(50.0f)
	, m_agentMaxClimb(50.0f), m_agentMaxSlope(40.0f)
	, m_regionMinSize(8), m_regionMergeSize(20)
	, m_edgeMaxLen(12.0f), m_edgeMaxError(1.3f)
	, m_vertsPerPoly(4.0f)
	, m_detailSampleDist(6.0f)
	, m_detailSampleMaxError(1.0f)
	, m_triareas(nullptr), m_solid(nullptr), m_chf(nullptr)
	, m_cset(nullptr), m_pmesh(nullptr), m_dmesh(nullptr)
	, m_cfg()
	, m_startRef(0)
	, m_endRef(0)
	, m_smoothPath{}, m_polys{}, m_npolys(0)
	, m_nsmoothPath(0)
{
	m_BuildContext = new RecastBuildContext;
	m_geom = new InputGeom;

	m_filter.setAreaCost(POLYAREA_GROUND, 1.0f);
	m_filter.setAreaCost(POLYAREA_WATER, 1.0f);
	m_filter.setAreaCost(POLYAREA_ROAD, 1.0f);
	m_filter.setAreaCost(POLYAREA_DOOR, 1.0f);
	m_filter.setAreaCost(POLYAREA_GRASS, 1.0f);
	m_filter.setAreaCost(POLYAREA_JUMP, 1.0f);

	m_polyPickExt[0] = 2;
	m_polyPickExt[1] = 4;
	m_polyPickExt[2] = 2;
}

RecastNavi::~RecastNavi()
{
	delete m_BuildContext;
	delete m_geom;
	clean_up();
}


void RecastNavi::clean_up()
{

	delete[] m_triareas;
	m_triareas = 0;
	rcFreeHeightField(m_solid);
	m_solid = 0;
	rcFreeCompactHeightfield(m_chf);
	m_chf = 0;
	rcFreeContourSet(m_cset);
	m_cset = 0;
	rcFreePolyMesh(m_pmesh);
	m_pmesh = 0;
	rcFreePolyMeshDetail(m_dmesh);
	m_dmesh = 0;
	dtFreeNavMesh(m_navMesh);
	m_navMesh = 0;
	dtFreeNavMeshQuery(m_navQuery);
	m_navQuery = 0;
}


bool RecastNavi::DoQuery(Vec3 vStartPos, Vec3 vEndPos)
{
	if (nullptr == m_navQuery)
		return false;

	m_startPos[0] = vStartPos.x;
	m_startPos[1] = vStartPos.y;
	m_startPos[2] = vStartPos.z;

	m_endPos[0] = vEndPos.x;
	m_endPos[1] = vEndPos.y;
	m_endPos[2] = vEndPos.z;

	m_startRef = 0;
	m_endRef = 0;

	m_navQuery->findNearestPoly(m_startPos, m_polyPickExt, &m_filter, &m_startRef, 0);
	m_navQuery->findNearestPoly(m_endPos, m_polyPickExt, &m_filter, &m_endRef, 0);

	m_vecstraightPath.clear();
	vector<Vec3>().swap(m_vecstraightPath);
	if (m_startPos && m_startRef && m_endRef)
	{
		m_navQuery->findPath(m_startRef, m_endRef, m_startPos, m_endPos, &m_filter, m_polys, &m_npolys, MAX_POLYS);
		m_nstraightPath = 0;
		if (m_npolys)
		{
			// 부분 경로의 경우 끝점이 마지막 다각형에 고정되어 있는지 확인.
			float epos[3];
			dtVcopy(epos, m_endPos);
			if (m_polys[m_npolys - 1] != m_endRef)
				m_navQuery->closestPointOnPoly(m_polys[m_npolys - 1], m_endPos, epos, 0);

			m_navQuery->findStraightPath(m_startPos, epos, m_polys, m_npolys,
				m_straightPath, m_straightPathFlags,
				m_straightPathPolys, &m_nstraightPath, MAX_POLYS, 0);

			for (int i = 0; i < m_nstraightPath * 3;)
			{
				Vec3 vPath;
				vPath.x = m_straightPath[i];
				vPath.y = m_straightPath[i + 1];
				vPath.z = m_straightPath[i + 2];
				i += 3;

				m_vecstraightPath.push_back(vPath);
			}

			return true;
		}
	}
	else
	{
		m_npolys = 0;
		m_nstraightPath = 0;
		return false;
	}
	{
		//if (m_startRef && m_endRef)
		//{
		//	m_navQuery->findPath(m_startRef, m_endRef, m_startPos, m_endPos, &m_filter, m_polys, &m_npolys, MAX_POLYS);
		//	m_nsmoothPath = 0;

		//	if (m_npolys)
		//	{
		//		dtPolyRef polys[MAX_POLYS];
		//		memcpy(polys, m_polys, sizeof(dtPolyRef) * m_npolys);
		//		int npolys = m_npolys;
		//		Vec3 vPath;

		//		float iterPos[3], targetPos[3];
		//		m_navQuery->closestPointOnPoly(m_startRef, m_startPos, iterPos, 0);
		//		m_navQuery->closestPointOnPoly(polys[npolys - 1], m_endPos, targetPos, 0);

		//		static const float STEP_SIZE = 0.5f;
		//		static const float SLOP = 0.01f;

		//		m_nsmoothPath = 0;

		//		vPath = Vec3(iterPos[0], iterPos[1], iterPos[2]);
		//		m_smoothPath.push_back(vPath);
		//		//dtVcopy(&m_smoothPath[m_nsmoothPath * 3], iterPos);
		//		m_nsmoothPath++;

		//		// Move towards target a small advancement at a time until target reached or
		//		// when ran out of memory to store the path.
		//		while (npolys && m_nsmoothPath < MAX_SMOOTH)
		//		{
		//			// Find location to steer towards.
		//			float steerPos[3];
		//			unsigned char steerPosFlag;
		//			dtPolyRef steerPosRef;

		//			if (!getSteerTarget(m_navQuery, iterPos, targetPos, SLOP,
		//				polys, npolys, steerPos, steerPosFlag, steerPosRef))
		//				break;

		//			bool endOfPath = (steerPosFlag & DT_STRAIGHTPATH_END) ? true : false;
		//			bool offMeshConnection = (steerPosFlag & DT_STRAIGHTPATH_OFFMESH_CONNECTION) ? true : false;

		//			// Find movement delta.
		//			float delta[3], len;
		//			dtVsub(delta, steerPos, iterPos);
		//			len = dtMathSqrtf(dtVdot(delta, delta));
		//			// If the steer target is end of path or off-mesh link, do not move past the location.
		//			if ((endOfPath || offMeshConnection) && len < STEP_SIZE)
		//				len = 1;
		//			else
		//				len = STEP_SIZE / len;
		//			float moveTgt[3];
		//			dtVmad(moveTgt, iterPos, delta, len);

		//			// Move
		//			float result[3];
		//			dtPolyRef visited[16];
		//			int nvisited = 0;
		//			m_navQuery->moveAlongSurface(polys[0], iterPos, moveTgt, &m_filter,
		//				result, visited, &nvisited, 16);

		//			npolys = fixupCorridor(polys, npolys, MAX_POLYS, visited, nvisited);
		//			npolys = fixupShortcuts(polys, npolys, m_navQuery);

		//			float h = 0;
		//			m_navQuery->getPolyHeight(polys[0], result, &h);
		//			result[1] = h;
		//			dtVcopy(iterPos, result);

		//			// Handle end of path and off-mesh links when close enough.
		//			if (endOfPath && inRange(iterPos, steerPos, SLOP, 1.0f))
		//			{
		//				// Reached end of path.
		//				dtVcopy(iterPos, targetPos);
		//				if (m_nsmoothPath < MAX_SMOOTH)
		//				{
		//					//dtVcopy(&m_smoothPath[m_nsmoothPath * 3], iterPos);
		//					Vec3 vPath = Vec3(iterPos[0], iterPos[1], iterPos[2]);
		//					m_smoothPath.push_back(vPath);
		//					//m_nsmoothPath++;
		//				}
		//				break;
		//			}
		//			else if (offMeshConnection && inRange(iterPos, steerPos, SLOP, 1.0f))
		//			{
		//				// Reached off-mesh connection.
		//				float startPos[3], endPos[3];

		//				// Advance the path up to and over the off-mesh connection.
		//				dtPolyRef prevRef = 0, polyRef = polys[0];
		//				int npos = 0;
		//				while (npos < npolys && polyRef != steerPosRef)
		//				{
		//					prevRef = polyRef;
		//					polyRef = polys[npos];
		//					npos++;
		//				}
		//				for (int i = npos; i < npolys; ++i)
		//					polys[i - npos] = polys[i];
		//				npolys -= npos;

		//				// Handle the connection.
		//				dtStatus status = m_navMesh->getOffMeshConnectionPolyEndPoints(prevRef, polyRef, startPos, endPos);
		//				if (dtStatusSucceed(status))
		//				{
		//					if (m_nsmoothPath < MAX_SMOOTH)
		//					{
		//						//dtVcopy(&m_smoothPath[m_nsmoothPath * 3], startPos);
		//						Vec3 vPath = Vec3(startPos[0], startPos[1], startPos[2]);
		//						m_smoothPath.push_back(vPath);
		//						m_nsmoothPath++;
		//						// Hack to make the dotted path not visible during off-mesh connection.
		//						if (m_nsmoothPath & 1)
		//						{
		//							//dtVcopy(&m_smoothPath[m_nsmoothPath * 3], startPos);
		//							Vec3 vPath = Vec3(startPos[0], startPos[1], startPos[2]);
		//							m_smoothPath.push_back(vPath);
		//							m_nsmoothPath++;
		//						}
		//					}
		//					// Move position at the other side of the off-mesh link.
		//					dtVcopy(iterPos, endPos);
		//					float eh = 0.0f;
		//					m_navQuery->getPolyHeight(polys[0], iterPos, &eh);
		//					iterPos[1] = eh;
		//				}
		//			}

		//			// Store results.
		//			if (m_nsmoothPath < MAX_SMOOTH)
		//			{
		//				//dtVcopy(&m_smoothPath[m_nsmoothPath * 3], iterPos);
		//				Vec3 vPath = Vec3(iterPos[0], iterPos[1], iterPos[2]);
		//				m_smoothPath.push_back(vPath);
		//				m_nsmoothPath++;
		//			}
		//		}
		//		return true;
		//	}

		//}
		//else
		//{
		//	m_npolys = 0;
		//	m_nsmoothPath = 0;
		//	return false;
		//}
	}

	return false;
}

bool RecastNavi::Raycast(Vec3 vRayStart, Vec3 vRayEnd, float* hitTime)
{
	float rayEnd[3];
	float rayStart[3];

	rayStart[0] = vRayStart.x;
	rayStart[1] = vRayStart.y;
	rayStart[2] = vRayStart.z;

	rayEnd[0] = vRayEnd.x;
	rayEnd[1] = vRayEnd.y;
	rayEnd[2] = vRayEnd.z;

	bool hit = m_geom->raycastMesh(rayStart, rayEnd, *hitTime);

	return hit;
}

Vec3 RecastNavi::DoFindRandomId(bool bStart)
{
	if(bStart)
	{
		dtStatus status = m_navQuery->findRandomPoint(&m_filter, frand, &m_startRef, m_startPos);
		if (dtStatusSucceed(status))
		{
			return Vec3(m_startPos[0], m_startPos[1], m_startPos[2]);
			int a = 0;
		}
	}
	else
	{
		dtStatus status = m_navQuery->findRandomPoint(&m_filter, frand, &m_endRef, m_endPos);
		if (dtStatusSucceed(status))
		{
			return Vec3(m_endPos[0], m_endPos[1], m_endPos[2]);
			int a = 0;
		}
	}
	return Vec3::Zero;
}

bool RecastNavi::getSteerTarget(dtNavMeshQuery* navQuery, const float* startPos, const float* endPos,
                                const float minTargetDist,
                                const dtPolyRef* path, const int pathSize,
                                float* steerPos, unsigned char& steerPosFlag, dtPolyRef& steerPosRef,
                                float* outPoints, int* outPointCount)
{
	// Find steer target.
	static const int MAX_STEER_POINTS = 3;
	float steerPath[MAX_STEER_POINTS * 3];
	unsigned char steerPathFlags[MAX_STEER_POINTS];
	dtPolyRef steerPathPolys[MAX_STEER_POINTS];
	int nsteerPath = 0;
	navQuery->findStraightPath(startPos, endPos, path, pathSize,
		steerPath, steerPathFlags, steerPathPolys, &nsteerPath, MAX_STEER_POINTS);
	if (!nsteerPath)
		return false;

	if (outPoints && outPointCount)
	{
		*outPointCount = nsteerPath;
		for (int i = 0; i < nsteerPath; ++i)
			dtVcopy(&outPoints[i * 3], &steerPath[i * 3]);
	}


	// Find vertex far enough to steer to.
	int ns = 0;
	while (ns < nsteerPath)
	{
		// Stop at Off-Mesh link or when point is further than slop away.
		if ((steerPathFlags[ns] & DT_STRAIGHTPATH_OFFMESH_CONNECTION) ||
			!inRange(&steerPath[ns * 3], startPos, minTargetDist, 1000.0f))
			break;
		ns++;
	}
	// Failed to find good point to steer to.
	if (ns >= nsteerPath)
		return false;

	dtVcopy(steerPos, &steerPath[ns * 3]);
	steerPos[1] = startPos[1];
	steerPosFlag = steerPathFlags[ns];
	steerPosRef = steerPathPolys[ns];

	return true;
}

inline bool RecastNavi::inRange(const float* v1, const float* v2, const float r, const float h)
{
	const float dx = v2[0] - v1[0];
	const float dy = v2[1] - v1[1];
	const float dz = v2[2] - v1[2];
	return (dx * dx + dz * dz) < r * r && fabsf(dy) < h;
}


int RecastNavi::fixupCorridor(dtPolyRef* path, const int npath, const int maxPath,
	const dtPolyRef* visited, const int nvisited)
{
	int furthestPath = -1;
	int furthestVisited = -1;

	// Find furthest common polygon.
	for (int i = npath - 1; i >= 0; --i)
	{
		bool found = false;
		for (int j = nvisited - 1; j >= 0; --j)
		{
			if (path[i] == visited[j])
			{
				furthestPath = i;
				furthestVisited = j;
				found = true;
			}
		}
		if (found)
			break;
	}

	// If no intersection found just return current path. 
	if (furthestPath == -1 || furthestVisited == -1)
		return npath;

	// Concatenate paths.	

	// Adjust beginning of the buffer to include the visited.
	const int req = nvisited - furthestVisited;
	const int orig = rcMin(furthestPath + 1, npath);
	int size = rcMax(0, npath - orig);
	if (req + size > maxPath)
		size = maxPath - req;
	if (size)
		memmove(path + req, path + orig, size * sizeof(dtPolyRef));

	// Store visited
	for (int i = 0; i < req; ++i)
		path[i] = visited[(nvisited - 1) - i];

	return req + size;
}

// 이 함수는 경로에 작은 유턴이 있는지 확인합니다. 즉,
// 경로에서 더 멀리 있는 다각형은 첫 번째 다각형에 인접합니다.
// 경로에 있습니다. 그런 일이 발생하면 바로 가기가 사용됩니다.
// 대상(T) 위치가 타일 경계에 있는 경우 이런 일이 발생할 수 있습니다.
// 그리고 우리는 (S) 타일 가장자리에 평행하게 접근하고 있습니다.
// 정점에서의 선택은 임의적일 수 있습니다.
//  +---+---+
//  |:::|:::|
//  +-S-+-T-+
//  |:::|   | <-- 계단이 여기서 끝나서 U턴 경로가 될 수 있습니다.
//  +---+---+
int RecastNavi::fixupShortcuts(dtPolyRef* path, int npath, dtNavMeshQuery* navQuery)
{
	if (npath < 3)
		return npath;

	// Get connected polygons
	static const int maxNeis = 16;
	dtPolyRef neis[maxNeis];
	int nneis = 0;

	const dtMeshTile* tile = 0;
	const dtPoly* poly = 0;
	if (dtStatusFailed(navQuery->getAttachedNavMesh()->getTileAndPolyByRef(path[0], &tile, &poly)))
		return npath;

	for (unsigned int k = poly->firstLink; k != DT_NULL_LINK; k = tile->links[k].next)
	{
		const dtLink* link = &tile->links[k];
		if (link->ref != 0)
		{
			if (nneis < maxNeis)
				neis[nneis++] = link->ref;
		}
	}

	// If any of the neighbour polygons is within the next few polygons
	// in the path, short cut to that polygon directly.
	static const int maxLookAhead = 6;
	int cut = 0;
	for (int i = dtMin(maxLookAhead, npath) - 1; i > 1 && cut == 0; i--)
	{
		for (int j = 0; j < nneis; j++)
		{
			if (path[i] == neis[j])
			{
				cut = i;
				break;
			}
		}
	}
	if (cut > 1)
	{
		int offset = cut - 1;
		npath -= offset;
		for (int i = 1; i < npath; i++)
			path[i] = path[i + offset];
	}

	return npath;
}


vector<Vec3>& RecastNavi::GetStraightPath()
{
	return m_vecstraightPath;
}

void RecastNavi::MakeQuery(dtNavMeshQuery* _Query)
{
	dtStatus status = _Query->init(m_navMesh, 2048);
	if (dtStatusFailed(status))
	{

		//return false;
	}

}

void RecastNavi::HandleBuild(CGameObject* _Obj)
{
	clean_up();

	if (!m_geom->CreateGeom(_Obj->MeshRender()->GetMesh(), _Obj->Transform()->GetWorldMat(), _Obj->Transform()->GetRelativeScale()))
	{
		assert(nullptr);
	}

	if (!m_geom)
	{
		assert(nullptr);
	}

	m_navQuery = dtAllocNavMeshQuery();

	{
		//DeleteTestObject();

		//float bmin[3]{};
		//float bmax[3]{};

		//float* verts = nullptr;
		//int nverts = 0;

		//int* tris = nullptr;
		//int ntris = 0;

		//Ptr<CMesh> _pMesh = _Obj->MeshRender()->GetMesh();
		//Matrix mWorldMat = _Obj->Transform()->GetWorldMat();
		//Vtx* vv = _pMesh->GetVtxSysMem();

		//int vertexCount = _pMesh->GetVtxCount();

		//vector<float> vertices;
		//vector<int> indices;
		//for (int i = 0; i < vertexCount; ++i)
		//{
		//	Vec3 vPos = vv[i].vPos;
		//	vPos = XMVector3TransformCoord(Vec3(vPos), mWorldMat);
		//	vertices.emplace_back(vPos.x);
		//	vertices.emplace_back(vPos.y);
		//	vertices.emplace_back(vPos.z);
		//}
		//vector<tIndexInfo> vecIdxInfo = _pMesh->GetIndexInfo();
		//for (int i = 0; i < vecIdxInfo.size(); ++i)
		//{
		//	vector<UINT> inds;
		//	inds.resize(vecIdxInfo[i].iIdxCount);
		//	memcpy(inds.data(), vecIdxInfo[i].pIdxSysMem, vecIdxInfo[i].iIdxCount * sizeof(UINT));
		//	for (int j = 0; j < vecIdxInfo[i].iIdxCount; ++j)
		//	{
		//		indices.emplace_back(inds[j]);
		//	}
		//}
		//
		//verts = vertices.data();
		//nverts = vertices.size() / 3;

		//tris = indices.data();
		//ntris = indices.size() / 3;

		//rcCalcBounds(verts, nverts, bmin, bmax);
	}

	const float* bmin = m_geom->getMeshBoundsMin();
	const float* bmax = m_geom->getMeshBoundsMax();
	const float* verts = m_geom->getVerts();
	const int nverts = m_geom->getVertCount();
	const int* tris = m_geom->getTris();
	const int ntris = m_geom->getTriCount();

	memset(&m_cfg, 0, sizeof(m_cfg));
	m_cfg.cs = m_cellSize;
	m_cfg.ch = m_cellHeight;
	m_cfg.walkableSlopeAngle = m_agentMaxSlope;
	m_cfg.walkableHeight = (int)ceilf(m_agentHeight / m_cfg.ch);
	m_cfg.walkableClimb = (int)floorf(m_agentMaxClimb / m_cfg.ch);
	m_cfg.walkableRadius = (int)ceilf(m_agentRadius / m_cfg.cs);
	m_cfg.maxEdgeLen = (int)(m_edgeMaxLen / m_cellSize);
	m_cfg.maxSimplificationError = m_edgeMaxError;
	m_cfg.minRegionArea = (int)rcSqr(m_regionMinSize);	
	m_cfg.mergeRegionArea = (int)rcSqr(m_regionMergeSize);
	m_cfg.maxVertsPerPoly = (int)m_vertsPerPoly;
	m_cfg.detailSampleDist = m_detailSampleDist < 0.9f ? 0 : m_cellSize * m_detailSampleDist;
	m_cfg.detailSampleMaxError = m_cellHeight * m_detailSampleMaxError;

	// 네비게이션 빌드될 공간
	// 사용될 매쉬 바운드
	rcVcopy(m_cfg.bmin, bmin);
	rcVcopy(m_cfg.bmax, bmax);
	rcCalcGridSize(m_cfg.bmin, m_cfg.bmax, m_cfg.cs, &m_cfg.width, &m_cfg.height);

	m_solid = rcAllocHeightfield();
	if (!rcCreateHeightfield(m_BuildContext, *m_solid, m_cfg.width, m_cfg.height, m_cfg.bmin, m_cfg.bmax, m_cfg.cs, m_cfg.ch))
	{
		OutputDebugStringW(L"Create Height Field Fail\r\n");
		assert(nullptr);
	}

	m_triareas = new unsigned char[ntris];
	memset(m_triareas, 0, ntris * sizeof(unsigned char));
	rcMarkWalkableTriangles(m_BuildContext
		, m_cfg.walkableSlopeAngle
		, verts
		, nverts
		, tris
		, ntris
		, m_triareas);

	if (!rcRasterizeTriangles(m_BuildContext
		, verts
		, nverts
		, tris
		, m_triareas
		, ntris
		, *m_solid
		, m_cfg.walkableClimb))
	{
		OutputDebugStringW(L"Rasterize Triangles Fail\r\n");
		assert(nullptr);
	}

	// soild 필터링
	rcFilterLowHangingWalkableObstacles(m_BuildContext, m_cfg.walkableClimb, *m_solid);
	rcFilterLedgeSpans(m_BuildContext, m_cfg.walkableHeight, m_cfg.walkableClimb, *m_solid);
	rcFilterWalkableLowHeightSpans(m_BuildContext, m_cfg.walkableHeight, *m_solid);


	// 높이 필드 압축
	m_chf = rcAllocCompactHeightfield();
	if (nullptr == m_chf)
	{
		OutputDebugStringW(L"m_chf Fail\r\n");
		assert(nullptr);
	}
	if (!rcBuildCompactHeightfield(m_BuildContext, m_cfg.walkableHeight, m_cfg.walkableClimb, *m_solid, *m_chf))
	{
		OutputDebugStringW(L"Build Compact Height Field Fail\r\n");
		assert(nullptr);
	}

	// 설정된 radius에 따라 걸을 수 있는 곳인지 설정
	if (!rcErodeWalkableArea(m_BuildContext, m_cfg.walkableRadius, *m_chf))
	{
		OutputDebugStringW(L"Erode Walkable Area Fail\r\n");
		assert(nullptr);
	}

	// 필드 분할 알고리즘 Watershed partitioning
	if (!rcBuildDistanceField(m_BuildContext, *m_chf))
	{
		OutputDebugStringW(L"Build Distance Field Fail\r\n");
		assert(nullptr);
	}

	// 보행 가능한 표면을 구멍이 없는 단순한 영역으로 분할.
	if (!rcBuildRegions(m_BuildContext, *m_chf, 0, m_cfg.minRegionArea, m_cfg.mergeRegionArea))
	{
		OutputDebugStringW(L"Build Regions Fail\r\n");
		assert(nullptr);
	}


	m_cset = rcAllocContourSet();
	if (!m_cset)
	{
		assert(nullptr);
	}

	if (!rcBuildContours(m_BuildContext, *m_chf, m_cfg.maxSimplificationError, m_cfg.maxEdgeLen, *m_cset))
	{
		OutputDebugStringW(L"Build Contours Fail\r\n");
		assert(nullptr);
	}

	// contours로 polygon mesh 생성
	m_pmesh = rcAllocPolyMesh();
	if (!m_pmesh)
	{
		assert(nullptr);
	}

	if (!rcBuildPolyMesh(m_BuildContext, *m_cset, m_cfg.maxVertsPerPoly, *m_pmesh))
	{
		OutputDebugStringW(L"Build Poly Mesh Fail\r\n");
		assert(nullptr);
	}

	// 각 다각형의 대략적인 높이에 접근할 수 있는 디테일 메시 생성.
	m_dmesh = rcAllocPolyMeshDetail();
	if (!m_dmesh)
	{
		assert(nullptr);
		//return false;
	}

	if (!rcBuildPolyMeshDetail(m_BuildContext, *m_pmesh, *m_chf, m_cfg.detailSampleDist, m_cfg.detailSampleMaxError, *m_dmesh))
	{
		assert(nullptr);
		//return false;
	}
	// Recast 폴리 메시에서 우회 데이터를 생성.
	// GUI는 우회가 처리할 수 있는 것보다 더 많은 다각형당 최대 포인트를 허용할 수 있음.
	// 제한(6)을 초과하지 않는 경우에만 우회 내비메시를 빌드 
	unsigned char* navData = 0;
	int navDataSize = 0;

	for (int i = 0; i < m_pmesh->npolys; ++i)
	{
		if (m_pmesh->areas[i] == RC_WALKABLE_AREA)
			m_pmesh->areas[i] = POLYAREA_GROUND;

		if (m_pmesh->areas[i] == POLYAREA_GROUND ||
			m_pmesh->areas[i] == POLYAREA_GRASS ||
			m_pmesh->areas[i] == POLYAREA_ROAD)
		{
			m_pmesh->flags[i] = POLYFLAGS_WALK;
		}
		else if (m_pmesh->areas[i] == POLYAREA_WATER)
		{
			m_pmesh->flags[i] = POLYFLAGS_SWIM;
		}
		else if (m_pmesh->areas[i] == POLYAREA_DOOR)
		{
			m_pmesh->flags[i] = POLYFLAGS_WALK | POLYFLAGS_DOOR;
		}
	}

	dtNavMeshCreateParams params;
	memset(&params, 0, sizeof(params));
	params.verts = m_pmesh->verts;
	params.vertCount = m_pmesh->nverts;
	params.polys = m_pmesh->polys;
	params.polyAreas = m_pmesh->areas;
	params.polyFlags = m_pmesh->flags;
	params.polyCount = m_pmesh->npolys;
	params.nvp = m_pmesh->nvp;
	params.detailMeshes = m_dmesh->meshes;
	params.detailVerts = m_dmesh->verts;
	params.detailVertsCount = m_dmesh->nverts;
	params.detailTris = m_dmesh->tris;
	params.detailTriCount = m_dmesh->ntris;

	params.offMeshConVerts = m_geom->getOffMeshConnectionVerts();
	params.offMeshConRad = m_geom->getOffMeshConnectionRads();
	params.offMeshConDir = m_geom->getOffMeshConnectionDirs();
	params.offMeshConAreas = m_geom->getOffMeshConnectionAreas();
	params.offMeshConFlags = m_geom->getOffMeshConnectionFlags();
	params.offMeshConUserID = m_geom->getOffMeshConnectionId();
	params.offMeshConCount = m_geom->getOffMeshConnectionCount();

	params.walkableHeight = m_agentHeight;
	params.walkableRadius = m_agentRadius;
	params.walkableClimb = m_agentMaxClimb;
	rcVcopy(params.bmin, m_pmesh->bmin);
	rcVcopy(params.bmax, m_pmesh->bmax);
	params.cs = m_cfg.cs;
	params.ch = m_cfg.ch;
	params.buildBvTree = true;

	if (!dtCreateNavMeshData(&params, &navData, &navDataSize))
	{
		m_BuildContext->log(RC_LOG_ERROR, "Could not build Detour navmesh.");
		assert(nullptr);
	}

	m_navMesh = dtAllocNavMesh();
	if (!m_navMesh)
	{
		dtFree(navData);
		m_BuildContext->log(RC_LOG_ERROR, "Could not create Detour navmesh");
		assert(nullptr);
	}

	dtStatus status;

	status = m_navMesh->init(navData, navDataSize, DT_TILE_FREE_DATA);
	if (dtStatusFailed(status))
	{
		dtFree(navData);
		m_BuildContext->log(RC_LOG_ERROR, "Could not init Detour navmesh");
		assert(nullptr);
	}

	status = m_navQuery->init(m_navMesh, 2048);
	if (dtStatusFailed(status))
	{
		m_BuildContext->log(RC_LOG_ERROR, "Could not init Detour navmesh query");
		assert(nullptr);
		//return false;
	}

	const int nvp = m_pmesh->nvp;
	const float cs = m_pmesh->cs;
	const float ch = m_pmesh->ch;
	const float* orig = m_pmesh->bmin;

	vector<Vtx> vecVer;
	vector<int> vecIdx;
	Vtx vertex;
	int idxIdx = 0;
	Matrix World = _Obj->Transform()->GetWorldMat();
	Matrix invWorld = _Obj->Transform()->GetWorldInvMat();
	for (int i = 0; i < m_pmesh->npolys; ++i)
	{
		const unsigned short* p = &m_pmesh->polys[i * nvp * 2];
		const unsigned char area = m_pmesh->areas[i];

		unsigned short vi[3];
		for (int j = 2; j < nvp; ++j)
		{
			if (p[j] == RC_MESH_NULL_IDX) break;
			vi[0] = p[0];
			vi[1] = p[j - 1];
			vi[2] = p[j];
			for (int k = 0; k < 3; ++k)
			{
				const unsigned short* v = &m_pmesh->verts[vi[k] * 3];
				const float x = orig[0] + v[0] * cs;
				const float y = orig[1] + (v[1] + 1) * ch;
				const float z = orig[2] + v[2] * cs;
				vertex.vPos.x = orig[0] + v[0] * cs;
				vertex.vPos.y = orig[1] + (v[1] + 1) * ch;
				vertex.vPos.z = orig[2] + v[2] * cs;
				vertex.vPos = XMVector3TransformCoord(vertex.vPos, invWorld);
				vecVer.push_back(vertex);
				vecIdx.push_back(idxIdx++);
			}
		}
	}

	Ptr<CMesh> convertCMesh = new CMesh;
	convertCMesh->Create(vecVer.data(), vecVer.size(), vecIdx.data(), vecIdx.size());
	CResMgr::GetInst()->AddRes<CMesh>(L"NaviMesh", convertCMesh);
	//DrawDebugMesh(World, L"NaviMesh", 1, Vec4(1.f, 1.f, 1.f, 1.f), 100000000000000.f, true);

	{
		//const dtNavMesh* navMesh = GetNavMeshc();
	//Vtx v;
	//Matrix invWorld = _Obj->Transform()->GetWorldInvMat();
	//Matrix World = _Obj->Transform()->GetWorldMat();
	//vector<Vtx> vecVer;
	//vector<int> vecIdx;
	//int k = 0;
	//for (int i = 0; i < navMesh->getMaxTiles(); ++i)
	//{
	//	const dtMeshTile* tile = navMesh->getTile(i);
	//	if (!tile->header)
	//	{
	//		continue;
	//	}
	//	for (int polyIdx = 0; polyIdx < tile->header->polyCount; ++polyIdx)
	//	{
	//		const dtPoly* poly = &tile->polys[polyIdx];
	//		//if (poly->getType() == DT_POLYTYPE_OFFMESH_CONNECTION)
	//		//{
	//		//	continue;
	//		//}

	//		const dtPolyDetail* polydetail = &tile->detailMeshes[polyIdx];
	//		
	//		for (int triIdx = 0; triIdx < polydetail->triCount; ++triIdx)
	//		{
	//			unsigned char* triangle = &tile->detailTris[(polydetail->triBase + triIdx) * 4];

	//			//vector<Vtx> vecVer;
	//			//vector<int> vecIdx;

	//			for (int vertex = 0; vertex < 3; ++vertex)
	//			{
	//				unsigned char vtxValue = triangle[vertex];
	//				

	//				float* vtxPos = nullptr;
	//				if (vtxValue < poly->vertCount)
	//				{
	//					//indies.push_back(poly->verts[vtxValue] * 3);
	//					vtxPos = &tile->verts[poly->verts[vtxValue] * 3];
	//				}
	//				else
	//				{
	//					//indies.push_back((polydetail->vertBase + vtxValue - poly->vertCount) * 3);
	//					vtxPos = &tile->detailVerts[(polydetail->vertBase + vtxValue - poly->vertCount) * 3];
	//				}
	//				Vec3 mV = XMVector3TransformCoord(Vec3(vtxPos[0], vtxPos[1], vtxPos[2]), invWorld);
	//				v.vPos = mV;
	//				v.vNormal = Vec3(0.f, 1.f, 0.f);
	//				v.vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	//				vecVer.push_back(v);
	//			}
	//			vecIdx.push_back(k++);
	//			vecIdx.push_back(k++);
	//			vecIdx.push_back(k++);
	//			//Vec3 pos = Vec3(vecVer[0].vPos.x, vecVer[0].vPos.y, vecVer[0].vPos.z);
	//			//verties.push_back(pos);

	//			//Ptr<CMesh> convertCMesh = new CMesh;
	//			//convertCMesh->Create(vecVer.data(), vecVer.size(), vecIdx.data(), vecIdx.size());
	//			//CResMgr::GetInst()->AddRes<CMesh>(L"convertMesh" + std::to_wstring(i) + std::to_wstring(polyIdx) + std::to_wstring(triIdx), convertCMesh);
	//			////debugMeshNames.push_back(L"convertMesh" + std::to_wstring(i) + std::to_wstring(polyIdx) + std::to_wstring(triIdx));
	//			//DrawDebugMesh(World, L"convertMesh" + std::to_wstring(i) + std::to_wstring(polyIdx) + std::to_wstring(triIdx), 1, Vec4(1.f, 1.f, 1.f, 1.f), 100000000000000.f, false);
	//			//DrawDebugMesh();
	//			//TestSpawnGameObject(pos, 1, convertCMesh);
	//		}
	//	}
	//}
	//Ptr<CMesh> convertCMesh = new CMesh;
	//convertCMesh->Create(vecVer.data(), vecVer.size(), vecIdx.data(), vecIdx.size());
	//CResMgr::GetInst()->AddRes<CMesh>(L"NaviMesh", convertCMesh);
	////debugMeshNames.push_back(L"convertMesh" + std::to_wstring(i) + std::to_wstring(polyIdx) + std::to_wstring(triIdx));
	//DrawDebugMesh(World, L"NaviMesh", 1, Vec4(1.f, 1.f, 1.f, 1.f), 100000000000000.f, true);
	}
}
