#include "pch.h"
#include "CLandScape.h"


#include "CCamera.h"
#include "CDecal.h"
#include "CKeyMgr.h"
#include "LandScapeMgr.h"
#include "CLevel.h"
#include "CLayer.h"
#include "CLevelMgr.h"
#include "CRenderMgr.h"
#include "CResMgr.h"
#include "CStructuredBuffer.h"
#include "CTransform.h"

CLandScape::CLandScape()
	: CRenderComponent(COMPONENT_TYPE::LANDSCAPE)
	, m_iFaceX(32)
	, m_iFaceZ(32)
	, m_TessEdge(8)
	, m_TessInside(8)
	, m_iWeightIdx(0)
	, m_vBrushScale(Vec2(0.2f, 0.2f))
{
	//CreateMesh();
	//SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"LandScapeMtrl"), 0);
	m_Recast = new RecastNavi;
	init();

	LandScapeMgr::GetInst()->AddLandScape(this);
}

CLandScape::~CLandScape()
{
	if (nullptr != m_pCrossBuffer)
		delete m_pCrossBuffer;

	if (nullptr != m_pWeightMapBuffer)
		delete m_pWeightMapBuffer;

	if (nullptr != m_Recast)
		delete m_Recast;

	if (nullptr != m_pVertexBuffer)
		delete m_pVertexBuffer;

	LandScapeMgr::GetInst()->DeleteLandScape(this);
}

void CLandScape::SetFaceX(UINT _FaceX)
{
	m_iFaceX = _FaceX;
	CreateMesh();
}

void CLandScape::SetFaceZ(UINT _FaceZ)
{
	m_iFaceZ = _FaceZ;
	CreateMesh();
}

void CLandScape::SetFace(UINT _FaceX, UINT _FaceZ)
{
	m_iFaceX = _FaceX;
	m_iFaceZ = _FaceZ;

	CreateMesh();
}

void CLandScape::CreateMesh()
{
	Vtx v;
	vector<Vtx> vecVtx;
	for (int i = 0; i < m_iFaceZ + 1; ++i)
	{
		for (int j = 0; j < m_iFaceX + 1; ++j)
		{
			v.vPos = Vec3(j, 0.f, i);
			v.vUV = Vec2((float)j, (float)m_iFaceZ - i);
			v.vTangent = Vec3(1.f, 0.f, 0.f);
			v.vNormal = Vec3(0.f, 1.f, 0.f);
			v.vBinormal = Vec3(0.f, 0.f, -1.f);
			v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);

			vecVtx.push_back(v);
		}
	}

	vector<UINT> vecIdx;

	for (int i = 0; i < m_iFaceZ; ++i)
	{
		for (int j = 0; j < m_iFaceX; ++j)
		{
			// 0
			// | \
			// 2--1  
			vecIdx.push_back((m_iFaceX + 1) * (i + 1) + (j));
			vecIdx.push_back((m_iFaceX + 1) * (i)+(j + 1));
			vecIdx.push_back((m_iFaceX + 1) * (i)+(j));

			// 0--1
			//  \ |
			//    2
			vecIdx.push_back((m_iFaceX + 1) * (i + 1) + (j));
			vecIdx.push_back((m_iFaceX + 1) * (i + 1) + (j + 1));
			vecIdx.push_back((m_iFaceX + 1) * (i)+(j + 1));
		}
	}

	Ptr<CMesh> pMesh = new CMesh(true);
	pMesh->Create(vecVtx.data(), (UINT)vecVtx.size(), vecIdx.data(), (UINT)vecIdx.size());
	CResMgr::GetInst()->AddRes<CMesh>(L"LandScapeMesh", pMesh);
	SetMesh(pMesh);

	// Mesh 재설정하고 나면 재질이 날라가기 때문에 다시 설정
	SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"LandScapeMtrl"), 0);
}

void CLandScape::AddHeightMapToMesh()
{
	//Ptr<CMesh> pMesh = CResMgr::GetInst()->FindRes<CMesh>(L"LandScapeMesh");

	//Vtx* vv = pMesh->GetVtxSysMem();

	//vector<float> vertices;
	//vector<int> indices;
	//int vertexCount = pMesh->GetVtxCount();
	//for (int i = 0; i < vertexCount; ++i)
	//{
	//	Vec3 vPos = vv[i].vPos;
	//	//vPos = XMVector3TransformCoord(Vec3(vPos), mWorldMat);
	//	//vPos += ;
	//	vertices.emplace_back(vPos.x);
	//	vertices.emplace_back(vPos.y);
	//	vertices.emplace_back(vPos.z);
	//}
	//vector<tIndexInfo> vecIdxInfo = pMesh->GetIndexInfo();
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
	const UINT rowPitch = m_HeightMap->GetRowPitch();
	const UINT slicePitch =m_HeightMap->GetSlicePitch();

	int a = 0;

}

