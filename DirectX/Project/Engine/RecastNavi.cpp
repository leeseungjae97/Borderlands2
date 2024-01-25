#include "pch.h"
#include "RecastNavi.h"

#include "CGameObject.h"
#include "CLandScape.h"
#include "CResMgr.h"
#include "CTransform.h"
#include "ptr.h"

RecastNavi::RecastNavi()
	: m_cellSize(0.3f)
	, m_cellHeight(0.2f)
	, m_agentHeight(2.0f)
	, m_agentRadius(0.6f)
	, m_agentMaxClimb(5.0f)
	, m_agentMaxSlope(90.0f)
	, m_regionMinSize(20)
	, m_regionMergeSize(20)
	, m_edgeMaxLen(12.0f)
	, m_edgeMaxError(1.3f)
	, m_vertsPerPoly(4.0f)
	, m_detailSampleDist(6.0f)
	, m_detailSampleMaxError(1.0f)
{
	m_ctx = new RecastBuildContext;
}

RecastNavi::~RecastNavi()
{
	delete m_ctx;
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
}

void RecastNavi::HandleBuild(CGameObject* _Obj)
{
	clean_up();
	DeleteTestObject();

	float bmin[3]{};
	float bmax[3]{};

	float* verts = nullptr;
	int nverts = 0;

	int* tris = nullptr;
	int ntris = 0;

	Ptr<CMesh> _pMesh = _Obj->LandScape()->GetMesh();
	Matrix mWorldMat = _Obj->Transform()->GetWorldMat();
	Vtx* vv = _pMesh->GetVtxSysMem();

	int vertexCount = _pMesh->GetVtxCount();

	vector<float> vertices;
	vector<int> indices;
	for (int i = 0; i < vertexCount; ++i)
	{
		Vec3 vPos = vv[i].vPos;
		vPos = XMVector3TransformCoord(Vec3(vPos), mWorldMat);
		if(vPos.y < 0)
		{
			int a = 0;
		}
		vertices.emplace_back(vPos.x);
		vertices.emplace_back(vPos.y);
		vertices.emplace_back(vPos.z);
	}
	vector<tIndexInfo> vecIdxInfo = _pMesh->GetIndexInfo();
	for (int i = 0; i < vecIdxInfo.size(); ++i)
	{
		//vector<UINT*> inds((UINT*)vecIdxInfo[i].pIdxSysMem, (UINT*)vecIdxInfo[i].pIdxSysMem + vecIdxInfo[i].iIdxCount);

		vector<UINT> inds;
		inds.resize(vecIdxInfo[i].iIdxCount);
		//vector<UINT>* inds = reinterpret_cast<vector<UINT>*>(vecIdxInfo[i].pIdxSysMem);
		memcpy(inds.data(), vecIdxInfo[i].pIdxSysMem, vecIdxInfo[i].iIdxCount * sizeof(UINT));
		for (int j = 0; j < vecIdxInfo[i].iIdxCount; ++j)
		{
			indices.emplace_back(inds[j]);
		}
	}

	verts = vertices.data();
	nverts = vertices.size() / 3;

	tris = indices.data();
	ntris = indices.size() / 3;

	rcCalcBounds(verts, nverts, bmin, bmax);

	memset(&m_cfg, 0, sizeof(m_cfg));
	m_cfg.cs = m_cellSize;
	m_cfg.ch = m_cellHeight;
	m_cfg.walkableSlopeAngle = m_agentMaxSlope;
	m_cfg.walkableHeight = (int)ceilf(m_agentHeight / m_cfg.ch);
	m_cfg.walkableClimb = (int)floorf(m_agentMaxClimb / m_cfg.ch);
	m_cfg.walkableRadius = (int)ceilf(m_agentRadius / m_cfg.cs);
	m_cfg.maxEdgeLen = (int)(m_edgeMaxLen / m_cellSize);
	m_cfg.maxSimplificationError = m_edgeMaxError;
	m_cfg.minRegionArea = (int)rcSqr(m_regionMinSize);		// Note: area = size*size
	m_cfg.mergeRegionArea = (int)rcSqr(m_regionMergeSize);	// Note: area = size*size
	m_cfg.maxVertsPerPoly = (int)m_vertsPerPoly;
	m_cfg.detailSampleDist = m_detailSampleDist < 0.9f ? 0 : m_cellSize * m_detailSampleDist;
	m_cfg.detailSampleMaxError = m_cellHeight * m_detailSampleMaxError;

	// 네비게이션 빌드될 공간
	// 사용될 매쉬 바운드
	rcVcopy(m_cfg.bmin, bmin);
	rcVcopy(m_cfg.bmax, bmax);
	rcCalcGridSize(m_cfg.bmin, m_cfg.bmax, m_cfg.cs, &m_cfg.width, &m_cfg.height);

	m_solid = rcAllocHeightfield();
	if (!rcCreateHeightfield(m_ctx, *m_solid, m_cfg.width, m_cfg.height, m_cfg.bmin, m_cfg.bmax, m_cfg.cs, m_cfg.ch))
	{
		OutputDebugStringW(L"Create Height Field Fail\r\n");
	}

	m_triareas = new unsigned char[ntris];
	memset(m_triareas, 0, ntris * sizeof(unsigned char));
	rcMarkWalkableTriangles(m_ctx
		, m_cfg.walkableSlopeAngle
		, verts
		, nverts
		, tris
		, ntris
		, m_triareas);
	if (!rcRasterizeTriangles(m_ctx
		, verts
		, nverts
		, tris
		, m_triareas
		, ntris
		, *m_solid
		, m_cfg.walkableClimb))
	{
		OutputDebugStringW(L"Rasterize Triangles Fail\r\n");
	}

	// soild 필터링
	rcFilterLowHangingWalkableObstacles(m_ctx, m_cfg.walkableClimb, *m_solid);
	rcFilterLedgeSpans(m_ctx, m_cfg.walkableHeight, m_cfg.walkableClimb, *m_solid);
	rcFilterWalkableLowHeightSpans(m_ctx, m_cfg.walkableHeight, *m_solid);


	// 높이 필드 압축
	m_chf = rcAllocCompactHeightfield();
	if (nullptr == m_chf)
	{
		OutputDebugStringW(L"m_chf Fail\r\n");
		assert(nullptr);
	}
	if (!rcBuildCompactHeightfield(m_ctx, m_cfg.walkableHeight, m_cfg.walkableClimb, *m_solid, *m_chf))
	{
		OutputDebugStringW(L"Build Compact Height Field Fail\r\n");
		assert(nullptr);
	}

	// 설정된 radius에 따라 걸을 수 있는 곳인지 설정
	if (!rcErodeWalkableArea(m_ctx, m_cfg.walkableRadius, *m_chf))
	{
		OutputDebugStringW(L"Erode Walkable Area Fail\r\n");
		assert(nullptr);
	}

	//const ConvexVolume* vols = m_geom->getConvexVolumes();
	//for (int i = 0; i < m_geom->getConvexVolumeCount(); ++i)
	//	rcMarkConvexPolyArea(m_ctx, vols[i].verts, vols[i].nverts, vols[i].hmin, vols[i].hmax, (unsigned char)vols[i].area, *m_chf);

	// 필드 분할 알고리즘 선택 : Watershed partitioning
	// Prepare for region partitioning, by calculating distance field along the walkable surface.
	if (!rcBuildDistanceField(m_ctx, *m_chf))
	{
		OutputDebugStringW(L"Build Distance Field Fail\r\n");
		assert(nullptr);
	}

	// Partition the walkable surface into simple regions without holes.
	if (!rcBuildRegions(m_ctx, *m_chf, 0, m_cfg.minRegionArea, m_cfg.mergeRegionArea))
	{
		OutputDebugStringW(L"Build Regions Fail\r\n");
		assert(nullptr);
	}


	m_cset = rcAllocContourSet();

	if (!rcBuildContours(m_ctx, *m_chf, m_cfg.maxSimplificationError, m_cfg.maxEdgeLen, *m_cset))
	{
		OutputDebugStringW(L"Build Contours Fail\r\n");
		assert(nullptr);
	}

	// contours로 polygon mesh 생성
	m_pmesh = rcAllocPolyMesh();

	if (!rcBuildPolyMesh(m_ctx, *m_cset, m_cfg.maxVertsPerPoly, *m_pmesh))
	{
		OutputDebugStringW(L"Build Poly Mesh Fail\r\n");
		assert(nullptr);
	}

	// polygon height에 접근하기 위한 mesh
	m_dmesh = rcAllocPolyMeshDetail();


	unsigned char* navData = 0;
	int navDataSize = 0;

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

	//params.offMeshConVerts = m_geom->getOffMeshConnectionVerts();
	//params.offMeshConRad = m_geom->getOffMeshConnectionRads();
	//params.offMeshConDir = m_geom->getOffMeshConnectionDirs();
	//params.offMeshConAreas = m_geom->getOffMeshConnectionAreas();
	//params.offMeshConFlags = m_geom->getOffMeshConnectionFlags();
	//params.offMeshConUserID = m_geom->getOffMeshConnectionId();
	//params.offMeshConCount = m_geom->getOffMeshConnectionCount();

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
		m_ctx->log(RC_LOG_ERROR, "Could not build Detour navmesh.");
		assert(nullptr);
	}

	m_navMesh = dtAllocNavMesh();
	if (!m_navMesh)
	{
		dtFree(navData);
		m_ctx->log(RC_LOG_ERROR, "Could not create Detour navmesh");
		assert(nullptr);
	}

	dtStatus status;

	status = m_navMesh->init(navData, navDataSize, DT_TILE_FREE_DATA);
	if (dtStatusFailed(status))
	{
		dtFree(navData);
		m_ctx->log(RC_LOG_ERROR, "Could not init Detour navmesh");
		assert(nullptr);
	}

	//status = m_navQuery->init(m_navMesh, 2048);
	//if (dtStatusFailed(status))
	//{
	//	m_ctx->log(RC_LOG_ERROR, "Could not init Detour navmesh query");
	//	return false;
	//}

	Matrix invWorld = _Obj->Transform()->GetWorldInvMat();
	const dtNavMesh* navMesh = GetNavMeshc();
	Vtx v;
	v.vNormal = Vec3(0.f, 0.f, -1.f);
	v.vTangent = Vec3(1.f, 0.f, 0.f);
	v.vBinormal = Vec3(0.f, -1.f, 0.f);
	for (int i = 0; i < navMesh->getMaxTiles(); ++i)
	{
		const dtMeshTile* tile = navMesh->getTile(i);
		if (!tile->header)
		{
			continue;
		}
		for (int polyIdx = 0; polyIdx < tile->header->polyCount; ++polyIdx)
		{

			const dtPoly* poly = &tile->polys[polyIdx];

			//if (poly->getType() == DT_POLYTYPE_OFFMESH_CONNECTION)
			//{
			//	continue;
			//}

			const dtPolyDetail* polydetail = &tile->detailMeshes[polyIdx];

			for (int triIdx = 0; triIdx < polydetail->triCount; ++triIdx)
			{
				unsigned char* triangle = &tile->detailTris[(polydetail->triBase + triIdx) * 4];

				vector<Vtx> vecVer;
				vector<int> vecIdx;
				Vec3 worldPos;

				for (int vertex = 0; vertex < 3; ++vertex)
				{
					unsigned char vtxValue = triangle[vertex];
					float* vtxPos = nullptr;
					if (vtxValue < poly->vertCount)
					{
						vtxPos = &tile->verts[poly->verts[vtxValue] * 3];
					}
					else
					{
						vtxPos = &tile->detailVerts[(polydetail->vertBase + vtxValue - poly->vertCount) * 3];
					}
					Vec3 mV = XMVector3TransformCoord(Vec3(vtxPos[0], vtxPos[1], vtxPos[2]), invWorld);
					//v.vPos.x = vtxPos[0];
					//v.vPos.y = vtxPos[1];
					//v.vPos.z = vtxPos[2];
					v.vPos = mV;
					v.vNormal = Vec3(0.f, 1.f, 0.f);
					v.vColor = Vec4(1.f, 0.f, 0.f, 1.f);
					if(vertex == 0)
						worldPos = Vec3(vtxPos[0], vtxPos[1], vtxPos[2]);
					vecVer.push_back(v);
				}
				//Vtx tmp = vecVer[1];
				//vecVer[1] = vecVer[2];
				//vecVer[2] = tmp;
				//for (int idxIdx = 0; idxIdx < poly->vertCount; ++idxIdx)
				//{
				//	
				//}
				vecIdx.push_back(0);
				vecIdx.push_back(1);
				vecIdx.push_back(2);

				Vec3 pos = Vec3(vecVer[0].vPos.x, vecVer[0].vPos.y, vecVer[0].vPos.z);

				Ptr<CMesh> convertCMesh = new CMesh(true);
				convertCMesh->Create(vecVer.data(), vecVer.size(), vecIdx.data(), vecIdx.size());
				//CResMgr::GetInst()->AddRes<CMesh>(L"convertMesh" + std::to_wstring(i), convertCMesh);
				TestSpawnGameObject(pos, 1, convertCMesh);
			}
		}
	}
}

