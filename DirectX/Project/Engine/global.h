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


// FileSystem
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>
using namespace std::experimental;
using namespace std::experimental::filesystem;



#include <wrl.h>
using namespace Microsoft::WRL;

#include <d3d11.h>					// DirectX11
#include <d3dcompiler.h>			// Shader ÄÄÆÄÀÏ
#include <DirectXMath.h>			// DX Math
#include <DirectXPackedVector.h>	

using namespace DirectX;
using namespace DirectX::PackedVector;

#pragma comment(lib, "d3d11")
#pragma comment(lib, "d3dcompiler")


#include "SimpleMath.h"
using namespace DirectX::SimpleMath;

typedef Vector2 Vec2;
typedef Vector3 Vec3;
typedef Vector4 Vec4;

// Fbx Loader
#include <FBXLoader/fbxsdk.h>

#include <PhysX/PxPhysicsAPI.h>

#ifdef _DEBUG
#pragma comment(lib, "FBXLoader/x64/debug/libfbxsdk-md.lib")

//#pragma comment(lib, "PhysX/debug/PhysXVehicle_static_64.lib")
//#pragma comment(lib, "PhysX/debug/PhysXVehicle2_static_64.lib")
//#pragma comment(lib, "PhysX/debug/PhysXExtensions_static_64.lib")
//#pragma comment(lib, "PhysX/debug/PhysXPvdSDK_static_64.lib")
//#pragma comment(lib, "PhysX/debug/PhysXCharacterKinematic_static_64.lib")
//#pragma comment(lib, "PhysX/debug/PhysXCooking_static_64.lib")
//#pragma comment(lib, "PhysX/debug/PVDRuntime_64.lib")
//#pragma comment(lib, "PhysX/debug/PhysX_static_64.lib")
//#pragma comment(lib, "PhysX/debug/PhysXCommon_static_64.lib")
//#pragma comment(lib, "PhysX/debug/PhysXFoundation_static_64.lib")

#else
#pragma comment(lib, "FBXLoader/x64/release/libfbxsdk-md.lib")

//#pragma comment(lib, "PhysX/release/PhysXVehicle_static_64.lib")
//#pragma comment(lib, "PhysX/release/PhysXVehicle2_static_64.lib")
//#pragma comment(lib, "PhysX/release/PhysXExtensions_static_64.lib")
//#pragma comment(lib, "PhysX/release/PhysXPvdSDK_static_64.lib")
//#pragma comment(lib, "PhysX/release/PhysXCharacterKinematic_static_64.lib")
//#pragma comment(lib, "PhysX/release/PhysXCooking_static_64.lib")
//#pragma comment(lib, "PhysX/release/PVDRuntime_64.lib")
//#pragma comment(lib, "PhysX/release/PhysX_static_64.lib")
//#pragma comment(lib, "PhysX/release/PhysXCommon_static_64.lib")
//#pragma comment(lib, "PhysX/release/PhysXFoundation_static_64.lib")
#endif

#include "define.h"
#include "struct.h"
#include "func.h"

#include "CSingleton.h"