void CLandScape::SetBrush(Ptr<CTexture> _BrushTex)
{
	m_pBrushTex = _BrushTex;

	if(m_pBrushLineDecal)
		m_pBrushLineDecal->Decal()->SetDecalTex(_BrushTex);
}

void CLandScape::SetHeightMapName(const wstring& name)
{
	m_HeightMapName = name;
	CreateTexture();
}

void CLandScape::SetColorMapName(const wstring& name)
{
	m_ColorMapName = name;
	CreateColorMap();
}


tRaycastOut CLandScape::GetLandScapeRayCastOut()
{
	tRaycastOut ray;
	if(m_pCrossBuffer)
		m_pCrossBuffer->GetData(&ray);

	return ray;
}

void CLandScape::finaltick()
{
	if(KEY_TAP(KEY::_1))
		m_iWeightIdx = 0;
	else if (KEY_TAP(KEY::_2))
		m_iWeightIdx = 1;
	else if (KEY_TAP(KEY::_3))
		m_iWeightIdx = 2;

	RayCasting();
	if (KEY_TAP(KEY::V))
	{
		//m_iWeightIdx++;
		//if (m_iWeightIdx > 3)
		//	m_iWeightIdx = 0;
		//m_iWeightIdx = m_iWeightIdx % 4;
	}

	if (KEY_PRESSED(KEY::Z))
	{
		// 교점 위치정보를 토대로 높이를 수정 함
		m_pCSHeightMap->SetInputBuffer(m_pCrossBuffer); // 픽킹 정보를 HeightMapShader 에 세팅

		m_pCSHeightMap->SetBrushTex(m_pBrushTex);		// 사용할 브러쉬 텍스쳐 세팅
		m_pCSHeightMap->SetBrushIndex(0);				// 브러쉬 인덱스 설정
		m_pCSHeightMap->SetBrushScale(m_vBrushScale);   // 브러쉬 크기
		m_pCSHeightMap->SetHeightMap(m_HeightMap);
		m_pCSHeightMap->Execute();
		//m_HeightMap->GetRowPitch();
	}
	if(KEY_PRESSED(KEY::X))
	{
		m_pCSColorMap->SetInputBuffer(m_pCrossBuffer);
		m_pCSColorMap->SetBrushTex(m_pBrushTex);
		m_pCSColorMap->SetBrushScale(m_vBrushScale);
		m_pCSColorMap->SetColorMap(m_ColorMap);
		m_pCSColorMap->Execute();
	}
	if(KEY_PRESSED(KEY::C))
	{
		// 교점 위치정보를 토대로 가중치를 수정함	
		m_pCSWeightMap->SetInputBuffer(m_pCrossBuffer); // 레이 캐스트 위치
		m_pCSWeightMap->SetBrushArrTex(m_pBrushTex);
		m_pCSWeightMap->SetBrushIndex(0);
		m_pCSWeightMap->SetBrushScale(m_vBrushScale); // 브러쉬 크기
		m_pCSWeightMap->SetWeightMap(m_pWeightMapBuffer, m_iWeightWidth, m_iWeightHeight); // 가중치맵, 가로 세로 개수			
		m_pCSWeightMap->SetWeightIdx(m_iWeightIdx);
		m_pCSWeightMap->Execute();
	}
}