dtNavMesh* RecastNavi::LoadNavMesh(const wstring& path)
{
	FILE* fp = nullptr;
	_wfopen_s(&fp, path.c_str(), L"rb");
	if (!fp) return 0;

	// Read header.
	NavMeshSetHeader header;
	size_t readLen = fread(&header, sizeof(NavMeshSetHeader), 1, fp);
	if (readLen != 1)
	{
		fclose(fp);
		return 0;
	}
	if (header.magic != NAVMESHSET_MAGIC)
	{
		fclose(fp);
		return 0;
	}
	if (header.version != NAVMESHSET_VERSION)
	{
		fclose(fp);
		return 0;
	}

	dtNavMesh* mesh = dtAllocNavMesh();
	if (!mesh)
	{
		fclose(fp);
		return 0;
	}
	dtStatus status = mesh->init(&header.params);
	if (dtStatusFailed(status))
	{
		fclose(fp);
		return 0;
	}

	// Read tiles.
	for (int i = 0; i < header.numTiles; ++i)
	{
		NavMeshTileHeader tileHeader;
		readLen = fread(&tileHeader, sizeof(tileHeader), 1, fp);
		if (readLen != 1)
		{
			fclose(fp);
			return 0;
		}

		if (!tileHeader.tileRef || !tileHeader.dataSize)
			break;

		unsigned char* data = (unsigned char*)dtAlloc(tileHeader.dataSize, DT_ALLOC_PERM);
		if (!data) break;
		memset(data, 0, tileHeader.dataSize);
		readLen = fread(data, tileHeader.dataSize, 1, fp);
		if (readLen != 1)
		{
			dtFree(data);
			fclose(fp);
			return 0;
		}

		mesh->addTile(data, tileHeader.dataSize, DT_TILE_FREE_DATA, tileHeader.tileRef, 0);
	}

	fclose(fp);

	return mesh;
}

void RecastNavi::SaveNavMesh(const char* path, const dtNavMesh* _NavMesh)
{
}

//void RecastNavi::LoadObj(const char* path)
//{
//	//m_geom->load(m_ctx, path);
//
//	//const rcMeshLoaderObj* loadedMesh = m_geom->getMesh();
//	//for(int i = 0 ; i < loadedMesh->getTriCount(); ++i)
//	//{
//	//	int tri = loadedMesh->getTris()[i];
//	//	for(int vertex = 0 ; vertex < 3 ; ++vertex)
//	//	{
//	//		
//	//		loadedMesh->getVerts()[i * vertex];
//	//	}
//	//}
//}
