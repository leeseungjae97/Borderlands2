#include "pch.h"

#include "CEventMgr.h"
#include "CLevelMgr.h"
#include "CLevel.h"
#include "CLayer.h"
#include "CGameObject.h"
#include "CMeshRender.h"
#include "CTransform.h"
#include "CRenderMgr.h"
#include "ptr.h"
#include "CResMgr.h"

static int testObjectNumbering = 0;
static vector<CGameObject*> vecTestObjGC;

void PreloadGameObject(CGameObject* _NewObject, Vec3 _vWorldPos, int _LayerIdx)
{
	_NewObject->Transform()->SetRelativePos(_vWorldPos);
	_NewObject->SetPreLoadingObject(true);
	CLevelMgr::GetInst()->GetCurLevel()->AddGameObject(_NewObject, _LayerIdx, false);
}

void PreloadGameObject(CGameObject* _NewObject, Vec3 _vWorldPos, LAYER_TYPE _Type)
{
	_NewObject->Transform()->SetRelativePos(_vWorldPos);
	_NewObject->SetPreLoadingObject(true);
	CLevelMgr::GetInst()->GetCurLevel()->AddGameObject(_NewObject, _Type, false);
}

void SpawnGameObject(CGameObject* _NewObject, Vec3 _vWorldPos, int _LayerIdx)
{
	_NewObject->Transform()->SetRelativePos(_vWorldPos);

	tEvent evn = {};

	evn.Type = EVENT_TYPE::CREATE_OBJECT;
	evn.wParam = (DWORD_PTR)_NewObject;
	evn.lParam = _LayerIdx;

	CEventMgr::GetInst()->AddEvent(evn);
}

void SpawnGameObject(CGameObject* _NewObject, Vec3 _vWorldPos, LAYER_TYPE _Type, bool _IntergratedCollider)
{
	if(!_IntergratedCollider)
	{
		_NewObject->Transform()->SetRelativePos(_vWorldPos);
	}
	
	tEvent evn = {};

	evn.Type = EVENT_TYPE::CREATE_OBJECT;
	evn.wParam = (DWORD_PTR)_NewObject;
	evn.lParam = CLevelMgr::GetInst()->GetCurLevel()->FindLayerByType(_Type)->GetLayerIndex();

	CEventMgr::GetInst()->AddEvent(evn);
}

float RadianToDegree(float radian)
{
	return XMConvertToDegrees(radian);
}

float DegreeToRadian(float degree)	
{
	return XMConvertToRadians(degree);
}

bool areAlmostEqual(float a, float b, float epsilon)
{
	return std::abs(a - b) < epsilon;
}

void TestSpawnGameObject(Vec3 _WorldPos, int _LayerIndex, Ptr<CMesh> _Mesh)
{
	CGameObject* testObject = new CGameObject;
	vecTestObjGC.push_back(testObject);
	testObject->SetName(L"testObject" + std::to_wstring(testObjectNumbering));

	++testObjectNumbering;

	testObject->AddComponent(new CTransform);
	testObject->Transform()->SetRelativePos(_WorldPos);
	testObject->Transform()->SetRelativeScale(Vec3(200.f, 200.f, 200.f));

	testObject->AddComponent(new CMeshRender);
	if (nullptr != _Mesh.Get())
	{
		testObject->MeshRender()->SetMesh(_Mesh);
	}
	else
	{
		testObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"SphereMesh"));
	}

	testObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Std3D_DeferredMtrl"), 0);
	//testObject->MeshRender()->GetMaterial(0)->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"texture\\tile\\TILE_01.tga"));

	tEvent evn = {};

	evn.Type = EVENT_TYPE::CREATE_OBJECT;
	evn.wParam = (DWORD_PTR)testObject;
	evn.lParam = _LayerIndex;

	CEventMgr::GetInst()->AddEvent(evn);
}
void TestPreloadGameObject(Vec3 _WorldPos, int _LayerIndex, Ptr<CMesh> _Mesh)
{

	CGameObject* testObject = new CGameObject;
	testObject->SetName(L"testObject" + std::to_wstring(testObjectNumbering));
	testObject->SetPreLoadingObject(true);
	++testObjectNumbering;

	testObject->AddComponent(new CTransform);
	testObject->Transform()->SetRelativePos(_WorldPos);
	testObject->Transform()->SetRelativeScale(Vec3(10.f, 10.f, 10.f));

	testObject->AddComponent(new CMeshRender);
	if(nullptr != _Mesh.Get())
	{
		testObject->MeshRender()->SetMesh(_Mesh);
	}else
	{
		testObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"SphereMesh"));
	}
	
	testObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Std3D_DeferredMtrl"), 0);
	testObject->MeshRender()->GetMaterial(0)->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"texture\\red.png"));
	testObject->Transform()->SetRelativePos(_WorldPos);
	CLevelMgr::GetInst()->GetCurLevel()->AddGameObject(testObject, _LayerIndex, false);
}