void CLandScape::render()
{
	if (GetMesh() == nullptr || GetMaterial(0) == nullptr)
		return;

	Transform()->UpdateData();

	//GetMaterial()->GetShader()->SetRSType(RS_TYPE::WIRE_FRAME);
	m_pVertexBuffer->UpdateData_CS(0, false);

	GetMaterial(0)->SetScalarParam(INT_0, &m_iFaceX);
	GetMaterial(0)->SetScalarParam(INT_1, &m_iFaceZ);
	GetMaterial(0)->SetTexParam(TEX_2, m_HeightMap);
	GetMaterial(0)->SetTexParam(TEX_3, m_ColorMap);

	Vec2 vResolution = Vec2(m_HeightMap->Width(), m_HeightMap->Height());
	GetMaterial(0)->SetScalarParam(SCALAR_PARAM::VEC2_0, &vResolution);
	GetMaterial(0)->SetTexParam(TEX_PARAM::TEX_2, m_HeightMap);

	// 가중치 버퍼 전달
	m_pWeightMapBuffer->UpdateData(17, PIPELINE_STAGE::PS_PIXEL);

	// 가중치 버퍼 해상도 전달
	Vec2 vWeightMapResolution = Vec2((float)m_iWeightWidth, (float)m_iWeightHeight);
	GetMaterial(0)->SetScalarParam(SCALAR_PARAM::VEC2_1, &vWeightMapResolution);

	// 타일 배열 개수 전달
	float m_fTileCount = float(m_pTileArrTex->GetArraySize() / 2); // 색상, 노말 합쳐져있어서 나누기 2 해줌
	GetMaterial(0)->SetScalarParam(SCALAR_PARAM::FLOAT_1, &m_fTileCount);

	// 타일 텍스쳐 전달
	GetMaterial(0)->SetTexParam(TEX_PARAM::TEX_ARR_0, m_pTileArrTex);

	// Camera World Pos 전달
	Vec3 vCamWorldPos = CRenderMgr::GetInst()->GetMainCam()->Transform()->GetWorldPos();
	GetMaterial(0)->SetScalarParam(VEC4_0, &vCamWorldPos);

	GetMaterial(0)->UpdateData();

	GetMesh()->render(0);

	m_pWeightMapBuffer->Clear();
}

void CLandScape::render(UINT _iSubset)
{
	render();
}

void CLandScape::Bake()
{
	//DeleteTestObject();
	//m_vecNode.clear();

	//Vec3 vPos = Transform()->GetRelativePos();
	//Vec3 m_vScale = Transform()->GetRelativeScale();

	//float _x = m_vScale.x / 2.f;
	//float _z = m_vScale.z / 2.f;
	//for (int i = 0; i < m_iFaceZ; ++i)
	//{
	//	for (int j = 0; j < m_iFaceX; ++j)
	//	{
	//		tNode node;
	//		node.iCoordX = j;
	//		node.iCoordY = i;
	//		node.vLandPos = vPos;
	//		node.vPos.x = j * m_vScale.x;
	//		node.vPos.y = 0.f;
	//		node.vPos.z = _z + i * m_vScale.z;

	//		TestPreloadGameObject(node.vPos, 0, nullptr);

	//		m_vecNode.push_back(node);
	//	}
	//}

	m_Recast->HandleBuild(GetOwner());
}

void CLandScape::SaveCurMesh()
{
	CResMgr::GetInst()->AddRes<CMesh>(L"LandScapeMesh", GetMesh());
}


void CLandScape::SetLandScapeMesh(Ptr<CMesh> _Mesh)
{
	SetMesh(_Mesh);
	SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"LandScapeMtrl"), 0);
}

