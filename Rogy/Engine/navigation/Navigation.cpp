#include "Navigation.h"

void Navigation::RecastCleanup()
{
	if (m_triareas) delete[] m_triareas;
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

	if (m_ctx) delete m_ctx;
}
#include "..\mesh\mesh.h"
bool Navigation::NavMeshBuild()
{
	// convert our geometry into the recast format

	std::cout << "NavMeshBuild Start\n";



	int nLoop = 0;

	float*           rc_verts;
	unsigned int     rc_nverts;
	int*             rc_tris;
	float*           rc_trinorms;
	unsigned int     rc_ntris;
	float	           rc_bmin[3];
	float	           rc_bmax[3];

	glm::vec3 VertA;
	glm::vec3  VertB;
	glm::vec3  VertC;
	glm::vec3  TriNorm;
	int nVert = 0;

	float SPLAT_BANKDISTANCE = 1000;

	/*rc_bmin[0] = SPLAT_BANKDISTANCE;
	rc_bmin[1] = SPLAT_BANKDISTANCE;
	rc_bmin[2] = SPLAT_BANKDISTANCE;
	rc_bmax[0] = -SPLAT_BANKDISTANCE;
	rc_bmax[1] = -SPLAT_BANKDISTANCE;
	rc_bmax[2] = -SPLAT_BANKDISTANCE;*/

	rc_bmin[0] = minx.x;
	rc_bmin[1] = minx.y;
	rc_bmin[2] = minx.z;

	rc_bmax[0] = maxx.x;
	rc_bmax[1] = maxx.y;
	rc_bmax[2] = maxx.z;
	
	// setup recast verts, and min/max
	rc_nverts = m_mesh->vertices.size();
	rc_verts = new float[rc_nverts * 3];
	for (nLoop = 0; nLoop < rc_nverts; nLoop++)
	{
		rc_verts[nLoop * 3 + 0] = m_mesh->vertices[nLoop].Position[0];
		rc_verts[nLoop * 3 + 1] = m_mesh->vertices[nLoop].Position[1];
		rc_verts[nLoop * 3 + 2] = m_mesh->vertices[nLoop].Position[2];

		if (m_mesh->vertices[nLoop].Position[0] < rc_bmin[0]) rc_bmin[0] = m_mesh->vertices[nLoop].Position[0];
		if (m_mesh->vertices[nLoop].Position[1] < rc_bmin[1]) rc_bmin[1] = m_mesh->vertices[nLoop].Position[1];
		if (m_mesh->vertices[nLoop].Position[2] < rc_bmin[2]) rc_bmin[2] = m_mesh->vertices[nLoop].Position[2];

		if (m_mesh->vertices[nLoop].Position[0] > rc_bmax[0]) rc_bmax[0] = m_mesh->vertices[nLoop].Position[0];
		if (m_mesh->vertices[nLoop].Position[1] > rc_bmax[1]) rc_bmax[1] = m_mesh->vertices[nLoop].Position[1];
		if (m_mesh->vertices[nLoop].Position[2] > rc_bmax[2]) rc_bmax[2] = m_mesh->vertices[nLoop].Position[2];

	}

	// work out how many triangles we need.  We don't use every triangle, for instance we don't use def light boxes.
	// we only used opaque, gel or alphat triangles.
	int nMaxTri = m_mesh->faces.size();
	int nTextureCategory = 0;
	rc_ntris = 0;
	for (nLoop = 0; nLoop < nMaxTri; nLoop++)
	{
			rc_ntris++;
	}



	// setup recast triangles
	rc_tris = new int[rc_ntris * 3];
	rc_trinorms = new float[rc_ntris * 3];

	int nTriCount = 0;
	for (nLoop = 0; nLoop < nMaxTri; nLoop++)
	{
		// not all triangles should be added, for instance we don't want deferred light boxes.
		// So check the texture category.  We only accept opaque, gel or alphat textures.

		// vertex indices
		rc_tris[nTriCount * 3 + 0] = m_mesh->faces[nLoop].indices[0];
		rc_tris[nTriCount * 3 + 1] = m_mesh->faces[nLoop].indices[2];
		rc_tris[nTriCount * 3 + 2] = m_mesh->faces[nLoop].indices[1];


		// calculate normal for this triangle.  
		// Why doesn't it exist already?  Because we usually use normals on verts, not triangles.
		nVert = m_mesh->faces[nLoop].indices[0];
		VertA.x = m_mesh->vertices[nVert].Position[0];
		VertA.y = m_mesh->vertices[nVert].Position[1];
		VertA.z = m_mesh->vertices[nVert].Position[2];

		nVert = m_mesh->faces[nLoop].indices[1];
		VertB.x = m_mesh->vertices[nVert].Position[0];
		VertB.y = m_mesh->vertices[nVert].Position[1];
		VertB.z = m_mesh->vertices[nVert].Position[2];

		nVert = m_mesh->faces[nLoop].indices[2];
		VertC.x = m_mesh->vertices[nVert].Position[0];
		VertC.y = m_mesh->vertices[nVert].Position[1];
		VertC.z = m_mesh->vertices[nVert].Position[2];

		VertA -= VertC;
		VertB -= VertC;

		
		VertA = glm::cross(VertA, VertB);
		VertA = glm::normalize(VertA);

		// recast version of the normal
		rc_trinorms[nTriCount * 3 + 0] = VertA.x;
		rc_trinorms[nTriCount * 3 + 1] = VertA.y;
		rc_trinorms[nTriCount * 3 + 2] = VertA.z;

		nTriCount++;

	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



	//RecastCleanup() ;

	m_ctx = new rcContext(true);
	//
	// Step 1. Initialize build config.
	//

	// cellsize (1.5, 1.0) was the most accurate at finding all the places we could go, but was also slow to generate.
	// Might be suitable for pre-generated meshes. Though it also produces a lot more polygons.

	m_cellSize = 9.0;//0.3;
	m_cellHeight = 6.0;//0.2;
	m_agentMaxSlope = 45;
	m_agentHeight = 64.0;
	m_agentMaxClimb = 16;
	m_agentRadius = 16;
	m_edgeMaxLen = 512;
	m_edgeMaxError = 1.3;
	m_regionMinSize = 50;
	m_regionMergeSize = 20;
	m_vertsPerPoly = 6;
	m_detailSampleDist = 6;
	m_detailSampleMaxError = 1;
	m_keepInterResults = false;

	// Init build configuration from GUI
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


	// Reset build times gathering.
	m_ctx->resetTimers();

	// Start the build process.	
	m_ctx->startTimer(RC_TIMER_TOTAL);

	// Set the area where the navigation will be build.
	// Here the bounds of the input mesh are used, but the
	// area could be specified by an user defined box, etc.
	rcVcopy(m_cfg.bmin, rc_bmin);
	rcVcopy(m_cfg.bmax, rc_bmax);
	rcCalcGridSize(m_cfg.bmin, m_cfg.bmax, m_cfg.cs, &m_cfg.width, &m_cfg.height);


	//
	// Step 2. Rasterize input polygon soup.
	//

	// Allocate voxel heightfield where we rasterize our input data to.

	m_solid = rcAllocHeightfield();
	if (!m_solid)
	{
		m_ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'solid'.");
		return false;
	}
	if (!rcCreateHeightfield(m_ctx, *m_solid, m_cfg.width, m_cfg.height, m_cfg.bmin, m_cfg.bmax, m_cfg.cs, m_cfg.ch))
	{
		m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not create solid heightfield.");
		return false;
	}

	// Allocate array that can hold triangle area types.
	// If you have multiple meshes you need to process, allocate
	// and array which can hold the max number of triangles you need to process.
	m_triareas = new unsigned char[rc_ntris];
	if (!m_triareas)
	{
		m_ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'm_triareas' (%d).", rc_ntris);
		return false;
	}

	// Find triangles which are walkable based on their slope and rasterize them.
	// If your input data is multiple meshes, you can transform them here, calculate
	// the are type for each of the meshes and rasterize them.
	memset(m_triareas, 0, rc_ntris * sizeof(unsigned char));
	rcMarkWalkableTriangles(m_ctx, m_cfg.walkableSlopeAngle, rc_verts, rc_nverts, rc_tris, rc_ntris, m_triareas);
	rcRasterizeTriangles(m_ctx, rc_verts, rc_nverts, rc_tris, m_triareas, rc_ntris, *m_solid, m_cfg.walkableClimb);

	if (!m_keepInterResults)
	{
		delete[] m_triareas;
		m_triareas = 0;
	}

	//
	// Step 3. Filter walkables surfaces.
	//

	// Once all geoemtry is rasterized, we do initial pass of filtering to
	// remove unwanted overhangs caused by the conservative rasterization
	// as well as filter spans where the character cannot possibly stand.
	rcFilterLowHangingWalkableObstacles(m_ctx, m_cfg.walkableClimb, *m_solid);
	rcFilterLedgeSpans(m_ctx, m_cfg.walkableHeight, m_cfg.walkableClimb, *m_solid);
	rcFilterWalkableLowHeightSpans(m_ctx, m_cfg.walkableHeight, *m_solid);



	//
	// Step 4. Partition walkable surface to simple regions.
	//

	// Compact the heightfield so that it is faster to handle from now on.
	// This will result more cache coherent data as well as the neighbours
	// between walkable cells will be calculated.
	m_chf = rcAllocCompactHeightfield();
	if (!m_chf)
	{
		m_ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'chf'.");
		return false;
	}
	if (!rcBuildCompactHeightfield(m_ctx, m_cfg.walkableHeight, m_cfg.walkableClimb, *m_solid, *m_chf))
	{
		m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not build compact data.");
		return false;
	}

	if (!m_keepInterResults)
	{
		rcFreeHeightField(m_solid);
		m_solid = 0;
	}

	// Erode the walkable area by agent radius.
	if (!rcErodeWalkableArea(m_ctx, m_cfg.walkableRadius, *m_chf))
	{
		m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not erode.");
		return false;
	}

	// (Optional) Mark areas.
	//const ConvexVolume* vols = m_geom->getConvexVolumes();
	//for (int i  = 0; i < m_geom->getConvexVolumeCount(); ++i)
	//	rcMarkConvexPolyArea(m_ctx, vols[i].verts, vols[i].nverts, vols[i].hmin, vols[i].hmax, (unsigned char)vols[i].area, *m_chf);

	// Prepare for region partitioning, by calculating distance field along the walkable surface.
	if (!rcBuildDistanceField(m_ctx, *m_chf))
	{
		m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not build distance field.");
		return false;
	}

	// Partition the walkable surface into simple regions without holes.
	if (!rcBuildRegions(m_ctx, *m_chf, m_cfg.borderSize, m_cfg.minRegionArea, m_cfg.mergeRegionArea))
	{
		m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not build regions.");
		return false;
	}

	//
	// Step 5. Trace and simplify region contours.
	//

	// Create contours.
	m_cset = rcAllocContourSet();
	if (!m_cset)
	{
		m_ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'cset'.");
		return false;
	}
	if (!rcBuildContours(m_ctx, *m_chf, m_cfg.maxSimplificationError, m_cfg.maxEdgeLen, *m_cset))
	{
		m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not create contours.");
		return false;
	}

	//
	// Step 6. Build polygons mesh from contours.
	//

	// Build polygon navmesh from the contours.
	m_pmesh = rcAllocPolyMesh();
	if (!m_pmesh)
	{
		m_ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'pmesh'.");
		return false;
	}
	if (!rcBuildPolyMesh(m_ctx, *m_cset, m_cfg.maxVertsPerPoly, *m_pmesh))
	{
		m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not triangulate contours.");
		return false;
	}

	//
	// Step 7. Create detail mesh which allows to access approximate height on each polygon.
	//

	m_dmesh = rcAllocPolyMeshDetail();
	if (!m_dmesh)
	{
		m_ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'pmdtl'.");
		return false;
	}

	if (!rcBuildPolyMeshDetail(m_ctx, *m_pmesh, *m_chf, m_cfg.detailSampleDist, m_cfg.detailSampleMaxError, *m_dmesh))
	{
		m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not build detail mesh.");
		return false;
	}

	if (!m_keepInterResults)
	{
		rcFreeCompactHeightfield(m_chf);
		m_chf = 0;
		rcFreeContourSet(m_cset);
		m_cset = 0;
	}

	// At this point the navigation mesh data is ready, you can access it from m_pmesh.
	// See duDebugDrawPolyMesh or dtCreateNavMeshData as examples how to access the data.

	//
	// (Optional) Step 8. Create Detour data from Recast poly mesh.
	//

	// The GUI may allow more max points per polygon than Detour can handle.
	// Only build the detour navmesh if we do not exceed the limit.


	if (m_cfg.maxVertsPerPoly <= DT_VERTS_PER_POLYGON)
	{
		std::cout <<"Detour 1000\n";

		unsigned char* navData = 0;
		int navDataSize = 0;


		// Update poly flags from areas.
		for (int i = 0; i < m_pmesh->npolys; ++i)
		{
			if (m_pmesh->areas[i] == RC_WALKABLE_AREA)
			{
				m_pmesh->areas[i] = SAMPLE_POLYAREA_GROUND;
				m_pmesh->flags[i] = SAMPLE_POLYFLAGS_WALK;
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

		// no off mesh connections yet
		m_offMeshConCount = 0;
		params.offMeshConVerts = m_offMeshConVerts;
		params.offMeshConRad = m_offMeshConRads;
		params.offMeshConDir = m_offMeshConDirs;
		params.offMeshConAreas = m_offMeshConAreas;
		params.offMeshConFlags = m_offMeshConFlags;
		params.offMeshConUserID = m_offMeshConId;
		params.offMeshConCount = m_offMeshConCount;

		params.walkableHeight = m_agentHeight;
		params.walkableRadius = m_agentRadius;
		params.walkableClimb = m_agentMaxClimb;
		rcVcopy(params.bmin, m_pmesh->bmin);
		rcVcopy(params.bmax, m_pmesh->bmax);
		params.cs = m_cfg.cs;
		params.ch = m_cfg.ch;

		std::cout << "Detour 2000\n";

		if (!dtCreateNavMeshData(&params, &navData, &navDataSize))
		{
			m_ctx->log(RC_LOG_ERROR, "Could not build Detour navmesh.");
			return false;
		}

		std::cout << "Detour 3000\n";

		m_navMesh = dtAllocNavMesh();
		if (!m_navMesh)
		{
			dtFree(navData);
			m_ctx->log(RC_LOG_ERROR, "Could not create Detour navmesh");
			return false;
		}

		std::cout << "Detour 4000\n";

		dtStatus status;

		status = m_navMesh->init(navData, navDataSize, DT_TILE_FREE_DATA);
		if (dtStatusFailed(status))
		{
			dtFree(navData);
			m_ctx->log(RC_LOG_ERROR, "Could not init Detour navmesh");
			return false;
		}

		std::cout << "Detour 5000\n";

		m_navQuery = dtAllocNavMeshQuery();
		status = m_navQuery->init(m_navMesh, 2048);

		std::cout << "Detour 5500\n";

		if (dtStatusFailed(status))
		{
			m_ctx->log(RC_LOG_ERROR, "Could not init Detour navmesh query");
			return false;
		}

		std::cout << "Detour 6000\n";
	}

	m_ctx->stopTimer(RC_TIMER_TOTAL);


	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	// cleanup stuff we don't need
	delete[] rc_verts;
	delete[] rc_tris;
	delete[] rc_trinorms;

	CreateRecastPolyMesh(*m_pmesh);

	std::cout << "NavMeshBuild End\n";
	return true;
}

int Navigation::FindPath(float * pStartPos, float * pEndPos, int nPathSlot, int nTarget)
{
	dtStatus status;
	float pExtents[3] = { 32.0f, 32.0f, 32.0f }; // size of box around start/end points to look for nav polygons
	dtPolyRef StartPoly;
	float StartNearest[3];
	dtPolyRef EndPoly;
	float EndNearest[3];
	dtPolyRef PolyPath[MAX_PATHPOLY];
	int nPathCount = 0;
	float StraightPath[MAX_PATHVERT * 3];
	int nVertCount = 0;


	// setup the filter
	dtQueryFilter Filter;
	Filter.setIncludeFlags(0xFFFF);
	Filter.setExcludeFlags(0);
	Filter.setAreaCost(SAMPLE_POLYAREA_GROUND, 1.0f);

	// find the start polygon
	status = m_navQuery->findNearestPoly(pStartPos, pExtents, &Filter, &StartPoly, StartNearest);
	if ((status&DT_FAILURE) || (status&DT_STATUS_DETAIL_MASK)) return -1; // couldn't find a polygon

	// find the end polygon
	status = m_navQuery->findNearestPoly(pEndPos, pExtents, &Filter, &EndPoly, EndNearest);
	if ((status&DT_FAILURE) || (status&DT_STATUS_DETAIL_MASK)) return -2; // couldn't find a polygon

	status = m_navQuery->findPath(StartPoly, EndPoly, StartNearest, EndNearest, &Filter, PolyPath, &nPathCount, MAX_PATHPOLY);
	if ((status&DT_FAILURE) || (status&DT_STATUS_DETAIL_MASK)) return -3; // couldn't create a path
	if (nPathCount == 0) return -4; // couldn't find a path

	status = m_navQuery->findStraightPath(StartNearest, EndNearest, PolyPath, nPathCount, StraightPath, NULL, NULL, &nVertCount, MAX_PATHVERT);
	if ((status&DT_FAILURE) || (status&DT_STATUS_DETAIL_MASK)) return -5; // couldn't create a path
	if (nVertCount == 0) return -6; // couldn't find a path

	// At this point we have our path.  Copy it to the path store
	int nIndex = 0;
	for (int nVert = 0; nVert < nVertCount; nVert++)
	{
		//m_PathStore[nPathSlot].PosX[nVert] = StraightPath[nIndex++];
		//m_PathStore[nPathSlot].PosY[nVert] = StraightPath[nIndex++];
		//m_PathStore[nPathSlot].PosZ[nVert] = StraightPath[nIndex++];

		//sprintf(m_chBug, "Path Vert %i, %f %f %f", nVert, m_PathStore[nPathSlot].PosX[nVert], m_PathStore[nPathSlot].PosY[nVert], m_PathStore[nPathSlot].PosZ[nVert]) ;
		//m_pLog->logMessage(m_chBug);
	}
	//m_PathStore[nPathSlot].MaxVertex = nVertCount;
	//m_PathStore[nPathSlot].Target = nTarget;

	return nVertCount;
}

void Navigation::CreateRecastPolyMesh(const rcPolyMesh & mesh)
{
	

}

void Navigation::CreateRecastPathLine(int nPathSlot)
{
	/*if (m_pRecastMOPath)
	{
		m_pRecastSN->detachObject("RecastMOPath");
		m_pSceneMgr->destroyManualObject(m_pRecastMOPath);
		m_pRecastMOPath = NULL;
	}


	m_pRecastMOPath = m_pSceneMgr->createManualObject("RecastMOPath");
	m_pRecastMOPath->begin("recastline", RenderOperation::OT_LINE_STRIP);


	int nVertCount = m_PathStore[nPathSlot].MaxVertex;
	for (int nVert = 0; nVert < nVertCount; nVert++)
	{
		m_pRecastMOPath->position(m_PathStore[nPathSlot].PosX[nVert], m_PathStore[nPathSlot].PosY[nVert] + 8.0f, m_PathStore[nPathSlot].PosZ[nVert]);
		m_pRecastMOPath->colour(1, 0, 0);

		//sprintf(m_chBug, "Line Vert %i, %f %f %f", nVert, m_PathStore[nPathSlot].PosX[nVert], m_PathStore[nPathSlot].PosY[nVert], m_PathStore[nPathSlot].PosZ[nVert]) ;
		//m_pLog->logMessage(m_chBug);
	}




	m_pRecastMOPath->end();
	m_pRecastSN->attachObject(m_pRecastMOPath);*/
}

Navigation::Navigation()
{
	/*m_triareas = NULL;
	m_solid = NULL;
	m_chf = NULL;
	m_cset = NULL;
	m_pmesh = NULL;
	//m_cfg;	
	m_dmesh = NULL;
	m_geom = NULL;
	m_navMesh = NULL;
	m_navQuery = NULL;
	//m_navMeshDrawFlags;
	m_ctx = NULL;
	
	RecastCleanup(); *///?? don't know if I should do this prior to making any recast stuff, but the demo did.
	//m_pRecastMOPath = NULL;
}

Navigation::~Navigation()
{
}

/*bool Navigation::Load(Mesh * m_mesh)
{
	

	return true;
}
*/