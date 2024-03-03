#include "pch.h"
#include "RecastMesh.h"

RecastMesh::RecastMesh()
	: m_scale(1.0f)
	, m_verts(0)
	, m_tris(0)
	, m_normals(0)
	, m_vertCount(0)
	, m_triCount(0)
{
}

RecastMesh::~RecastMesh()
{
	delete[] m_verts;
	delete[] m_normals;
	delete[] m_tris;
}


void RecastMesh::parseMesh(Ptr<CMesh> pMesh)
{
	UINT vtxCount = pMesh->GetVtxCount();
	m_verts = new float[vtxCount * 3];
	m_normals = new float[vtxCount];
	//m_tris = new int[vtxCount];
}

void RecastMesh::addVertex(float x, float y, float z, int& cap)
{
	if (m_vertCount + 1 > cap)
	{
		cap = !cap ? 8 : cap * 2;
		float* nv = new float[cap * 3];
		if (m_vertCount)
			memcpy(nv, m_verts, m_vertCount * 3 * sizeof(float));
		delete[] m_verts;
		m_verts = nv;
	}
	float* dst = &m_verts[m_vertCount * 3];
	*dst++ = x * m_scale;
	*dst++ = y * m_scale;
	*dst++ = z * m_scale;
	m_vertCount++;
}

void RecastMesh::addTriangle(int a, int b, int c, int& cap)
{
	if (m_triCount + 1 > cap)
	{
		cap = !cap ? 8 : cap * 2;
		int* nv = new int[cap * 3];
		if (m_triCount)
			memcpy(nv, m_tris, m_triCount * 3 * sizeof(int));
		delete[] m_tris;
		m_tris = nv;
	}
	int* dst = &m_tris[m_triCount * 3];
	*dst++ = a;
	*dst++ = b;
	*dst++ = c;
	m_triCount++;
}

static int parseFace(char* row, int* data, int n, int vcnt)
{
	int j = 0;
	while (*row != '\0')
	{
		// Skip initial white space
		while (*row != '\0' && (*row == ' ' || *row == '\t'))
			row++;
		char* s = row;
		// Find vertex delimiter and terminated the string there for conversion.
		while (*row != '\0' && *row != ' ' && *row != '\t')
		{
			if (*row == '/') *row = '\0';
			row++;
		}
		if (*s == '\0')
			continue;
		int vi = atoi(s);
		data[j++] = vi < 0 ? vi + vcnt : vi - 1;
		if (j >= n) return j;
	}
	return j;
}
