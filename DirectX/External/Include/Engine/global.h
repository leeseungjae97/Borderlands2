#pragma once

#include <vector>
#include <list>
#include <map>
#include <string>

using std::vector;
using std::list;
using std::map;
using std::make_pair;
using std::wstring;
using std::string;

#include <typeinfo>
#include <assert.h>
#include <thread>

// FileSystem
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>
using namespace std::experimental;
using namespace std::experimental::filesystem;

#include <wrl.h>
using namespace Microsoft::WRL;

#include <d3d11.h>					// DirectX11
#include <d3dcompiler.h>			// Shader ÄÄÆÄÀÏ
//#include <DirectXMath.h>			// DX Math
//#include <DirectXPackedVector.h>	

#pragma comment(lib, "d3d11")
#pragma comment(lib, "d3dcompiler")

#include "SimpleMath.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;
using namespace DirectX::PackedVector;

typedef DirectX::SimpleMath::Vector2 Vec2;
typedef DirectX::SimpleMath::Vector3 Vec3;
typedef DirectX::SimpleMath::Vector4 Vec4;
typedef DirectX::SimpleMath::Quaternion Quat;

// Fbx Loader
#include <FBXLoader/fbxsdk.h>
#include <PhysX/PxPhysicsAPI.h>

#ifdef _DEBUG
#pragma comment(lib, "FBXLoader/x64/debug/libfbxsdk-md.lib")
#pragma comment(lib, "FBXLoader/x64/debug/libxml2-md.lib")
#pragma comment(lib, "FBXLoader/x64/debug/zlib-md.lib")

#pragma comment(lib, "Recast/x64/debug/Recast.lib")
#pragma comment(lib, "Recast/x64/debug/Detour.lib")
#pragma comment(lib, "Recast/x64/debug/DebugUtils.lib")
#pragma comment(lib, "Recast/x64/debug/DetourCrowd.lib")
#pragma comment(lib, "Recast/x64/debug/DetourTileCache.lib")

#pragma comment(lib, "PhysX/debug/PhysX_64.lib")
#pragma comment(lib, "PhysX/debug/PhysXCharacterKinematic_static_64.lib")
#pragma comment(lib, "PhysX/debug/PhysXCommon_64.lib")
#pragma comment(lib, "PhysX/debug/PhysXCooking_64.lib")
#pragma comment(lib, "PhysX/debug/PhysXExtensions_static_64.lib")
#pragma comment(lib, "PhysX/debug/PhysXFoundation_64.lib")
#pragma comment(lib, "PhysX/debug/PhysXPvdSDK_static_64.lib")

#else
#pragma comment(lib, "FBXLoader/x64/release/libfbxsdk-md.lib")

#pragma comment(lib, "Recast/x64/release/Recast.lib")
#pragma comment(lib, "Recast/x64/release/Detour.lib")
#pragma comment(lib, "Recast/x64/release/DebugUtils.lib")
#pragma comment(lib, "Recast/x64/release/DetourCrowd.lib")
#pragma comment(lib, "Recast/x64/release/DetourTileCache.lib")

#pragma comment(lib, "PhysX/release/PhysX_64.lib")
#pragma comment(lib, "PhysX/release/PhysXCharacterKinematic_static_64.lib")
#pragma comment(lib, "PhysX/release/PhysXCommon_64.lib")
#pragma comment(lib, "PhysX/release/PhysXCooking_64.lib")
#pragma comment(lib, "PhysX/release/PhysXExtensions_static_64.lib")
#pragma comment(lib, "PhysX/release/PhysXFoundation_64.lib")
#pragma comment(lib, "PhysX/release/PhysXPvdSDK_static_64.lib")
#endif

#include "define.h"
#include "struct.h"
#include "func.h"

#include "CSingleton.h"