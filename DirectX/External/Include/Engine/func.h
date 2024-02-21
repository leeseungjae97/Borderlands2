#pragma once

#ifndef FUNC_H
#define FUNC_H

// 오브젝트 생성
class CGameObject;
class CLevel;
class CMesh;
class CRigidBody;
//class Ptr;
void PreloadGameObject(CGameObject* _NewObject, Vec3 _vWorldPos, int _LayerIdx);
void PreloadGameObject(CGameObject* _NewObject, Vec3 _vWorldPos, LAYER_TYPE _Type);

void SpawnGameObject(CGameObject* _NewObject, Vec3 _vWorldPos, int _LayerIdx);
void SpawnGameObject(CGameObject* _NewObject, Vec3 _vWorldPos, LAYER_TYPE _Type, bool _IntergratedCollider = false);

void ChangeCurLevel(CLevel* _ChangeLevel);

void LevelRecognize();
// 오브젝트 삭제
void DestroyObject(CGameObject* _DeletObject);

// DrawDebugShape
void DrawDebugRect(Vec3 _vWorldPos, Vec2 _vWorldScale, Vec4 _vColor, Vec3 _vRotation, float _fTime = 0.f, bool DepthTest = false);
void DrawDebugRect(const Matrix& _matWorld, Vec4 _vColor, float _fTime = 0.f, bool DepthTest = false);

void DrawDebugCircle(Vec3 _vWorldPos, float _fRadius, Vec4 _vColor, Vec3 _vRotation, float _fTime = 0.f, bool DepthTest = false);
void DrawDebugCircle(const Matrix& _matWorld, Vec4 _vColor, float _fTime = 0.f, bool DepthTest = false);

void DrawDebugCube(Vec3 _vWorldPos, float _fRadius, Vec4 _vColor, Vec3 _vRotation, float _fTime = 0.f, bool DepthTest = false);
void DrawDebugCube(const Matrix& _matWorld, Vec4 _vColor, float _fTime = 0.f, bool DepthTest = false);

void DrawDebugSphere(Vec3 _vWorldPos, float _fRadius, Vec4 _vColor, Vec3 _vRotation, float _fTime = 0.f, bool DepthTest = false);
void DrawDebugSphere(const Matrix& _matWorld, Vec4 _vColor, float _fTime = 0.f, bool DepthTest = false);

void DrawDebugLine(const Matrix& matWorld, Vec3 _vWorldPos, float _fRadius, Vec4 _vColor, float _fTime = 0.f, bool DepthTest = false);
void DrawDebugLine(DIR_TYPE _dirType, Vec3 _vWorldPos, float _fRadius, Vec4 _vColor, Vec3 _vRotation, float _fTime = 0.f, bool DepthTest = false);
void DrawDebugLine(DIR_TYPE _dirType, const Matrix& _matWorld, Vec4 _vColor, float _fTime = 0.f, bool DepthTest = false);

void DrawDebugMesh(const Matrix& _matWorld, const wstring& _wsDebugShapeName, Vec4 _vColor, float _fTime = 0.f, bool DepthTest = false);

static void Strtrim(char* s) { char* str_end = s + strlen(s); while (str_end > s && str_end[-1] == ' ') str_end--; *str_end = 0; }
static int  Stricmp(const char* s1, const char* s2) { int d; while ((d = toupper(*s2) - toupper(*s1)) == 0 && *s1) { s1++; s2++; } return d; }
static int  Strnicmp(const char* s1, const char* s2, int n) { int d = 0; while (n > 0 && (d = toupper(*s2) - toupper(*s1)) == 0 && *s1) { s1++; s2++; n--; } return d; }

static string Strsubstr(char* s, const char* substr)
{
	string result = "";
	result.append(s);
	result.append(substr);
	return result;
}
static string Strsubstr(char* s, char* substr)
{
	string result = "";
	result.append(s);
	result.append(substr);
	return result;
}

