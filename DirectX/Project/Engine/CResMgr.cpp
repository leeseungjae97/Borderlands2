#include "pch.h"
#include "CResMgr.h"

#include "CAnimation3D_CShader.h"
#include "CPathMgr.h"

CResMgr::CResMgr()
	: m_Changed(false)
{
}

CResMgr::~CResMgr()
{
}

void CResMgr::init()
{
	InitSound();

	CreateDefaultMesh();
	CreateDefaultGraphicsShader();
	CreateDefaultComputeShader();
	CreateDefaultMaterial();	
}



void CResMgr::InitSound()
{
	FMOD::System_Create(&CSound::g_pFMOD);

	if (nullptr == CSound::g_pFMOD)
	{
		assert(nullptr);
	}

	// 32개 채널 생성
	CSound::g_pFMOD->init(32, FMOD_DEFAULT, nullptr);
}


void CResMgr::tick()
{
	m_Changed = false;
}

void CResMgr::CreateDefaultMesh()
{	
	vector<Vtx> vecVtx;
	vector<UINT> vecIdx;
	Vtx v;


	Ptr<CMesh> pMesh = nullptr;

	// ==============
	// PointMesh 생성
	// ==============
	v.vPos = Vec3(0.f, 0.f, 0.f);
	v.vUV = Vec2(0.f, 0.f);
	v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);

	UINT idx = 0;

	pMesh = new CMesh(true);
	pMesh->Create(&v, 1, &idx, 1);
	AddRes(L"PointMesh", pMesh);


	// =============
	// RectMesh 생성
	// =============
	// 0 --- 1 
	// |  \  |
	// 3 --- 2
	v.vNormal = Vec3(0.f, 0.f, -1.f);
	v.vTangent = Vec3(1.f, 0.f, 0.f);
	v.vBinormal = Vec3(0.f, -1.f, 0.f);

	v.vPos = Vec3(-0.5f, 0.5f, 0.f);
	v.vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	v.vUV = Vec2(0.f, 0.f);
	vecVtx.push_back(v);

	v.vPos = Vec3(0.5f, 0.5f, 0.f);
	v.vColor = Vec4(0.f, 1.f, 0.f, 1.f);
	v.vUV = Vec2(1.f, 0.f);
	vecVtx.push_back(v);

	v.vPos = Vec3(0.5f, -0.5f, 0.f);
	v.vColor = Vec4(0.f, 0.f, 1.f, 1.f);
	v.vUV = Vec2(1.f, 1.f);
	vecVtx.push_back(v);

	v.vPos = Vec3(-0.5f, -0.5f, 0.f);
	v.vColor = Vec4(0.f, 0.f, 0.f, 1.f);
	v.vUV = Vec2(0.f, 1.f);
	vecVtx.push_back(v);

	vecIdx.push_back(0);
	vecIdx.push_back(2);
	vecIdx.push_back(3);

	vecIdx.push_back(0);
	vecIdx.push_back(1);
	vecIdx.push_back(2);

	pMesh = new CMesh(true);
	pMesh->Create(vecVtx.data(), (UINT)vecVtx.size(), vecIdx.data(), (UINT)vecIdx.size());
	AddRes(L"RectMesh", pMesh);
	
	vecIdx.clear();
	vecIdx.push_back(0);
	vecIdx.push_back(1);
	vecIdx.push_back(2);
	vecIdx.push_back(3);
	vecIdx.push_back(0);

	pMesh = new CMesh(true);
	pMesh->Create(vecVtx.data(), (UINT)vecVtx.size(), vecIdx.data(), (UINT)vecIdx.size());
	AddRes(L"RectMesh_Debug", pMesh);

	vecVtx.clear();
	vecIdx.clear();


	// ===========
	// Circle Mesh
	// ===========
	//      3 
	//   4     2
	//  5 --0-- 1	
	// 반지름
	float fRadius = 0.5f;

	// 각도
	UINT Slice = 40;
	float fTheta = XM_2PI / (float)Slice;

	v.vNormal = Vec3(0.f, 0.f, -1.f);
	v.vTangent = Vec3(1.f, 0.f, 0.f);
	v.vBinormal = Vec3(0.f, -1.f, 0.f);

	// 중심점
	v.vPos = Vec3(0.f, 0.f, 0.f);
	v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
	v.vUV = Vec2(0.5f, 0.5f);
	vecVtx.push_back(v);

	// 정점 위치 지정
	for (UINT i = 0; i < Slice; ++i)
	{
		v.vPos = Vec3(fRadius * cosf(fTheta * (float)i), fRadius * sinf(fTheta * (float)i), 0.f);
		v.vUV = Vec2(v.vPos.x + 0.5f, -v.vPos.y + 0.5f);
		vecVtx.push_back(v);
	}

	// 인덱스 설정
	for (UINT i = 0; i < Slice - 1; ++i)
	{
		vecIdx.push_back(0);
		vecIdx.push_back(i + 2);
		vecIdx.push_back(i + 1);
	}

	// 마지막 삼각형
	vecIdx.push_back(0);
	vecIdx.push_back(1);
	vecIdx.push_back(Slice);

	v.vNormal = Vec3(0.f, 0.f, 1.f);
	v.vTangent = Vec3(1.f, 0.f, 0.f);
	v.vBinormal = Vec3(0.f, -1.f, 0.f);

	v.vPos = Vec3(0.f, 0.f, 0.f);
	v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
	v.vUV = Vec2(0.5f, 0.5f);
	vecVtx.push_back(v);

	// 정점 위치 지정
	for (UINT i = 0; i < Slice; ++i)
	{
		v.vPos = Vec3(fRadius * cosf(fTheta * (float)i), fRadius * sinf(fTheta * (float)i), 0.f);
		v.vUV = Vec2(v.vPos.x + 0.5f, -v.vPos.y + 0.5f);
		vecVtx.push_back(v);
	}

	// 인덱스 설정
	for (UINT i = Slice + 1; i < Slice * 2; ++i)
	{
		vecIdx.push_back(Slice);
		vecIdx.push_back(i + 1);
		vecIdx.push_back(i + 2);
	}

	// 마지막 삼각형
	vecIdx.push_back(Slice);
	vecIdx.push_back(Slice + 1);
	vecIdx.push_back((Slice * 2) + 1);

	pMesh = new CMesh(true);
	pMesh->Create(vecVtx.data(), (UINT)vecVtx.size(), vecIdx.data(), (UINT)vecIdx.size());
	AddRes(L"CircleMesh", pMesh);
	
	vecIdx.clear();
	for (UINT i = 0; i < Slice; ++i)
	{
		vecIdx.push_back(i + 1);
	}
	vecIdx.push_back(1);

	pMesh = new CMesh(true);
	pMesh->Create(vecVtx.data(), (UINT)vecVtx.size(), vecIdx.data(), (UINT)vecIdx.size());
	AddRes(L"CircleMesh_Debug", pMesh);

	vecVtx.clear();
	vecIdx.clear();

	// =========
   // Cube Mesh
   // =========
	Vtx arrCube[24] = {};

	// 윗면
	arrCube[0].vPos = Vec3(-0.5f, 0.5f, 0.5f);
	arrCube[0].vColor = Vec4(1.f, 1.f, 1.f, 1.f);
	arrCube[0].vUV = Vec2(0.f, 0.f);
	arrCube[0].vNormal = Vec3(0.f, 1.f, 0.f);

	arrCube[1].vPos = Vec3(0.5f, 0.5f, 0.5f);
	arrCube[1].vColor = Vec4(1.f, 1.f, 1.f, 1.f);
	arrCube[1].vUV = Vec2(0.f, 0.f);
	arrCube[1].vNormal = Vec3(0.f, 1.f, 0.f);

	arrCube[2].vPos = Vec3(0.5f, 0.5f, -0.5f);
	arrCube[2].vColor = Vec4(1.f, 1.f, 1.f, 1.f);
	arrCube[2].vUV = Vec2(0.f, 0.f);
	arrCube[2].vNormal = Vec3(0.f, 1.f, 0.f);

	arrCube[3].vPos = Vec3(-0.5f, 0.5f, -0.5f);
	arrCube[3].vColor = Vec4(1.f, 1.f, 1.f, 1.f);
	arrCube[3].vUV = Vec2(0.f, 0.f);
	arrCube[3].vNormal = Vec3(0.f, 1.f, 0.f);


	// 아랫 면   
	arrCube[4].vPos = Vec3(-0.5f, -0.5f, -0.5f);
	arrCube[4].vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	arrCube[4].vUV = Vec2(0.f, 0.f);
	arrCube[4].vNormal = Vec3(0.f, -1.f, 0.f);

	arrCube[5].vPos = Vec3(0.5f, -0.5f, -0.5f);
	arrCube[5].vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	arrCube[5].vUV = Vec2(0.f, 0.f);
	arrCube[5].vNormal = Vec3(0.f, -1.f, 0.f);

	arrCube[6].vPos = Vec3(0.5f, -0.5f, 0.5f);
	arrCube[6].vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	arrCube[6].vUV = Vec2(0.f, 0.f);
	arrCube[6].vNormal = Vec3(0.f, -1.f, 0.f);

	arrCube[7].vPos = Vec3(-0.5f, -0.5f, 0.5f);
	arrCube[7].vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	arrCube[7].vUV = Vec2(0.f, 0.f);
	arrCube[7].vNormal = Vec3(0.f, -1.f, 0.f);

	// 왼쪽 면
	arrCube[8].vPos = Vec3(-0.5f, 0.5f, 0.5f);
	arrCube[8].vColor = Vec4(0.f, 1.f, 0.f, 1.f);
	arrCube[8].vUV = Vec2(0.f, 0.f);
	arrCube[8].vNormal = Vec3(-1.f, 0.f, 0.f);

	arrCube[9].vPos = Vec3(-0.5f, 0.5f, -0.5f);
	arrCube[9].vColor = Vec4(0.f, 1.f, 0.f, 1.f);
	arrCube[9].vUV = Vec2(0.f, 0.f);
	arrCube[9].vNormal = Vec3(-1.f, 0.f, 0.f);

	arrCube[10].vPos = Vec3(-0.5f, -0.5f, -0.5f);
	arrCube[10].vColor = Vec4(0.f, 1.f, 0.f, 1.f);
	arrCube[10].vUV = Vec2(0.f, 0.f);
	arrCube[10].vNormal = Vec3(-1.f, 0.f, 0.f);

	arrCube[11].vPos = Vec3(-0.5f, -0.5f, 0.5f);
	arrCube[11].vColor = Vec4(0.f, 1.f, 0.f, 1.f);
	arrCube[11].vUV = Vec2(0.f, 0.f);
	arrCube[11].vNormal = Vec3(-1.f, 0.f, 0.f);

	// 오른쪽 면
	arrCube[12].vPos = Vec3(0.5f, 0.5f, -0.5f);
	arrCube[12].vColor = Vec4(0.f, 0.f, 1.f, 1.f);
	arrCube[12].vUV = Vec2(0.f, 0.f);
	arrCube[12].vNormal = Vec3(1.f, 0.f, 0.f);

	arrCube[13].vPos = Vec3(0.5f, 0.5f, 0.5f);
	arrCube[13].vColor = Vec4(0.f, 0.f, 1.f, 1.f);
	arrCube[13].vUV = Vec2(0.f, 0.f);
	arrCube[13].vNormal = Vec3(1.f, 0.f, 0.f);

	arrCube[14].vPos = Vec3(0.5f, -0.5f, 0.5f);
	arrCube[14].vColor = Vec4(0.f, 0.f, 1.f, 1.f);
	arrCube[14].vUV = Vec2(0.f, 0.f);
	arrCube[14].vNormal = Vec3(1.f, 0.f, 0.f);

	arrCube[15].vPos = Vec3(0.5f, -0.5f, -0.5f);
	arrCube[15].vColor = Vec4(0.f, 0.f, 1.f, 1.f);
	arrCube[15].vUV = Vec2(0.f, 0.f);
	arrCube[15].vNormal = Vec3(1.f, 0.f, 0.f);

	// 뒷 면
	arrCube[16].vPos = Vec3(0.5f, 0.5f, 0.5f);
	arrCube[16].vColor = Vec4(1.f, 1.f, 0.f, 1.f);
	arrCube[16].vUV = Vec2(0.f, 0.f);
	arrCube[16].vNormal = Vec3(0.f, 0.f, 1.f);

	arrCube[17].vPos = Vec3(-0.5f, 0.5f, 0.5f);
	arrCube[17].vColor = Vec4(1.f, 1.f, 0.f, 1.f);
	arrCube[17].vUV = Vec2(0.f, 0.f);
	arrCube[17].vNormal = Vec3(0.f, 0.f, 1.f);

	arrCube[18].vPos = Vec3(-0.5f, -0.5f, 0.5f);
	arrCube[18].vColor = Vec4(1.f, 1.f, 0.f, 1.f);
	arrCube[18].vUV = Vec2(0.f, 0.f);
	arrCube[18].vNormal = Vec3(0.f, 0.f, 1.f);

	arrCube[19].vPos = Vec3(0.5f, -0.5f, 0.5f);
	arrCube[19].vColor = Vec4(1.f, 1.f, 0.f, 1.f);
	arrCube[19].vUV = Vec2(0.f, 0.f);
	arrCube[19].vNormal = Vec3(0.f, 0.f, 1.f);

	// 앞 면
	arrCube[20].vPos = Vec3(-0.5f, 0.5f, -0.5f);;
	arrCube[20].vColor = Vec4(1.f, 0.f, 1.f, 1.f);
	arrCube[20].vUV = Vec2(0.f, 0.f);
	arrCube[20].vNormal = Vec3(0.f, 0.f, -1.f);

	arrCube[21].vPos = Vec3(0.5f, 0.5f, -0.5f);
	arrCube[21].vColor = Vec4(1.f, 0.f, 1.f, 1.f);
	arrCube[21].vUV = Vec2(0.f, 0.f);
	arrCube[21].vNormal = Vec3(0.f, 0.f, -1.f);

	arrCube[22].vPos = Vec3(0.5f, -0.5f, -0.5f);
	arrCube[22].vColor = Vec4(1.f, 0.f, 1.f, 1.f);
	arrCube[22].vUV = Vec2(0.f, 0.f);
	arrCube[22].vNormal = Vec3(0.f, 0.f, -1.f);

	arrCube[23].vPos = Vec3(-0.5f, -0.5f, -0.5f);
	arrCube[23].vColor = Vec4(1.f, 0.f, 1.f, 1.f);
	arrCube[23].vUV = Vec2(0.f, 0.f);
	arrCube[23].vNormal = Vec3(0.f, 0.f, -1.f);

	// 인덱스
	for (int i = 0; i < 12; i += 2)
	{
		vecIdx.push_back(i * 2);
		vecIdx.push_back(i * 2 + 1);
		vecIdx.push_back(i * 2 + 2);

		vecIdx.push_back(i * 2);
		vecIdx.push_back(i * 2 + 2);
		vecIdx.push_back(i * 2 + 3);
	}

	pMesh = new CMesh;
	pMesh->Create(arrCube, 24, vecIdx.data(), (UINT)vecIdx.size());
	AddRes(L"CubeMesh", pMesh);
	vecIdx.clear();

	// ===============
	// Cube Mesh Debug
	// ===============

	pMesh = new CMesh(true);
	vecIdx.push_back(0); vecIdx.push_back(1); vecIdx.push_back(2); vecIdx.push_back(3); vecIdx.push_back(0);
	vecIdx.push_back(7); vecIdx.push_back(6); vecIdx.push_back(1); vecIdx.push_back(2); vecIdx.push_back(5);
	vecIdx.push_back(6); vecIdx.push_back(7); vecIdx.push_back(4); vecIdx.push_back(3); vecIdx.push_back(2);
	vecIdx.push_back(5); vecIdx.push_back(4);

	pMesh->Create(arrCube, 24, vecIdx.data(), (UINT)vecIdx.size());
	AddRes<CMesh>(L"CubeMesh_Debug", pMesh);
	vecIdx.clear();

	// ===========
    // Sphere Mesh
    // ===========
	fRadius = 0.5f;

	// Top
	v.vPos = Vec3(0.f, fRadius, 0.f);
	v.vUV = Vec2(0.5f, 0.f);
	v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);

	v.vNormal = v.vPos;
	v.vNormal.Normalize();
	v.vTangent = Vec3(1.f, 0.f, 0.f);
	v.vBinormal = Vec3(0.f, 0.f, -1.f);

	vecVtx.push_back(v);

	// Body
	UINT iStackCount = 40; // 가로 분할 개수
	UINT iSliceCount = 40; // 세로 분할 개수

	// 스택카운터 만큼 PI를 나눔
	float fStackAngle = XM_PI / iStackCount;
	// 스택카운터 만큼 2PI를 나눔
	float fSliceAngle = XM_2PI / iSliceCount;

	float fUVXStep = 1.f / (float)iSliceCount;
	float fUVYStep = 1.f / (float)iStackCount;

	for (UINT i = 1; i < iStackCount; ++i)
	{
		float phi = i * fStackAngle;
		for (UINT j = 0; j <= iSliceCount; ++j)
		{
			float theta = j * fSliceAngle;

			// 가로 분할 후
			// 세로 분할로 그려지는 원을 그려줌.

			v.vPos = Vec3(fRadius * sinf(i * fStackAngle) * cosf(j * fSliceAngle)
				, fRadius * cosf(i * fStackAngle)
				, fRadius * sinf(i * fStackAngle) * sinf(j * fSliceAngle));
			v.vUV = Vec2(fUVXStep * j, fUVYStep * i);
			v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);

			// 법선벡터는 중심에서 정점으로 부터의 방향과 같다
			v.vNormal = v.vPos;
			v.vNormal.Normalize();

			v.vTangent.x = -fRadius * sinf(phi) * sinf(theta);
			v.vTangent.y = 0.f;
			v.vTangent.z = fRadius * sinf(phi) * cosf(theta);
			v.vTangent.Normalize();

			// Binormal은 두 벡터로 부터 직교해야 하기때문에 벡터외적으로 구함.
			v.vNormal.Cross(v.vTangent, v.vBinormal);
			v.vBinormal.Normalize();

			vecVtx.push_back(v);
		}
	}

	// Bottom
	v.vPos = Vec3(0.f, -fRadius, 0.f);
	v.vUV = Vec2(0.5f, 1.f);
	v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
	v.vNormal = v.vPos;
	v.vNormal.Normalize();

	v.vTangent = Vec3(1.f, 0.f, 0.f);
	v.vBinormal = Vec3(0.f, 0.f, -1.f);
	vecVtx.push_back(v);

	// 인덱스
	// 북극점
	for (UINT i = 0; i < iSliceCount; ++i)
	{
		vecIdx.push_back(0);
		vecIdx.push_back(i + 2);
		vecIdx.push_back(i + 1);
	}

	// 몸통
	for (UINT i = 0; i < iStackCount - 2; ++i)
	{
		for (UINT j = 0; j < iSliceCount; ++j)
		{
			// + 
			// | \
	        // +--+
			vecIdx.push_back((iSliceCount + 1) * (i)+(j)+1);
			vecIdx.push_back((iSliceCount + 1) * (i + 1) + (j + 1) + 1);
			vecIdx.push_back((iSliceCount + 1) * (i + 1) + (j)+1);

			// +--+
			//  \ |
			//    +
			vecIdx.push_back((iSliceCount + 1) * (i)+(j)+1);
			vecIdx.push_back((iSliceCount + 1) * (i)+(j + 1) + 1);
			vecIdx.push_back((iSliceCount + 1) * (i + 1) + (j + 1) + 1);
		}
	}

	// 남극점
	UINT iBottomIdx = (UINT)vecVtx.size() - 1;
	for (UINT i = 0; i < iSliceCount; ++i)
	{
		vecIdx.push_back(iBottomIdx);
		vecIdx.push_back(iBottomIdx - (i + 2));
		vecIdx.push_back(iBottomIdx - (i + 1));
	}

	pMesh = new CMesh(true);
	pMesh->Create(vecVtx.data(), (UINT)vecVtx.size(), vecIdx.data(), (UINT)vecIdx.size());
	AddRes<CMesh>(L"SphereMesh", pMesh);
	vecVtx.clear();
	vecIdx.clear();


	// ===========
    // Cone Mesh
    // ===========
	// Top
	v.vPos = Vec3(0.f, fRadius, 0.f);
	v.vUV = Vec2(0.5f, 0.f);
	v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);

	v.vNormal = v.vPos;
	v.vNormal.Normalize();
	v.vTangent = Vec3(1.f, 0.f, 0.f);
	v.vNormal.Cross(v.vTangent, v.vBinormal);
	v.vBinormal.Normalize();

	vecVtx.push_back(v);

	fRadius = 0.5f;

	// 각도
	Slice = 40;
	fTheta = XM_2PI / (float)Slice;

	// 정점 위치 지정
	for (UINT i = 0; i < Slice; ++i)
	{
		v.vPos = Vec3(fRadius * cosf(fTheta * (float)i), 0.f, fRadius * sinf(fTheta * (float)i));
		v.vUV = Vec2(v.vPos.x + 0.5f, v.vPos.y + 0.5f);

		vecVtx.push_back(v);
	}

	// 중심점
	v.vPos = Vec3(0.f, 0.f, 0.f);
	v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
	v.vUV = Vec2(0.5f, 0.5f);
	vecVtx.push_back(v);

	v.vNormal = -v.vPos;
	v.vNormal.Normalize();
	v.vTangent = Vec3(1.f, 0.f, 0.f);
	v.vNormal.Cross(v.vTangent, v.vBinormal);
	v.vBinormal.Normalize();

	for (UINT i = 0; i < Slice; ++i)
	{
		v.vPos = Vec3(fRadius * cosf(fTheta * (float)i), 0.f, fRadius * sinf(fTheta * (float)i));
		v.vUV = Vec2(v.vPos.x + 0.5f, v.vPos.y + 0.5f);

		vecVtx.push_back(v);
	}


	// 인덱스 설정
	for (UINT i = 0; i < Slice - 1; ++i)
	{
		vecIdx.push_back(0);
		vecIdx.push_back(i + 2);
		vecIdx.push_back(i + 1);
	}

	// 마지막 삼각형
	vecIdx.push_back(0);
	vecIdx.push_back(1);
	vecIdx.push_back(Slice);

	for (UINT i = Slice + 1; i < (Slice * 2); ++i)
	{
		vecIdx.push_back(Slice + 1);
		vecIdx.push_back(i + 1);
		vecIdx.push_back(i + 2);
	}

	vecIdx.push_back(Slice + 2);
	vecIdx.push_back(Slice + 1);
	vecIdx.push_back((Slice * 2) + 1);

	pMesh = new CMesh(true);
	pMesh->Create(vecVtx.data(), (UINT)vecVtx.size(), vecIdx.data(), (UINT)vecIdx.size());
	AddRes<CMesh>(L"ConeMesh", pMesh);
	vecVtx.clear();
	vecIdx.clear();
}