void DeleteTestObject()
{
	for(CGameObject* obj : vecTestObjGC)
	{
		DestroyObject(obj);
	}
	vecTestObjGC.clear();
}

void ChangeCurLevel(CLevel* _ChangeLevel)
{
	tEvent evn = {};

	evn.Type = EVENT_TYPE::LEVEL_CHANGE;
	evn.wParam = (DWORD_PTR)_ChangeLevel;

	CEventMgr::GetInst()->AddEvent(evn);
}

void LevelRecognize()
{
	tEvent evn = {};
	evn.Type = EVENT_TYPE::LEVEL_RECOG;
	CEventMgr::GetInst()->AddEvent(evn);
}


void DestroyObject(CGameObject* _DeletObject)
{
	if (_DeletObject->IsDead())
		return;

	tEvent evn = {};

	evn.Type = EVENT_TYPE::DELETE_OBJECT;
	evn.wParam = (DWORD_PTR)_DeletObject;

	CEventMgr::GetInst()->AddEvent(evn);
}

void DrawDebugRect(Vec3 _vWorldPos, Vec2 _vWorldScale, Vec4 _vColor
	, Vec3 _vRotation, float _fTime, bool DepthTest)
{
	tDebugShapeInfo info = {};

	info.matWorld = XMMatrixIdentity();
	info.eShape = SHAPE_TYPE::RECT;
	info.fMaxTime = _fTime;
	info.vWorldPos = _vWorldPos;
	info.vWorldScale = Vec3(_vWorldScale.x, _vWorldScale.y, 1.f);
	info.vWorldRotation = _vRotation;
	info.vColor = _vColor;
	info.bDepthTest = DepthTest;

	CRenderMgr::GetInst()->AddDebugShapeInfo(info);
}

void DrawDebugRect(const Matrix& _matWorld, Vec4 _vColor, float _fTime, bool DepthTest)
{
	tDebugShapeInfo info = {};

	info.matWorld = _matWorld;
	info.eShape = SHAPE_TYPE::RECT;
	info.fMaxTime = _fTime;
	info.vColor = _vColor;
	info.bDepthTest = DepthTest;

	CRenderMgr::GetInst()->AddDebugShapeInfo(info);
}

void DrawDebugCircle(Vec3 _vWorldPos, float _fRadius, Vec4 _vColor, Vec3 _vRotation
	, float _fTime, bool DepthTest)
{
	tDebugShapeInfo info = {};

	info.matWorld = XMMatrixIdentity();
	info.eShape = SHAPE_TYPE::CIRCLE;
	info.fMaxTime = _fTime;
	info.vWorldPos = _vWorldPos;
	info.vWorldScale = Vec3(_fRadius, _fRadius, 1.f);
	info.vWorldRotation = _vRotation;
	info.vColor = _vColor;
	info.bDepthTest = DepthTest;

	CRenderMgr::GetInst()->AddDebugShapeInfo(info);
}

void DrawDebugCircle(const Matrix& _matWorld, Vec4 _vColor, float _fTime, bool DepthTest)
{
	tDebugShapeInfo info = {};

	info.matWorld = _matWorld;
	info.eShape = SHAPE_TYPE::CIRCLE;
	info.fMaxTime = _fTime;
	info.vColor = _vColor;
	info.bDepthTest = DepthTest;

	CRenderMgr::GetInst()->AddDebugShapeInfo(info);
}

void DrawDebugCube(Vec3 _vWorldPos, float _fRadius, Vec4 _vColor
	, Vec3 _vRotation, float _fTime, bool DepthTest)
{
	tDebugShapeInfo info = {};

	info.matWorld = XMMatrixIdentity();
	info.eShape = SHAPE_TYPE::CUBE;
	info.fMaxTime = _fTime;
	info.vWorldPos = _vWorldPos;
	info.vWorldScale = Vec3(_fRadius, _fRadius, 1.f);
	info.vWorldRotation = _vRotation;
	info.vColor = _vColor;
	info.bDepthTest = DepthTest;

	CRenderMgr::GetInst()->AddDebugShapeInfo(info);
}

