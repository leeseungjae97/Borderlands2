#pragma once

#define DEVICE  CDevice::GetInst()->GetDevice()
#define CONTEXT CDevice::GetInst()->GetDeviceContext()

#define CLONE(type) public: virtual type* Clone() { return new type(*this); }
#define CLONE_DISABLE(type) public: virtual type* Clone() { return nullptr; assert(nullptr); }

#define KEY_TAP(Key) CKeyMgr::GetInst()->GetKeyState(Key) == KEY_STATE::TAP		
#define KEY_RELEASE(Key) CKeyMgr::GetInst()->GetKeyState(Key) == KEY_STATE::RELEASE
#define KEY_PRESSED(Key) CKeyMgr::GetInst()->GetKeyState(Key) == KEY_STATE::PRESSED

#define DT CTimeMgr::GetInst()->GetDeltaTime()

#define MAX_LAYER	32
#define MAX_MIP		8

#define SINGLE(type) private: type(); ~type(); friend class CSingleton<type>;

#define SMG_IDX 0
#define SNIPER_IDX 1
#define PISTOL_IDX 2

enum class GUN_ANIMATION_TYPE
{
	FIRE,
	RELOAD,
	FAST_RELOAD,
	IDLE,
	END,
};

enum class ANIMATION_TYPE
{
	WALK_FORWARD,
	WALK_BACK,
	WALK_RIGHT,
	WALK_LEFT,
	RUN_FORWARD,
	JUMP,
	IDLE,
	DRAW,
	RELOAD,
	FAST_RELOAD,
	MELEE,
	ATTACK1,
	ATTACK2,
	ATTACK3,
	ATTACK4,
	DIE,
	DEAD,
	EXTRA1,
	EXTRA2,
	EXTRA3,
	EXTRA4,
	END,
};
enum class RAYCAST_TYPE
{
	SHOOT,
	LOOK,
	END,
};
enum class COMPONENT_TYPE
{
	TRANSFORM,
	COLLIDER3D,
	RIGIDBODY,
	ANIMATOR3D,
	ANIMATOR2D,
	LIGHT3D,
	PATHFIND,
	GIZMO,
	CAMERA,

	MESHRENDER,
	PARTICLESYSTEM,
	LANDSCAPE,
	SKYBOX,
	DECAL,
		
	END,

	SCRIPT,
};

extern const char* COMPONENT_TYPE_STR[(UINT)COMPONENT_TYPE::END];
extern const wchar_t* COMPONENT_TYPE_WSTR[(UINT)COMPONENT_TYPE::END];


enum class RES_TYPE
{
	MESHDATA,
	MATERIAL,
	PREFAB,

	MESH,			
	TEXTURE,		
	SOUND,

	GRAPHICS_SHADER,
	COMPUTE_SHADER,

	END,
};

extern const char* RES_TYPE_STR[(UINT)RES_TYPE::END];
extern const wchar_t* RES_TYPE_WSTR[(UINT)RES_TYPE::END];

extern const char* ANIMATION_TYPE_STR[(UINT)ANIMATION_TYPE::END + 1];
extern const wchar_t* ANIMATION_TYPE_WSTR[(UINT)ANIMATION_TYPE::END + 1];

extern const char* GUN_ANIMATION_TYPE_STR[(UINT)GUN_ANIMATION_TYPE::END + 1];
extern const wchar_t* GUN_ANIMATION_TYPE_WSTR[(UINT)GUN_ANIMATION_TYPE::END + 1];

enum class CB_TYPE
{
	TRANSFORM,	// b0
	MATERIAL,	// b1
	GLOBAL,		// b2
	END,
};


enum SCALAR_PARAM
{
	INT_0,
	INT_1,
	INT_2,
	INT_3,

	INT_4,
	INT_5,
	INT_6,
	INT_7,

	FLOAT_0,
	FLOAT_1,
	FLOAT_2,
	FLOAT_3,

	FLOAT_4,
	FLOAT_5,
	FLOAT_6,
	FLOAT_7,

	VEC2_0,
	VEC2_1,
	VEC2_2,
	VEC2_3,

	//VEC3_0,
	//VEC3_1,
	//VEC3_2,
	//VEC3_3,

	VEC4_0,
	VEC4_1,
	VEC4_2,
	VEC4_3,