void CResMgr::CreateDefaultGraphicsShader()
{
	Ptr<CGraphicsShader> pShader = nullptr;

	// ============================
	// Std2DShader
	// RasterizerState      : None
	// BlendState           : Mask
	// DepthStencilState    : Less
	//
	// Parameter
	// g_tex_0              : Output Texture
	// ============================
	pShader = new CGraphicsShader;
	pShader->SetKey(L"Std2DShader");
	pShader->CreateVertexShader(L"shader\\std2d.fx", "VS_Std2D");
	pShader->CreatePixelShader(L"shader\\std2d.fx", "PS_Std2D");

	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetDSType(DS_TYPE::LESS);
	pShader->SetBSType(BS_TYPE::MASK);

	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_MASK);

	// Param
	pShader->AddTexParam(TEX_0, "Output Texture");

	AddRes(pShader->GetKey(), pShader);



	// ======================================
	// Std2DLightShader
	// RasterizerState      : None
	// BlendState           : Mask
	// DepthStencilState    : Less
	//
	// Parameter
	// g_tex_0              : Output Texture
	// g_tex_1              : Nomal Texture
	// ======================================
	pShader = new CGraphicsShader;
	pShader->SetKey(L"Std2DLightShader");
	pShader->CreateVertexShader(L"shader\\std2d.fx", "VS_Std2DLight");
	pShader->CreatePixelShader(L"shader\\std2d.fx", "PS_Std2DLight");

	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetDSType(DS_TYPE::LESS);
	pShader->SetBSType(BS_TYPE::MASK);

	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_MASK);

	// Param
	pShader->AddScalarParam(INT_1, "Test Int");
	pShader->AddScalarParam(FLOAT_0, "Test Float");
	pShader->AddScalarParam(VEC2_0, "Test Vec2");
	pShader->AddScalarParam(VEC4_0, "Test Vec4");

	pShader->AddTexParam(TEX_0, "Output Texture 1");
	pShader->AddTexParam(TEX_1, "Output Texture 2");
	pShader->AddTexParam(TEX_2, "Output Texture 3");

	AddRes(pShader->GetKey(), pShader);

	// =================
	// DebugShape Shader
	// Topology : LineStrip
	// RS_TYPE  : CULL_NONE
	// DS_TYPE  : NO_TEST_NO_WRITE
	// BS_TYPE  : Default
	// g_vec4_0 : OutColor
	// ==================
	pShader = new CGraphicsShader;
	pShader->SetKey(L"DebugShapeShader");
	pShader->CreateVertexShader(L"shader\\debugshape.fx", "VS_DebugShape");
	pShader->CreatePixelShader(L"shader\\debugshape.fx", "PS_DebugShape");

	pShader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetBSType(BS_TYPE::DEFAULT);

	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_MASK);

	AddRes(pShader->GetKey(), pShader);

	// =================
	// DebugSphereShape Shader
	// Topology : LineStrip
	// RS_TYPE  : CULL_NONE
	// DS_TYPE  : NO_TEST_NO_WRITE
	// BS_TYPE  : Default
	// g_vec4_0 : OutColor
	// ==================
	pShader = new CGraphicsShader;
	pShader->SetKey(L"DebugSphereShapeShader");
	pShader->CreateVertexShader(L"shader\\debugshape.fx", "VS_DebugShape");
	pShader->CreatePixelShader(L"shader\\debugshape.fx", "PS_DebugSphereShape");

	pShader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pShader->SetRSType(RS_TYPE::CULL_BACK);
	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetBSType(BS_TYPE::ALPHA_BLEND);

	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_MASK);

	AddRes(pShader->GetKey(), pShader);

	// ============================
	// TileMap Shader
	// 
	// RS_TYPE : CULL_NONE
	// DS_TYPE : LESS
	// BS_TYPE : MASK

	// Parameter
	// g_tex_0 : Tile Atlas Texture
	// ============================
	pShader = new CGraphicsShader;
	pShader->SetKey(L"TileMapShader");
	pShader->CreateVertexShader(L"shader\\tilemap.fx", "VS_TileMap");
	pShader->CreatePixelShader(L"shader\\tilemap.fx", "PS_TileMap");
	
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetDSType(DS_TYPE::LESS);
	pShader->SetBSType(BS_TYPE::MASK);

	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_MASK);

	AddRes(pShader->GetKey(), pShader);


	// ============================
	// ParticleRender
	// 
	// RS_TYPE : CULL_NONE
	// DS_TYPE : NO_WRITE
	// BS_TYPE : ALPHA_BLEND

	// Parameter
	// g_int_0 : Particle Index
	// 
	// Domain : TRANSPARENT
	// ============================
	pShader = new CGraphicsShader;
	pShader->SetKey(L"ParticleRenderShader");
	pShader->CreateVertexShader(L"shader\\particle_render.fx", "VS_ParticleRender");
	pShader->CreateGeometryShader(L"shader\\particle_render.fx", "GS_ParticleRender");
	pShader->CreatePixelShader(L"shader\\particle_render.fx", "PS_ParticleRender");

	pShader->SetRSType(RS_TYPE::CULL_NONE);
	//pShader->SetRSType(RS_TYPE::WIRE_FRAME);
	pShader->SetDSType(DS_TYPE::NO_WRITE);
	pShader->SetBSType(BS_TYPE::ALPHA_BLEND);
	pShader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_TRANSPARENT);

	AddRes(pShader->GetKey(), pShader);
	// ============================
    // ParticleRender2
    // 
    // RS_TYPE : CULL_NONE
    // DS_TYPE : NO_WRITE
    // BS_TYPE : ALPHA_BLEND

    // Parameter
    // g_int_0 : Particle Index
    // 
    // Domain : TRANSPARENT
    // ============================
	pShader = new CGraphicsShader;
	pShader->SetKey(L"ParticleRenderShader2");
	pShader->CreateVertexShader(L"shader\\particle_render.fx", "VS_ParticleRender");
	pShader->CreateGeometryShader(L"shader\\particle_render.fx", "GS_ParticleRender2");
	pShader->CreatePixelShader(L"shader\\particle_render.fx", "PS_ParticleRender");

	pShader->SetRSType(RS_TYPE::CULL_NONE);
	//pShader->SetRSType(RS_TYPE::WIRE_FRAME);
	pShader->SetDSType(DS_TYPE::NO_WRITE);
	pShader->SetBSType(BS_TYPE::ALPHA_BLEND);
	pShader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_TRANSPARENT);

	AddRes(pShader->GetKey(), pShader);


	// ============================
	// GrayShader
	// RS_TYPE : CULL_NONE
	// DS_TYPE : NO_TEST_NO_WRITE
	// BS_TYPE : DEFAULT	 
	// Domain : POSTPROCESS
	// ============================
	pShader = new CGraphicsShader;
	pShader->SetKey(L"GrayShader");
	pShader->CreateVertexShader(L"shader\\postprocess.fx", "VS_GrayShader");	
	pShader->CreatePixelShader(L"shader\\postprocess.fx", "PS_GrayShader");
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);	
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_POSTPROCESS);
	AddRes(pShader->GetKey(), pShader);

	// ============================
	// Distortion Shader
	// RS_TYPE : CULL_NONE
	// DS_TYPE : NO_TEST_NO_WRITE
	// BS_TYPE : DEFAULT	 
	// Domain : POSTPROCESS
	// ============================
	pShader = new CGraphicsShader;
	pShader->SetKey(L"DistortionShader");
	pShader->CreateVertexShader(L"shader\\postprocess.fx", "VS_Distortion");
	pShader->CreatePixelShader(L"shader\\postprocess.fx", "PS_Distortion");
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_POSTPROCESS);

	// Parameter
	pShader->AddTexParam(TEX_1, "Noise Texture");

	AddRes(pShader->GetKey(), pShader);


	// ============================
	// TestShader
	// RS_TYPE : CULL_NONE
	// DS_TYPE : LESS
	// BS_TYPE : DEFAULT	 
	// Domain : MASK
	// ============================
	pShader = new CGraphicsShader;
	pShader->SetKey(L"TestShader");
	pShader->CreateVertexShader(L"shader\\test.fx", "VS_TestShader");
	pShader->CreatePixelShader(L"shader\\test.fx", "PS_TestShader");
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_MASK);

	// Parameter
	pShader->AddScalarParam(INT_0, "Color Type");
	pShader->AddTexParam(TEX_0, "Output Texture");

	AddRes(pShader->GetKey(), pShader);

	// ============================
    // Std3DShader
    // RS_TYPE : CULL_BACK
    // DS_TYPE : LESS
    // BS_TYPE : DEFAULT	 
    // Domain : MASK
    // ============================
	pShader = new CGraphicsShader;
	pShader->SetKey(L"Std3DShader");
	pShader->CreateVertexShader(L"shader\\std3d.fx", "VS_Std3D");
	pShader->CreatePixelShader(L"shader\\std3d.fx", "PS_Std3D");
	//pShader->SetRSType(RS_TYPE::WIRE_FRAME);
	pShader->SetRSType(RS_TYPE::CULL_BACK);
	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_MASK);

	pShader->AddTexParam(TEX_0, "Output Texture");
	pShader->AddTexParam(TEX_1, "Normal Mapping Texture");

	AddRes(pShader->GetKey(), pShader);

	// ============================
    // SkyBoxShader
    // RS_TYPE : CULL_FRONT
    // DS_TYPE : LESS_EQUAL
    // BS_TYPE : DEFAULT	 
    // Domain : MASK
    // ============================
	pShader = new CGraphicsShader;
	pShader->SetKey(L"SkyBoxShader");
	pShader->CreateVertexShader(L"shader\\skybox.fx", "VS_SkyBoxShader");
	pShader->CreatePixelShader(L"shader\\skybox.fx", "PS_SkyBoxShader");
	pShader->SetRSType(RS_TYPE::CULL_FRONT);
	pShader->SetDSType(DS_TYPE::LESS_EQUAL);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_MASK);

	pShader->AddTexParam(TEX_0, "Output Texture");

	AddRes(pShader->GetKey(), pShader);
	// ============================
    // Decal Shader
    // RS_TYPE : CULL_FRONT
    // DS_TYPE : NoTest_NoWirte
    // BS_TYPE : Alpha
    // Domain  : Decal
    // ============================
    pShader = new CGraphicsShader;
    pShader->SetKey(L"DecalShader");

    pShader->CreateVertexShader(L"shader\\decal.fx", "VS_Decal");
    pShader->CreatePixelShader(L"shader\\decal.fx", "PS_Decal");

    pShader->SetRSType(RS_TYPE::CULL_FRONT);
    pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetBSType(BS_TYPE::ALPHA_BLEND);
    pShader->SetDomain(SHADER_DOMAIN::DOMAIN_DECAL);

    // Parameter	
    pShader->AddTexParam(TEX_1, "Output Texture");

    AddRes(pShader->GetKey(), pShader);

	// ============================
	// Std3D_Deferred
	// RS_TYPE : CULL_FRONT
	// DS_TYPE : LESS
	// BS_TYPE : DEFAULT
	// Domain : Deferred
	// ============================
	pShader = new CGraphicsShader;
	pShader->SetKey(L"Std3D_DeferredShader");

	pShader->CreateVertexShader(L"shader\\std3d_deferred.fx", "VS_Std3D_Deferred");
	pShader->CreatePixelShader(L"shader\\std3d_deferred.fx", "PS_Std3D_Deferred");

	pShader->SetRSType(RS_TYPE::CULL_BACK);
	//pShader->SetRSType(RS_TYPE::WIRE_FRAME);
	pShader->SetDSType(DS_TYPE::LESS_EQUAL);
	//pShader->SetDSType(DS_TYPE::STENCIL_CULL_TEST_O);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_DEFERRED);

	// Parameter	
	pShader->AddTexParam(TEX_0, "Output Texture");

	AddRes(pShader->GetKey(), pShader);

	// ============================
	// StencilCullShader1
	// RS_TYPE : CULL_NONE
	// DS_TYPE : STENCIL_CULL
	// BS_TYPE : DEFAULT
	// Domain : Deferred
	// ============================
	pShader = new CGraphicsShader;
	pShader->SetKey(L"StencilCullShader1");

	pShader->CreateVertexShader(L"shader\\light.fx", "VS_MShader");
	pShader->CreatePixelShader(L"shader\\light.fx", "PS_MShader");

	pShader->SetStencilRef(2);
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetDSType(DS_TYPE::STENCIL_CULL_TEST_O);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_POSTPROCESS);


	AddRes(pShader->GetKey(), pShader);

	// ============================
	// StencilDeployShader
	// RS_TYPE : CULL_NONE
	// DS_TYPE : STENCIL_CULL
	// BS_TYPE : DEFAULT
	// Domain : Deferred
	// ============================
	pShader = new CGraphicsShader;
	pShader->SetKey(L"StencilDeployShader");

	pShader->CreateVertexShader(L"shader\\light.fx", "VS_MShader");
	pShader->CreatePixelShader(L"shader\\light.fx", "PS_MShader");

	pShader->SetStencilRef(0);
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetDSType(DS_TYPE::STENCIL_CULL_DEPLOY);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_POSTPROCESS);


	AddRes(pShader->GetKey(), pShader);

	// ============================
	// Planet3D_Deferred
	// RS_TYPE : CULL_FRONT
	// DS_TYPE : LESS
	// BS_TYPE : DEFAULT
	// Domain : Deferred
	// ============================
	pShader = new CGraphicsShader;
	pShader->SetKey(L"Planet3D_DeferredShader");

	pShader->CreateVertexShader(L"shader\\planet3d_deferred.fx", "VS_Planet3D_Deferred");
	pShader->CreatePixelShader(L"shader\\planet3d_deferred.fx", "PS_Planet3D_Deferred");

	pShader->SetRSType(RS_TYPE::CULL_BACK);
	pShader->SetDSType(DS_TYPE::LESS_EQUAL);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_DEFERRED);

	// Parameter	
	pShader->AddTexParam(TEX_0, "Output Texture");

	AddRes(pShader->GetKey(), pShader);

	pShader = new CGraphicsShader;
	pShader->SetKey(L"Planet3D_DeferredShader_Alpha");

	pShader->CreateVertexShader(L"shader\\planet3d_deferred.fx", "VS_Planet3D_Deferred");
	pShader->CreatePixelShader(L"shader\\planet3d_deferred.fx", "PS_Planet3D_Deferred_Alpha");

	pShader->SetRSType(RS_TYPE::CULL_BACK);
	pShader->SetDSType(DS_TYPE::LESS_EQUAL);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_DEFERRED);

	// Parameter	
	pShader->AddTexParam(TEX_0, "Output Texture");

	AddRes(pShader->GetKey(), pShader);

	// ============================
    // DirLightShader
    // RS_TYPE : CULL_BACK
    // DS_TYPE : NO_TEST_NO_WRITE
    // BS_TYPE : ONE_ONE
    // Domain : LIGHT
    // ============================
	pShader = new CGraphicsShader;
	pShader->SetKey(L"DirLightShader");

	pShader->CreateVertexShader(L"shader\\light.fx", "VS_DirLightShader");
	pShader->CreatePixelShader(L"shader\\light.fx", "PS_DirLightShader");

	pShader->SetRSType(RS_TYPE::CULL_BACK);
	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_LIGHT);
	pShader->SetBSType(BS_TYPE::ONE_ONE);

	AddRes(pShader->GetKey(), pShader);

	// ============================
    // PointLightShader
    // RS_TYPE : CULL_BACK
    // DS_TYPE : NO_TEST_NO_WRITE
    // BS_TYPE : ONE_ONE
    // Domain : LIGHT
    // ============================
	pShader = new CGraphicsShader;
	pShader->SetKey(L"PointLightShader");

	pShader->CreateVertexShader(L"shader\\light.fx", "VS_PointLightShader");
	pShader->CreatePixelShader(L"shader\\light.fx", "PS_PointLightShader");

	pShader->SetRSType(RS_TYPE::CULL_FRONT);
	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_LIGHT);
	pShader->SetBSType(BS_TYPE::ONE_ONE);

	AddRes(pShader->GetKey(), pShader);

	// =====================================
	// ShadowMap Shader
	// MRT              : SHADOWMAP
	// Domain           : DOMAIN_LIGHT	
	// Rasterizer       : CULL_BACK
	// DepthStencil     : LESS
	// Blend            : Default
	// =====================================
	pShader = new CGraphicsShader;
	pShader->SetKey(L"ShadowMapShader");

	pShader->CreateVertexShader(L"shader\\light.fx", "VS_ShadowMap");
	pShader->CreatePixelShader(L"shader\\light.fx", "PS_ShadowMap");

	pShader->SetRSType(RS_TYPE::CULL_BACK);
	pShader->SetDSType(DS_TYPE::LESS);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_LIGHT);

	AddRes(pShader->GetKey(), pShader);

	// =====================================
	// ShadowMapExp Shader
	// MRT              : SHADOWMAP
	// Domain           : DOMAIN_LIGHT	
	// Rasterizer       : CULL_BACK
	// DepthStencil     : LESS
	// Blend            : Default
	// =====================================
	pShader = new CGraphicsShader;
	pShader->SetKey(L"ShadowMapExpShader");

	pShader->CreateVertexShader(L"shader\\light.fx", "VS_ShadowMap");
	pShader->CreatePixelShader(L"shader\\light.fx", "PS_ShadowMapExp");

	pShader->SetRSType(RS_TYPE::CULL_BACK);
	pShader->SetDSType(DS_TYPE::LESS);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_LIGHT);

	AddRes(pShader->GetKey(), pShader);

	// ============================
    // SpotLightShader
    // RS_TYPE : CULL_BACK
    // DS_TYPE : NO_TEST_NO_WRITE
    // BS_TYPE : ONE_ONE
    // Domain : LIGHT
    // ============================
	pShader = new CGraphicsShader;
	pShader->SetKey(L"SpotLightShader");

	pShader->CreateVertexShader(L"shader\\light.fx", "VS_SpotLightShader");
	pShader->CreatePixelShader(L"shader\\light.fx", "PS_SpotLightShader");

	pShader->SetRSType(RS_TYPE::CULL_FRONT);
	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_LIGHT);
	pShader->SetBSType(BS_TYPE::ONE_ONE);

	AddRes(pShader->GetKey(), pShader);

	// =====================================
    // MergeShader
    // MRT              : SwapChain
    // Domain           : DOMAIN_LIGHT
    // Mesh             : RectMesh
    // Rasterizer       : CULL_BACK
    // DepthStencil     : NO_TEST_NO_WRITE
    // Blend            : Default
    // =====================================
	pShader = new CGraphicsShader;
	pShader->SetKey(L"MergeShader");

	pShader->CreateVertexShader(L"shader\\light.fx", "VS_MergeShader");
	pShader->CreatePixelShader(L"shader\\light.fx", "PS_MergeShader");

	pShader->SetRSType(RS_TYPE::CULL_BACK);
	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_LIGHT);

	AddRes(pShader->GetKey(), pShader);

	// ============================
	// Deferred Decal Shader
	// RS_TYPE : CULL_FRONT
	// DS_TYPE : NoTest_NoWirte
	// BS_TYPE : DEFEREED_DECAL_BLEND
	// Domain  : Decal
	// ============================
	pShader = new CGraphicsShader;
	pShader->SetKey(L"DeferredDecalShader");

	pShader->CreateVertexShader(L"shader\\decal.fx", "VS_DeferredDecal");
	pShader->CreatePixelShader(L"shader\\decal.fx", "PS_DeferredDecal");

	pShader->SetRSType(RS_TYPE::CULL_FRONT);
	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetBSType(BS_TYPE::DEFERRED_DECAL_BLEND);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_DEFERRED_DECAL);

	// Parameter
	pShader->AddTexParam(TEX_1, "Output Texture");

	AddRes(pShader->GetKey(), pShader);


	// ============================
	// Deferred Area Decal Shader
	// RS_TYPE : CULL_FRONT
	// DS_TYPE : NoTest_NoWirte
	// BS_TYPE : DEFEREED_DECAL_BLEND
	// Domain  : Decal
	// ============================
	pShader = new CGraphicsShader;
	pShader->SetKey(L"DeferredAreaDecalShader");

	pShader->CreateVertexShader(L"shader\\area_decal.fx", "VS_DeferredAreaDecal");
	pShader->CreatePixelShader(L"shader\\area_decal.fx", "PS_DeferredAreaDecal");

	pShader->SetRSType(RS_TYPE::CULL_FRONT);
	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetBSType(BS_TYPE::DEFERRED_DECAL_BLEND);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_DEFERRED_DECAL);

	// Parameter
	pShader->AddTexParam(TEX_1, "Output Texture");

	AddRes(pShader->GetKey(), pShader);

	// =====================================
	// Tess Shader
	// MRT              : SwapChain
	// Domain           : DOMAIN_OPAQUE	
	// Rasterizer       : CULL_NONE
	// DepthStencil     : LESS
	// Blend            : Default
	// =====================================
	pShader = new CGraphicsShader;
	pShader->SetKey(L"TessShader");

	pShader->CreateVertexShader(L"shader\\tess.fx", "VS_Tess");
	pShader->CreateHullShader(L"shader\\tess.fx", "HS_Tess");
	pShader->CreateDomainShader(L"shader\\tess.fx", "DS_Tess");
	pShader->CreatePixelShader(L"shader\\tess.fx", "PS_Tess");

	pShader->SetRSType(RS_TYPE::WIRE_FRAME);
	//pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetDSType(DS_TYPE::LESS);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_OPAQUE);

	pShader->SetTopology(D3D_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);

	AddRes(pShader->GetKey(), pShader);

}