void DrawDebugCube(const Matrix& _matWorld, Vec4 _vColor, float _fTime, bool DepthTest)
{
	tDebugShapeInfo info = {};

	info.matWorld = _matWorld;
	info.eShape = SHAPE_TYPE::CUBE;
	info.fMaxTime = _fTime;
	info.vColor = _vColor;
	info.bDepthTest = DepthTest;

	CRenderMgr::GetInst()->AddDebugShapeInfo(info);
}

void DrawDebugSphere(Vec3 _vWorldPos, float _fRadius, Vec4 _vColor
	, Vec3 _vRotation, float _fTime, bool DepthTest)
{
	tDebugShapeInfo info = {};

	info.matWorld = XMMatrixIdentity();
	info.eShape = SHAPE_TYPE::SPHERE;
	info.fMaxTime = _fTime;
	info.vWorldPos = _vWorldPos;
	info.vWorldScale = Vec3(_fRadius, _fRadius, 1.f);
	info.vWorldRotation = _vRotation;
	info.vColor = _vColor;
	info.bDepthTest = DepthTest;

	CRenderMgr::GetInst()->AddDebugShapeInfo(info);
}

void DrawDebugSphere(const Matrix& _matWorld, Vec4 _vColor, float _fTime, bool DepthTest)
{
	tDebugShapeInfo info = {};

	info.matWorld = _matWorld;
	info.eShape = SHAPE_TYPE::SPHERE;
	info.fMaxTime = _fTime;
	info.vColor = _vColor;
	info.bDepthTest = DepthTest;

	CRenderMgr::GetInst()->AddDebugShapeInfo(info);
}
void DrawDebugLine(const Matrix& matWorld, Vec3 _vWorldPos, float _fRadius, Vec4 _vColor, float _fTime, bool DepthTest)
{
	tDebugShapeInfo info = {};

	info.matWorld = matWorld;

	info.eShape = SHAPE_TYPE::FRONT_LINE;
	;
	info.vWorldScale = Vec3(_fRadius, _fRadius, _fRadius);
	info.fMaxTime = _fTime;
	info.vWorldPos = _vWorldPos;
	//info.vWorldRotation = Vec3(cosf(angle), sinf(-angle), 0.f);
	//info.vDir = _vDir;
	info.vColor = _vColor;
	info.bDepthTest = DepthTest;

	CRenderMgr::GetInst()->AddDebugShapeInfo(info);
}
void DrawDebugLine(DIR_TYPE _dirType, Vec3 _vWorldPos, float _fRadius, Vec4 _vColor, Vec3 _vRotation, float _fTime, bool DepthTest)
{
	tDebugShapeInfo info = {};

	info.matWorld = XMMatrixIdentity();

	if (_dirType == DIR_TYPE::UP)
		info.eShape = SHAPE_TYPE::UP_LINE;
	else if (_dirType == DIR_TYPE::FRONT)
		info.eShape = SHAPE_TYPE::FRONT_LINE;
	else
		info.eShape = SHAPE_TYPE::RIGHT_LINE;

	info.vWorldScale = Vec3(_fRadius, _fRadius, _fRadius);
	info.fMaxTime = _fTime;
	info.vWorldPos = _vWorldPos;
	info.vWorldRotation = _vRotation;
	info.vColor = _vColor;
	info.bDepthTest = DepthTest;

	CRenderMgr::GetInst()->AddDebugShapeInfo(info);
}

void DrawDebugLine(DIR_TYPE _dirType, const Matrix& _matWorld, Vec4 _vColor, float _fTime, bool DepthTest)
{
	tDebugShapeInfo info = {};

	info.matWorld = _matWorld;
	if(_dirType == DIR_TYPE::UP)
		info.eShape = SHAPE_TYPE::UP_LINE;
	else if (_dirType == DIR_TYPE::FRONT)
		info.eShape = SHAPE_TYPE::FRONT_LINE;
	else
		info.eShape = SHAPE_TYPE::RIGHT_LINE;

	info.fMaxTime = _fTime;
	info.vColor = _vColor;
	info.bDepthTest = DepthTest;

	CRenderMgr::GetInst()->AddDebugShapeInfo(info);
}