void CLandScape::init()
{
	CreateMesh();

	SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"LandScapeMtrl"), 0);

	CreateComputeShader();

	// 레이캐스팅 결과 받는 버퍼
	m_pCrossBuffer = new CStructuredBuffer;
	m_pCrossBuffer->Create(sizeof(tRaycastOut), 1, SB_TYPE::READ_WRITE, true);

	//m_pNodesBuffer = new CStructuredBuffer;
	//m_pNodesBuffer->Create(sizeof(tNode), 1, SB_TYPE::READ_WRITE, false, "m_pNodesBuffer");

	m_pVertexBuffer = new CStructuredBuffer;
	m_pVertexBuffer->Create(sizeof(tStructVertex), 1, SB_TYPE::READ_WRITE, true);
	//m_pNodesBuffer = new CStructuredBuffer;
	//m_pNodesBuffer->Create(sizeof(tNode), 1, SB_TYPE::READ_WRITE, true, "m_pNodesBuffer");
	//m_vecNodes.resize(m_iFaceX * m_iFaceZ);

	//vector<Ptr<CTexture>> vecTex;
	//vecTex.push_back(CResMgr::GetInst()->FindRes<CTexture>(L"texture\\tile\\TILE_01.tga"));
	//vecTex.push_back(CResMgr::GetInst()->FindRes<CTexture>(L"texture\\tile\\TILE_02.tga"));
	//vecTex.push_back(CResMgr::GetInst()->FindRes<CTexture>(L"texture\\tile\\TILE_03.tga"));
	//vecTex.push_back(CResMgr::GetInst()->FindRes<CTexture>(L"texture\\tile\\TILE_04.tga"));
	//vecTex.push_back(CResMgr::GetInst()->FindRes<CTexture>(L"texture\\tile\\TILE_01_N.tga"));
	//vecTex.push_back(CResMgr::GetInst()->FindRes<CTexture>(L"texture\\tile\\TILE_02_N.tga"));
	//vecTex.push_back(CResMgr::GetInst()->FindRes<CTexture>(L"texture\\tile\\TILE_03_N.tga"));
	//vecTex.push_back(CResMgr::GetInst()->FindRes<CTexture>(L"texture\\tile\\TILE_04_N.tga"));

	//m_pTileArrTex->CreateArrayTexture(vecTex, 8);
	//m_pTileArrTex->Save(L"texture\\tile\\TILE_ARRR.dds");
	//CResMgr::GetInst()->AddRes(L"TILE_ARRR.dds", m_pTileArrTex);

	//m_pTileArrTex = CResMgr::GetInst()->Load<CTexture>(L"texture\\tile\\TILE_ARRR.dds", L"texture\\tile\\TILE_ARRR.dds");
	
	//m_pTileArrTex = CResMgr::GetInst()->LoadTexture(L"texture\\tile\\TILE_ARRR.dds", L"texture\\tile\\TILE_ARRR.dds", 8);
	m_pTileArrTex = CResMgr::GetInst()->FindRes<CTexture>(L"texture\\tile\\TILE_ARRR.dds");
	m_pTileArrTex->GenerateMip(8);
}

void CLandScape::CreateComputeShader()
{
	m_pCSHeightMap = (CHeightMapShader*)CResMgr::GetInst()->FindRes<CComputeShader>(L"HeightMapShaderCS").Get();

	m_pCSRaycast = (CRayCastShader*)CResMgr::GetInst()->FindRes<CComputeShader>(L"RaycastShaderCS").Get();

	m_pCSColorMap = (CColorMapShader*)CResMgr::GetInst()->FindRes<CComputeShader>(L"ColorMapShaderCS").Get();

	m_pCSWeightMap = (CWeightMapShader*)CResMgr::GetInst()->FindRes<CComputeShader>(L"WeightMapShader").Get();


	//m_pCSPathInit = (CLandScapePathShader*)CResMgr::GetInst()->FindRes<CComputeShader>(L"PathInitShader").Get();
}

void CLandScape::CreateTexture()
{
	CreateHeightMap();
	CreateColorMap();
	CreateWeightMap();
	m_pBrushTex = CResMgr::GetInst()->FindRes<CTexture>(L"texture\\brush\\Brush_01.png");
}

void CLandScape::CreateHeightMap()
{
	// 높이맵 텍스쳐		
	m_HeightMap = CResMgr::GetInst()->FindRes<CTexture>(m_HeightMapName);
	if (nullptr == m_HeightMap)
	{
		m_HeightMap = CResMgr::GetInst()->CreateTexture(m_HeightMapName
			, 2048, 2048
			, DXGI_FORMAT_R32_FLOAT
			, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS
			, D3D11_USAGE_DEFAULT);
	}
}

void CLandScape::CreateColorMap()
{
	m_ColorMap = CResMgr::GetInst()->FindRes<CTexture>(m_ColorMapName);
	if (nullptr == m_ColorMap)
	{
		m_ColorMap = CResMgr::GetInst()->CreateTexture(m_ColorMapName
			, 2048, 2048
			, DXGI_FORMAT_R32G32B32A32_FLOAT
			, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS
			, D3D11_USAGE_DEFAULT);
	}
}

void CLandScape::CreateWeightMap()
{
	// 가중치 버퍼
	m_iWeightWidth = 1024;
	m_iWeightHeight = 1024;

	m_pWeightMapBuffer = new CStructuredBuffer;
	m_pWeightMapBuffer->Create(sizeof(tWeight_4), m_iWeightWidth * m_iWeightHeight, SB_TYPE::READ_WRITE, false);
}