#include "CSetColorShader.h"
#include "CParticleUpdateShader.h"

void CResMgr::CreateDefaultComputeShader()
{
	Ptr<CComputeShader> pCS = nullptr;

	// Texture 색상 변경 쉐이더
	pCS = new CSetColorShader(32, 32, 1);
	pCS->SetKey(L"SetColorCS");
	pCS->CreateComputeShader(L"shader\\setcolor.fx", "CS_SetColor");
	AddRes(pCS->GetKey(), pCS);

	// Particle Update 쉐이더
	pCS = new CParticleUpdateShader(128, 1, 1);
	pCS->SetKey(L"ParticleUpdateCS");
	pCS->CreateComputeShader(L"shader\\particle_update.fx", "CS_ParticleUpdate");
	AddRes(pCS->GetKey(), pCS);

	// Particle Update 쉐이더
	pCS = new CParticleUpdateShader(1000, 1, 1);
	pCS->SetKey(L"ParticleUpdateCS2");
	pCS->CreateComputeShader(L"shader\\particle_update.fx", "CS_ParticleUpdate2");
	AddRes(pCS->GetKey(), pCS);

	// Animation Matrix Update 쉐이더
	pCS = new CAnimation3D_CShader(256, 1, 1);
	pCS->SetKey(L"Animation3DUpdateCS");
	pCS->CreateComputeShader(L"shader\\animation3d.fx", "CS_Animation3D");
	AddRes(pCS->GetKey(), pCS);
}