void DrawDebugMesh(const Matrix& _matWorld, const wstring& _wsDebugShapeName, int _iMtrlCnt, Vec4 _vColor, float _fTime, bool DepthTest)
{
	tDebugShapeInfo info = {};

	info.matWorld = _matWorld;
	info.eShape = SHAPE_TYPE::MESH;
	info.fMaxTime = _fTime;
	info.vColor = _vColor;
	info.bDepthTest = DepthTest;
	info.iMtrlCount = _iMtrlCnt;
	info.wsDebugShapeName = _wsDebugShapeName;

	CRenderMgr::GetInst()->AddDebugShapeInfo(info);
}

bool IsValidObj(CGameObject*& _Target)
{
	if (nullptr == _Target)
		return false;

	if (_Target->IsDead())
	{
		_Target = nullptr;
		return false;
	}

	return true;
}

const char* ToString(RES_TYPE type)
{
	return RES_TYPE_STR[(UINT)type];
}

const wchar_t* ToWString(RES_TYPE type)
{
	return RES_TYPE_WSTR[(UINT)type];
}


const char* ToString(COMPONENT_TYPE type)
{
	return COMPONENT_TYPE_STR[(UINT)type];
}

const char* ToString(PLAYER_ANIMATION_TYPE type)
{
	return PLAYER_ANIMATION_TYPE_STR[(UINT)type];
}
wstring GetRelativePath(const wstring& _strBase, const wstring& _strPath)
{
	wstring strRelativePath;
	if (-1 == _strPath.find(_strBase))
	{
		return strRelativePath;
	}

	strRelativePath = _strPath.substr(_strBase.length(), _strPath.length());
	return strRelativePath;

	return wstring();
}
void SaveWString(const wstring& _str, FILE* _File)
{
	UINT iLen = (UINT)_str.length();
	fwrite(&iLen, sizeof(UINT), 1, _File);
	fwrite(_str.c_str(), sizeof(wchar_t), _str.length(), _File);
}

void LoadWString(wstring& _str, FILE* _File)
{
	//if (L"" == _str || nullptr == _File) return;
	wchar_t szBuffer[256] = {};

	UINT iLen = 0;
	fread(&iLen, sizeof(UINT), 1, _File);
	fread(szBuffer, sizeof(wchar_t), iLen, _File);

	_str = szBuffer;
}

Matrix GetMatrixFromFbxMatrix(FbxAMatrix& _mat)
{
	Matrix mat;
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			mat.m[i][j] = (float)_mat.Get(i, j);
		}
	}
	return mat;
}