	MAT_0,
	MAT_1,
	MAT_2,
	MAT_3,	
};

enum TEX_PARAM
{
	TEX_0,
	TEX_1,
	TEX_2,
	TEX_3,
	TEX_4,
	TEX_5,
	TEX_6,
	TEX_7,

	TEX_CUBE_0,
	TEX_CUBE_1,

	TEX_ARR_0,
	TEX_ARR_1,

	TEX_END,
};

enum PIPELINE_STAGE
{
	PS_VERTEX = 0x01,
	PS_HULL = 0x02,
	PS_DOMAIN = 0x04,
	PS_GEOMETRY = 0x08,
	PS_PIXEL = 0x10,	

	PS_ALL = PS_VERTEX | PS_HULL | PS_DOMAIN | PS_GEOMETRY | PS_PIXEL,	
};

enum class RS_TYPE
{
	CULL_BACK,
	CULL_FRONT,
	CULL_NONE,
	WIRE_FRAME,
	END,
};

enum class DS_TYPE
{
	LESS,
	LESS_EQUAL,
	GREATER,
	GREATER_EQUAL,
	NO_WRITE,			// LESS, DepthWrite X
	NO_TEST_NO_WRITE,	// Test X, DepthWrite X
	NO_TEST,
	STENCIL_CULL_TEST_T,
	STENCIL_CULL_TEST_O,
	STENCIL_CULL_DEPLOY,
	END,
};


enum class BS_TYPE
{
	DEFAULT,		// No Blending
	MASK,			// Alpha Coverage
	ALPHA_BLEND,	// Alpha 계수 
	ONE_ONE,		// 1:1 혼합
	DEFERRED_DECAL_BLEND,
	END,
};





enum class DIR_TYPE
{
	RIGHT,
	UP,
	FRONT,	
};

enum class PROJ_TYPE
{
	ORTHOGRAPHIC,
	PERSPECTIVE,
};

enum class SHADER_DOMAIN
{
	DOMAIN_DEFERRED,
	DOMAIN_DEFERRED_DECAL,

	DOMAIN_OPAQUE,		// 불투명 오브젝트
	DOMAIN_MASK,		// 불투명, 완전 투명
	DOMAIN_DECAL,		// 데칼
	DOMAIN_TRANSPARENT,	// 반투명
	DOMAIN_POSTPROCESS, // 후 처리
	DOMAIN_UI,

	DOMAIN_LIGHT,
	DOMAIN_UNDEFINED,
};


enum class EVENT_TYPE
{
	CREATE_OBJECT,	// wParam : GameObject, lParam : Layer Index
	DELETE_OBJECT,  // wParam : GameObject

	ADD_CHILD,

	DELETE_RESOURCE,	// wParam : RES_TYPE, lParam : Resource Adress

	LEVEL_CHANGE,
	LEVEL_RESET,
	LEVEL_RECOG,
	LEVEL_LOAD,
};


enum class SHAPE_TYPE
{
	RECT,
	CIRCLE,
	CUBE,
	SPHERE,
	UP_LINE,
	FRONT_LINE,
	RIGHT_LINE,
	MESH,
	END,
};


enum class COLLIDER2D_TYPE
{
	RECT,
	CIRCLE,
};


enum class LIGHT_TYPE
{
	DIRECTIONAL,
	POINT,
	SPOT,
};

enum class LEVEL_STATE
{
	PLAY,
	PAUSE,
	STOP,
	LOAD,
	NO_UPDATE_RENDER,
};

enum class SB_TYPE
{
	READ_ONLY,
	READ_WRITE,
};

enum class PARTICLE_MODULE
{
	PARTICLE_SPAWN,
	COLOR_CHANGE,
	SCALE_CHANGE,
	ADD_VELOCITY,

	DRAG,
	NOISE_FORCE,
	RENDER,
	DUMMY_3,

	USE_MOUSE,
	DUMMY1,
	DUMMY2,
	DUMMY3,

	END,
};

enum class MRT_TYPE
{
	SWAPCHAIN,

	DEFERRED,

	DEFERRED_DECAL,

	LIGHT,

	SHADOWMAP,

	STENCIL_CULL,

	STENCIL_CULL_DEPLOY,

	END,
};