void CResMgr::CreateDefaultMaterial()
{
	Ptr<CMaterial> pMtrl = nullptr;

	// Test Material
	pMtrl = new CMaterial(true);
	pMtrl->SetShader(FindRes<CGraphicsShader>(L"TestShader"));
	AddRes(L"TestMtrl", pMtrl);

	// Std2D Material
	pMtrl = new CMaterial(true);
	pMtrl->SetShader(FindRes<CGraphicsShader>(L"Std2DShader"));
	AddRes(L"Std2DMtrl", pMtrl);

	// Std2DAnim Material
	pMtrl = new CMaterial(true);
	pMtrl->SetShader(FindRes<CGraphicsShader>(L"Std2DShader"));
	AddRes(L"Std2DAnimMtrl", pMtrl);

	// Std2DLight Material
	pMtrl = new CMaterial(true);
	pMtrl->SetShader(FindRes<CGraphicsShader>(L"Std2DLightShader"));
	AddRes(L"Std2DLightMtrl", pMtrl);

	// Std2DLight Material
	pMtrl = new CMaterial(true);
	pMtrl->SetShader(FindRes<CGraphicsShader>(L"Std2DLightShader"));
	AddRes(L"Std2DAnimLightMtrl", pMtrl);
	
	// DebugShape Material
	pMtrl = new CMaterial(true);
	pMtrl->SetShader(FindRes<CGraphicsShader>(L"DebugShapeShader"));
	AddRes(L"DebugShapeMtrl", pMtrl);

	// DebugSphereShapeShader Material
	pMtrl = new CMaterial(true);
	pMtrl->SetShader(FindRes<CGraphicsShader>(L"DebugSphereShapeShader"));
	AddRes(L"DebugSphereShapeMtrl", pMtrl);

	// TileMap Material
	pMtrl = new CMaterial(true);
	pMtrl->SetShader(FindRes<CGraphicsShader>(L"TileMapShader"));
	AddRes(L"TileMapMtrl", pMtrl);

	// Particle Render Material
	pMtrl = new CMaterial(true);
	pMtrl->SetShader(FindRes<CGraphicsShader>(L"ParticleRenderShader"));
	AddRes(L"ParticleRenderMtrl", pMtrl);

	// Particle Render Material2
	pMtrl = new CMaterial(true);
	pMtrl->SetShader(FindRes<CGraphicsShader>(L"ParticleRenderShader2"));
	AddRes(L"ParticleRenderMtrl2", pMtrl);

	// GrayShader(PostProcess)
	pMtrl = new CMaterial(true);
	pMtrl->SetShader(FindRes<CGraphicsShader>(L"GrayShader"));
	AddRes(L"GrayMtrl", pMtrl);	

	// DistortionShader(PostProcess)
	pMtrl = new CMaterial(true);
	pMtrl->SetShader(FindRes<CGraphicsShader>(L"DistortionShader"));
	AddRes(L"DistortionMtrl", pMtrl);	

	// TestShader
	pMtrl = new CMaterial(true);
	pMtrl->SetShader(FindRes<CGraphicsShader>(L"TestShader"));
	AddRes(L"TestShaderMtrl", pMtrl);

	pMtrl = new CMaterial(true);
	pMtrl->SetShader(FindRes<CGraphicsShader>(L"Std3DShader"));
	AddRes(L"Std3DShaderMtrl", pMtrl);

	pMtrl = new CMaterial(true);
	pMtrl->SetShader(FindRes<CGraphicsShader>(L"SkyBoxShader"));
	AddRes(L"SkyBoxMtrl", pMtrl);

	// Std3D_DeferredShader
	pMtrl = new CMaterial(true);
	pMtrl->SetShader(FindRes<CGraphicsShader>(L"Std3D_DeferredShader"));
	AddRes(L"Std3D_DeferredMtrl", pMtrl);

	//Planet3D_DeferredShader
	pMtrl = new CMaterial(true);
	pMtrl->SetShader(FindRes<CGraphicsShader>(L"Planet3D_DeferredShader"));
	AddRes(L"Planet3D_DeferredMtrl", pMtrl);

	pMtrl = new CMaterial(true);
	pMtrl->SetShader(FindRes<CGraphicsShader>(L"Planet3D_DeferredShader_Alpha"));
	AddRes(L"Planet3D_DeferredAlphaMtrl", pMtrl);
	
	// DirLightMtrl
	pMtrl = new CMaterial(true);
	pMtrl->SetShader(FindRes<CGraphicsShader>(L"DirLightShader"));
	AddRes(L"DirLightMtrl", pMtrl);

	// PointLightMtrl
	pMtrl = new CMaterial(true);
	pMtrl->SetShader(FindRes<CGraphicsShader>(L"PointLightShader"));
	AddRes(L"PointLightMtrl", pMtrl);

	// MergeMtrl
	pMtrl = new CMaterial(true);
	pMtrl->SetShader(FindRes<CGraphicsShader>(L"MergeShader"));
	AddRes(L"MergeMtrl", pMtrl);

	// DecalMtrl
	pMtrl = new CMaterial(true);
	pMtrl->SetShader(FindRes<CGraphicsShader>(L"DecalShader"));
	AddRes(L"DecalMtrl", pMtrl);

	// DeferredDecalMtrl
	pMtrl = new CMaterial(true);
	pMtrl->SetShader(FindRes<CGraphicsShader>(L"DeferredDecalShader"));
	AddRes(L"DeferredDecalMtrl", pMtrl);

	pMtrl = new CMaterial(true);
	pMtrl->SetShader(FindRes<CGraphicsShader>(L"DeferredAreaDecalShader"));
	AddRes(L"DeferredAreaDecalMtrl", pMtrl);

	
	pMtrl = new CMaterial(true);
	pMtrl->SetShader(FindRes<CGraphicsShader>(L"StencilCullShader1"));
	AddRes(L"StencilCullMtrl1", pMtrl);

	
	pMtrl = new CMaterial(true);
	pMtrl->SetShader(FindRes<CGraphicsShader>(L"StencilDeployShader"));
	AddRes(L"StencilDeployMtrl", pMtrl);

	// ShadowMapMtrl
	pMtrl = new CMaterial(true);
	pMtrl->SetShader(FindRes<CGraphicsShader>(L"ShadowMapShader"));
	AddRes(L"ShadowMapMtrl", pMtrl);

	pMtrl = new CMaterial(true);
	pMtrl->SetShader(FindRes<CGraphicsShader>(L"ShadowMapExpShader"));
	AddRes(L"ShadowMapExpMtrl", pMtrl);
	
	// TessMtrl
	pMtrl = new CMaterial(true);
	pMtrl->SetShader(FindRes<CGraphicsShader>(L"TessShader"));
	AddRes(L"TessMtrl", pMtrl);	
}