void CLandScape::RayCasting()
{
	// 시점 카메라를 가져옴
	CCamera* pMainCam = CRenderMgr::GetInst()->GetMainCam();
	if (nullptr == pMainCam)
		return;

	// 월드 기준 광선을 지형의 로컬로 보냄
	const Matrix& matWorldInv = Transform()->GetWorldInvMat();
	const tRay& ray = pMainCam->GetRay();

	tRay CamRay = {};
	CamRay.vStart = XMVector3TransformCoord(ray.vStart, matWorldInv);
	CamRay.vDir = XMVector3TransformNormal(ray.vDir, matWorldInv);
	CamRay.vDir.Normalize();

	// 지형과 카메라 Ray 의 교점을 구함

	tRaycastOut out = { Vec2(0.f, 0.f), 0x7fffff, 0 , Vec3(0.f, 0.f, 0.f), 0.f};
	m_pCrossBuffer->SetData(&out, 1);

	m_pCSRaycast->SetHeightMap(m_HeightMap);
	m_pCSRaycast->SetFaceCount(m_iFaceX, m_iFaceZ);
	m_pCSRaycast->SetCameraRay(CamRay);
	m_pCSRaycast->SetOuputBuffer(m_pCrossBuffer);

	m_pCSRaycast->Execute();

	m_pCrossBuffer->GetData(&out);

	Vec3 pos = XMVector3TransformCoord(out.vCrossPos, Transform()->GetWorldMat());
	Vec3 scale = Vec3(m_HeightMap->Width() * m_vBrushScale.x * 3, m_HeightMap->Width() * 100.f, m_HeightMap->Height() * m_vBrushScale.x * 3);
	if(m_pBrushLineDecal)
	{
		m_pBrushLineDecal->Transform()->SetRelativePos(pos);
		m_pBrushLineDecal->Transform()->SetRelativeScale(scale);
	}
	
}

void CLandScape::LoadFromLevelFile(FILE* _FILE)
{
	fread(&m_iFaceX, sizeof(UINT), 1, _FILE);
	fread(&m_iFaceZ, sizeof(UINT), 1, _FILE);
	fread(&m_vBrushScale, sizeof(Vec2), 1, _FILE);
	fread(&m_iWeightWidth, sizeof(UINT), 1, _FILE);
	fread(&m_iWeightHeight, sizeof(UINT), 1, _FILE);
	fread(&m_iWeightIdx, sizeof(UINT), 1, _FILE);
	fread(&m_eMod, sizeof(UINT), 1, _FILE);

	LoadResRef(m_pBrushTex, _FILE);
	LoadResRef(m_HeightMap, _FILE);
	LoadResRef(m_ColorMap, _FILE);
	LoadResRef(m_pTileArrTex, _FILE);

	LoadWString(m_HeightMapName, _FILE);
	LoadWString(m_ColorMapName, _FILE);

	CreateComputeShader();

	if(nullptr == m_pCrossBuffer)
		m_pCrossBuffer = new CStructuredBuffer;

	m_pCrossBuffer->Create(sizeof(tRaycastOut), 1, SB_TYPE::READ_WRITE, true);

	if(nullptr == m_pWeightMapBuffer)
		m_pWeightMapBuffer = new CStructuredBuffer;

	m_pWeightMapBuffer->Create(sizeof(tWeight_4), m_iWeightWidth * m_iWeightHeight, SB_TYPE::READ_WRITE, false);

	//CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurLevel();
	//pCurLevel->GetLayer(1)->GetObjects();
}

void CLandScape::SaveToLevelFile(FILE* _FILE)
{
	fwrite(&m_iFaceX, sizeof(UINT), 1, _FILE);
	fwrite(&m_iFaceZ, sizeof(UINT), 1, _FILE);
	fwrite(&m_vBrushScale, sizeof(Vec2), 1, _FILE);
	fwrite(&m_iWeightWidth, sizeof(UINT), 1, _FILE);
	fwrite(&m_iWeightHeight, sizeof(UINT), 1, _FILE);
	fwrite(&m_iWeightIdx, sizeof(UINT), 1, _FILE);
	fwrite(&m_eMod, sizeof(UINT), 1, _FILE);

	SaveResRef(m_pBrushTex.Get(), _FILE);
	SaveResRef(m_HeightMap.Get(), _FILE);
	SaveResRef(m_ColorMap.Get(), _FILE);
	SaveResRef(m_pTileArrTex.Get(), _FILE);

	SaveWString(m_HeightMapName, _FILE);
	SaveWString(m_ColorMapName, _FILE);
}