static wstring StringToWString(string& str)
{
	return wstring(str.begin(), str.end());
}
static string WStringToString(wstring& str)
{
	return string(str.begin(), str.end());
}
static wstring CharToWString(char* _ch)
{
	return wstring(_ch, &_ch[strlen(_ch)]);
}
static bool CharCmpWString(char* _ch, wstring& str)
{
	if (strlen(_ch) != str.length()) return false;
	for(int i = 0 ;  i < strlen(_ch); ++i)
	{
		if (_ch[i] == '\0') break;
		if (_ch[i] != str[i]) return false;
	}

	return true;
}

// GameObject 유효성 체크
bool IsValidObj(CGameObject*& _Target);

const char* ToString(RES_TYPE);
const wchar_t* ToWSTring(RES_TYPE);

const char* ToString(COMPONENT_TYPE);
const wchar_t* ToWSTring(COMPONENT_TYPE);

wstring GetRelativePath(const wstring& _strBase, const wstring& _strPath);

// Save / Load
void SaveWString(const wstring& _str, FILE* _File);
void LoadWString(wstring& _str, FILE* _File);

Matrix GetMatrixFromFbxMatrix(FbxAMatrix& _mat);

int GetSizeofFormat(DXGI_FORMAT _eFormat);

float RadianToDegree(float radian);
float DegreeToRadian(float degree);
bool areAlmostEqual(float a, float b, float epsilon = 0.1f);

class CRes;
template<typename T>
class Ptr;

//Test Code
void TestSpawnGameObject(Vec3 _WorldPos, int _LayerIndex, Ptr<CMesh> _Mesh);
void TestPreloadGameObject(Vec3 _WorldPos, int _LayerIndex, Ptr<CMesh> _Mesh);

void DeleteTestObject();

void SaveResRef(Ptr<CRes> _Res, FILE* _File);

class CResMgr;
template<typename T>
void LoadResRef(Ptr<T>& _Res, FILE* _File)
{
	int i = 0;	
	fread(&i, sizeof(i), 1, _File);
	
	if (i)
	{
		wstring strKey, strRelativePath;
		LoadWString(strKey, _File);
		LoadWString(strRelativePath, _File);

		_Res = CResMgr::GetInst()->Load<T>(strKey, strRelativePath);
	}
}

template<typename T, UINT Size>
void Safe_Del_Array(T* (&arr)[Size])
{
	for (UINT i = 0; i < Size; ++i)
	{
		if (nullptr != arr[i])
			delete arr[i];
	}	
}

template<typename T>
void Safe_Del_Vec(vector<T*>& _vec)
{
	for (size_t i = 0; i < _vec.size(); ++i)
	{
		if (nullptr != _vec[i])
		{
			delete _vec[i];
		}
	}
	_vec.clear();
}

template<typename T1, typename T2>
void Safe_Del_Map(map<T1, T2>& _map)
{
	for (const auto& pair : _map)
	{
		if (nullptr != pair.second)
			delete pair.second;
	}

	_map.clear();
}

template <typename T, UINT _Size>
void DeleteArray(T* (&Arr)[_Size])
{
	for (UINT i = 0; i < _Size; ++i)
	{
		if (nullptr != Arr[i])
			delete Arr[i];
	}
}
template<typename T1, typename T2>
void Copy_Map(const map<T1, T2>& _src, map<T1, T2>& _dest)
{
	for (const auto& pair : _src)
	{
		if (nullptr != pair.second)
		{
			_dest.insert(pair);
		}
	}
}

template<typename T>
void Copy_Vec(vector<T*>& _src, vector<T*>& _dest)
{
	_dest.resize(_src.size());
	for (size_t i = 0; i < _src.size(); ++i)
	{
		if (nullptr != _src[i])
		{
			_dest[i] = _src[i];
		}
	}
}

template <typename T, UINT _Size>
void CopyArray(T (&DestArr)[_Size],const T (&SrcArr)[_Size])
{
	for (UINT i = 0; i < _Size; ++i)
	{
		DestArr[i] = SrcArr[i];
	}
}

template <typename T, UINT _Size>
void CopyArray(T* (&DestArr)[_Size],const T* (&SrcArr)[_Size])
{
	for (UINT i = 0; i < _Size; ++i)
	{
		if (nullptr != SrcArr[i])
			DestArr[i] = SrcArr[i];
	}
}

#endif