Ptr<CTexture> CResMgr::CreateTexture(const wstring& _strKey, UINT _Width, UINT _Height
	, DXGI_FORMAT _pixelformat, UINT _BindFlag, D3D11_USAGE _Usage)
{
	Ptr<CTexture> pTex =  FindRes<CTexture>(_strKey);

	assert(nullptr == pTex);

	pTex = new CTexture(true);
	if (FAILED(pTex->Create(_Width, _Height, _pixelformat, _BindFlag, _Usage)))
	{
		assert(nullptr);
	}

	AddRes<CTexture>(_strKey, pTex);

	return pTex;
}

Ptr<CTexture> CResMgr::CreateTexture(const wstring& _strKey, ComPtr<ID3D11Texture2D> _Tex2D)
{
	Ptr<CTexture> pTex = FindRes<CTexture>(_strKey);

	assert(nullptr == pTex);

	pTex = new CTexture(true);
	if (FAILED(pTex->Create(_Tex2D)))
	{
		assert(nullptr);
	}

	AddRes<CTexture>(_strKey, pTex);

	return pTex;
}

Ptr<CMeshData> CResMgr::LoadFBX(const wstring& _strPath)
{
	wstring strFileName = path(_strPath).stem();

	wstring strName = L"meshdata\\";
	strName += strFileName + L".mdat";

	//Ptr<CMeshData> pMeshData = FindRes<CMeshData>(strName);

	Ptr<CMeshData> pMeshData;

	//if (nullptr != pMeshData)
	//	return pMeshData;

	pMeshData = CMeshData::LoadFromFBX(_strPath);
	pMeshData->SetKey(strName);
	pMeshData->SetRelativePath(strName);

	m_arrRes[(UINT)RES_TYPE::MESHDATA].insert(make_pair(strName, pMeshData.Get()));

	// meshdata 를 실제파일로 저장
	pMeshData->Save(strName);

	return pMeshData;
}


void CResMgr::DeleteRes(RES_TYPE _type, const wstring& _strKey)
{
	map<wstring, Ptr<CRes>>::iterator iter = m_arrRes[(UINT)_type].find(_strKey);

	assert(!(iter == m_arrRes[(UINT)_type].end()));

	m_arrRes[(UINT)_type].erase(iter);	

	m_Changed = true;
}
