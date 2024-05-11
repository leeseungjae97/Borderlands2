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

enum class WARRIOR_ANIMATION_TYPE
{
	ENTER,
	IDLE,
	DIE,
	FAR_BREATH,
	MID_BREATH,
	NEAR_BREATH,
	TAIL_BEAM_ENTER,
	TAIL_BEAM_IDLE,
	TAIL_BEAM_EXIT,
	TAIL_ATTACK,
	ROCK_THROW,
	HIDE,
	APPEAR,
	END,
};
enum class CONSTRUCTOR_ANIMATION_TYPE
{
	BEAM_ENTER,
	BEAM_LOOP,
	BEAM_EXIT,
	SMASH_GROUND,
	GROUND_TO_SKY,
	IDLE,
	DIE,
	HIDE,
	APPEAR,
	END
};
enum class GUN_ANIMATION_TYPE
{
	FIRE,
	RELOAD,
	FAST_RELOAD,
	IDLE,
	DRAW,
	END,
};

enum class ENEMY_ANIMATION_TYPE
{
	WALK_FORWARD,
	WALK_BACK,
	WALK_RIGHT,
	WALK_LEFT,
	SPRINT_FORWARD,

	IDLE,

	FIRE,

	LOOK,
	LOOK_RIGHT,
	LOOK_LEFT,
	LOOK_BACK,

	RELOAD,
	MELEE,

	DIE,
	END,
};

enum class PLAYER_ANIMATION_TYPE
{
	WALK_FORWARD,
	SNIPER_WALK_FORWARD,
	PISTOL_WALK_FORWARD,

	WALK_BACK,
	SNIPER_WALK_BACK,
	PISTOL_WALK_BACK,

	WALK_RIGHT,
	SNIPER_WALK_RIGHT,
	PISTOL_WALK_RIGHT,

	WALK_LEFT,
	SNIPER_WALK_LEFT,
	PISTOL_WALK_LEFT,

	SPRINT_FORWARD,
	SNIPER_SPRINT_FORWARD,
	PISTOL_SPRINT_FORWARD,

	JUMP,
	SNIPER_JUMP,
	PISTOL_JUMP,

	IDLE,
	SNIPER_IDLE,
	PISTOL_IDLE,

	DRAW,
	SNIPER_DRAW,
	PISTOL_DRAW,

	RELOAD,
	SNIPER_RELOAD,
	PISTOL_RELOAD,

	FAST_RELOAD,
	SNIPER_FAST_RELOAD,
	PISTOL_FAST_RELOAD,

	SIGHT_IDLE,
	SNIPER_SIGHT_IDLE,
	PISTOL_SIGHT_IDLE,

	SIGHT_F,
	SNIPER_SIGHT_F,
	PISTOL_SIGHT_F,

	GRENADE_THROW,
	SNIPER_GRENADE_THROW,
	PISTOL_GRENADE_THROW,

	MELEE,
	SNIPER_MELEE,
	PISTOL_MELEE,

	FIRE,
	SNIPER_FIRE,
	PISTOL_FIRE,

	DIE,
	DEAD,

	END,
};
enum class RAYCAST_TYPE
{
	SHOOT,
	LOOK,
	REPLY,
	END,
};

enum class RAYCAST_GROUP_TYPE {
	NoRaycastingCollider = 0x0,
	Enemy = 0x1,
	Player = 0x2,
	DefaultRaycastingCollider = 0x4,
	AllRaycastingCollider = 0xf,
};

enum class COMPONENT_TYPE
{
	TRANSFORM,
	COLLIDER3D,
	RIGIDBODY,
	ANIMATOR3D,
	ANIMATOR2D,
	LIGHT3D,
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

extern const char* PLAYER_ANIMATION_TYPE_STR[(UINT)PLAYER_ANIMATION_TYPE::END + 1];
extern const wchar_t* PLAYER_ANIMATION_TYPE_WSTR[(UINT)PLAYER_ANIMATION_TYPE::END + 1];

extern const char* GUN_ANIMATION_TYPE_STR[(UINT)GUN_ANIMATION_TYPE::END + 1];
extern const wchar_t* GUN_ANIMATION_TYPE_WSTR[(UINT)GUN_ANIMATION_TYPE::END + 1];

extern const char* ENEMY_ANIMATION_TYPE_STR[(UINT)ENEMY_ANIMATION_TYPE::END + 1];
extern const wchar_t* ENEMY_ANIMATION_TYPE_WSTR[(UINT)ENEMY_ANIMATION_TYPE::END + 1];

extern const char* WARRIOR_ANIMATION_TYPE_STR[(UINT)WARRIOR_ANIMATION_TYPE::END + 1];
extern const wchar_t* WARRIOR_ANIMATION_TYPE_WSTR[(UINT)WARRIOR_ANIMATION_TYPE::END + 1];

extern const char* CONSTRUCTOR_ANIMATION_TYPE_STR[(UINT)CONSTRUCTOR_ANIMATION_TYPE::END + 1];
extern const wchar_t* CONSTRUCTOR_ANIMATION_TYPE_WSTR[(UINT)CONSTRUCTOR_ANIMATION_TYPE::END + 1];

enum class CB_TYPE
{
	TRANSFORM,	// b0
	MATERIAL,	// b1
	GLOBAL,		// b2
	PAPER_BURN, // b3
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
	DOMAIN_TEXT,

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
	MESH_FACE,
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
	OBJECT_DIRECTIONAL,
};

enum class LEVEL_STATE
{
	PLAY,
	PAUSE,
	STOP,
	LOAD,
	NO_UPDATE_RENDER,
};

enum class LAYER_TYPE
{
	Camera,
	Default,
	Terrain,
	Environment,
	Enemy,
	Player,
	Item,
	NoRaycastingCollider,
	PlayerBullet,
	EnemyBullet,
	Missile,
	Light,
	ViewPortUI,

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

	END,
};

enum class MRT_TYPE
{
	SWAPCHAIN,

	HDR,

	HDR_LINE,

	BLOOMED_HDR,

	DEFERRED,

	DEFERRED_DECAL,

	LIGHT,

	BLUR_V,

	BLUR_H,

	SCOPE_RENDER,

	SCOPE_DEFERRED_RENDER,

	SCOPE_LIGHT_RENDER,

	SCOPE_SHADOWMAP_RENDER,

	MAP_RENDER,

	MAP_DEFERRED_RENDER,

	MAP_LINE,

	MAP_COLOR,

	SHADOWMAP,

	END,
};