int GetSizeofFormat(DXGI_FORMAT _eFormat)
{
	int iRetByte = 0;
	switch (_eFormat)
	{
	case DXGI_FORMAT_R32G32B32A32_TYPELESS:
	case DXGI_FORMAT_R32G32B32A32_FLOAT:
	case DXGI_FORMAT_R32G32B32A32_UINT:
	case DXGI_FORMAT_R32G32B32A32_SINT:
		iRetByte = 128;
		break;

	case DXGI_FORMAT_R32G32B32_TYPELESS:
	case DXGI_FORMAT_R32G32B32_FLOAT:
	case DXGI_FORMAT_R32G32B32_UINT:
	case DXGI_FORMAT_R32G32B32_SINT:
		iRetByte = 96;
		break;
	case DXGI_FORMAT_R16G16B16A16_TYPELESS:
	case DXGI_FORMAT_R16G16B16A16_FLOAT:
	case DXGI_FORMAT_R16G16B16A16_UNORM:
	case DXGI_FORMAT_R16G16B16A16_UINT:
	case DXGI_FORMAT_R16G16B16A16_SNORM:
	case DXGI_FORMAT_R16G16B16A16_SINT:
	case DXGI_FORMAT_R32G32_TYPELESS:
	case DXGI_FORMAT_R32G32_FLOAT:
	case DXGI_FORMAT_R32G32_UINT:
	case DXGI_FORMAT_R32G32_SINT:
	case DXGI_FORMAT_R32G8X24_TYPELESS:
	case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
	case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
	case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
		iRetByte = 64;
		break;
	case DXGI_FORMAT_R10G10B10A2_TYPELESS:
	case DXGI_FORMAT_R10G10B10A2_UNORM:
	case DXGI_FORMAT_R10G10B10A2_UINT:
	case DXGI_FORMAT_R11G11B10_FLOAT:
	case DXGI_FORMAT_R8G8B8A8_TYPELESS:
	case DXGI_FORMAT_R8G8B8A8_UNORM:
	case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
	case DXGI_FORMAT_R8G8B8A8_UINT:
	case DXGI_FORMAT_R8G8B8A8_SNORM:
	case DXGI_FORMAT_R8G8B8A8_SINT:
	case DXGI_FORMAT_R16G16_TYPELESS:
	case DXGI_FORMAT_R16G16_FLOAT:
	case DXGI_FORMAT_R16G16_UNORM:
	case DXGI_FORMAT_R16G16_UINT:
	case DXGI_FORMAT_R16G16_SNORM:
	case DXGI_FORMAT_R16G16_SINT:
	case DXGI_FORMAT_R32_TYPELESS:
	case DXGI_FORMAT_D32_FLOAT:
	case DXGI_FORMAT_R32_FLOAT:
	case DXGI_FORMAT_R32_UINT:
	case DXGI_FORMAT_R32_SINT:
	case DXGI_FORMAT_R24G8_TYPELESS:
	case DXGI_FORMAT_D24_UNORM_S8_UINT:
	case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
	case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
	case DXGI_FORMAT_B8G8R8A8_UNORM:
	case DXGI_FORMAT_B8G8R8X8_UNORM:
		iRetByte = 32;
		break;
	case DXGI_FORMAT_R8G8_TYPELESS:
	case DXGI_FORMAT_R8G8_UNORM:
	case DXGI_FORMAT_R8G8_UINT:
	case DXGI_FORMAT_R8G8_SNORM:
	case DXGI_FORMAT_R8G8_SINT:
	case DXGI_FORMAT_R16_TYPELESS:
	case DXGI_FORMAT_R16_FLOAT:
	case DXGI_FORMAT_D16_UNORM:
	case DXGI_FORMAT_R16_UNORM:
	case DXGI_FORMAT_R16_UINT:
	case DXGI_FORMAT_R16_SNORM:
	case DXGI_FORMAT_R16_SINT:
	case DXGI_FORMAT_B5G6R5_UNORM:
	case DXGI_FORMAT_B5G5R5A1_UNORM:
		iRetByte = 16;
		break;
	case DXGI_FORMAT_R8_TYPELESS:
	case DXGI_FORMAT_R8_UNORM:
	case DXGI_FORMAT_R8_UINT:
	case DXGI_FORMAT_R8_SNORM:
	case DXGI_FORMAT_R8_SINT:
	case DXGI_FORMAT_A8_UNORM:
		iRetByte = 8;
		break;
		// Compressed format; http://msdn2.microsoft.com/en-us/library/bb694531(VS.85).aspx
	case DXGI_FORMAT_BC2_TYPELESS:
	case DXGI_FORMAT_BC2_UNORM:
	case DXGI_FORMAT_BC2_UNORM_SRGB:
	case DXGI_FORMAT_BC3_TYPELESS:
	case DXGI_FORMAT_BC3_UNORM:
	case DXGI_FORMAT_BC3_UNORM_SRGB:
	case DXGI_FORMAT_BC5_TYPELESS:
	case DXGI_FORMAT_BC5_UNORM:
	case DXGI_FORMAT_BC5_SNORM:
		iRetByte = 128;
		break;
		// Compressed format; http://msdn2.microsoft.com/en-us/library/bb694531(VS.85).aspx
	case DXGI_FORMAT_R1_UNORM:
	case DXGI_FORMAT_BC1_TYPELESS:
	case DXGI_FORMAT_BC1_UNORM:
	case DXGI_FORMAT_BC1_UNORM_SRGB:
	case DXGI_FORMAT_BC4_TYPELESS:
	case DXGI_FORMAT_BC4_UNORM:
	case DXGI_FORMAT_BC4_SNORM:
		iRetByte = 64;
		break;
		// Compressed format; http://msdn2.microsoft.com/en-us/library/bb694531(VS.85).aspx
	case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
		iRetByte = 32;
		break;
		// These are compressed, but bit-size information is unclear.
	case DXGI_FORMAT_R8G8_B8G8_UNORM:
	case DXGI_FORMAT_G8R8_G8B8_UNORM:
		iRetByte = 32;
		break;
	case DXGI_FORMAT_UNKNOWN:
	default:
		return -1;
	}

	return iRetByte / 8;
}

void SaveResRef(Ptr<CRes> _Res, FILE* _File)
{
	int i = 0;
	if (nullptr == _Res)
	{
		fwrite(&i, sizeof(i), 1, _File);
	}
	else
	{
		i = 1;
		fwrite(&i, sizeof(i), 1, _File);
		SaveWString(_Res->GetKey(), _File);
		SaveWString(_Res->GetRelativePath(), _File);
	}
}

const wchar_t* ToWString(COMPONENT_TYPE type)
{
	return COMPONENT_TYPE_WSTR[(UINT)type];
}