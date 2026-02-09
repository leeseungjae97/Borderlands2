#pragma once



struct tVertex
{
	Vec3 vPos;
	Vec4 vColor;
	Vec2 vUV;

	Vec3 vTangent;
	Vec3 vNormal;
	Vec3 vBinormal;

	Vec4 vWeights;
    Vec4 vIndices;
};

typedef tVertex Vtx;

struct tStructVertex
{
	Vec3 vPos;
	Vec3 vNormal;
	Vec3 vTangent;
	Vec3 vBinormal;
};

// ===========
// Instancing
// ===========
union uInstID
{
	struct
	{
		UINT iMesh;
		WORD iMtrl;
		unsigned char iMtrlIdx;
        unsigned char bAnim;
	};
	ULONG64 llID;
};

class CGameObject;
struct tInstObj
{
	CGameObject* pObj;
	UINT		 iMtrlIdx;
	int			 iAnimInstIdx;
};

struct tInstancingData
{
	Matrix matWorld;
	Matrix matWV;
	Matrix matWVP;
	int    iRowIdx;
};


// Event
struct tEvent
{
	EVENT_TYPE	Type;
	DWORD_PTR	wParam;
	DWORD_PTR	lParam;
};

struct Event {
	Event() {};
	~Event() {};

	void operator=(const std::shared_ptr<std::function<void()>> func)
	{
		mEvent = std::move(func);
	}
	void operator()()
	{
		if (mEvent)
			(*mEvent)();
	}

	std::shared_ptr<std::function<void()>> mEvent;
};

struct Events {
	Events()
		: startEvent{}
		, completeEvent{}
		, progressEvent{}
		, endEvent{}
	{
	};
	~Events() {}
	void SaveToLevelFile(FILE* file)
	{
		fwrite(&startEvent, sizeof(Event), 1, file);
		fwrite(&completeEvent, sizeof(Event), 1, file);
		fwrite(&progressEvent, sizeof(Event), 1, file);
		fwrite(&endEvent, sizeof(Event), 1, file);
	}

	void LoadFromLevelFile(FILE* file)
	{
		fread(&startEvent, sizeof(Event), 1, file);
		fread(&completeEvent, sizeof(Event), 1, file);
		fread(&progressEvent, sizeof(Event), 1, file);
		fread(&endEvent, sizeof(Event), 1, file);
	}
	Event startEvent;
	Event completeEvent;
	Event progressEvent;
	Event endEvent;
};

struct tDebugShapeInfo
{
	SHAPE_TYPE	eShape;
	Matrix		matWorld;
	Vec3		vWorldPos;
	Vec3		vWorldScale;
	Vec3		vWorldRotation;
	Vec3		vDir;
	Vec4		vColor;
	wstring		wsDebugShapeName;
	float		fMaxTime;
	float		fCurTime;
	int			iMtrlCount;
	bool		bDepthTest;
};



struct tLightColor
{
	Vec4 vDiffuse;	// ���� ����
	Vec4 vAmbient;	// �ֺ� ��(ȯ�� ��)
};

// LightInfo
struct tLightInfo
{
	tLightColor Color;		 // ���� ����

	Vec4		vWorldPos;   // ������ ���� �����̽� ��ġ
	Vec4		vWorldDir;	 // ���� ������ ����

	UINT		LightType;   // ���� Ÿ��(���⼺, ��, ����Ʈ)
	float		Radius;		 // ���� �ݰ�(��Ÿ�)
	float		Angle;		 // ���� ����
	int			padding;
};

// CAnimator2D
struct tAnim2DFrm
{
	Vec2	LeftTopUV;
	Vec2	SliceUV;
	Vec2	Offset;
	float	fDuration;
};


// Particle
struct tParticle
{
	Vec4	vLocalPos;		// ������Ʈ�κ��� ������ �Ÿ�
	Vec4	vWorldPos;		// ��ƼŬ ���� ������ġ
	Vec4	vWorldScale;	// ��ƼŬ ũ��	
	Vec4	vColor;			// ��ƼŬ ����
	Vec4	vVelocity;		// ��ƼŬ ���� �ӵ�
	Vec4	vForce;			// ��ƼŬ�� �־��� ��
	Vec4	vRandomForce;	// ��ƼŬ�� ����Ǵ� �������� ��
	Vec4	vObjPos;

	float   Age;			// ���� �ð�
	float   PrevAge;		// ���� ������ ���� �ð�

	float   NomalizedAge;	// ������� �����ð��� 0~1�� ����ȭ �� ��
	float	LifeTime;		// ����

	float	Mass;			// ����
	float   ScaleFactor;	// �߰� ũ�� ����

	int     Active;			// ��ƼŬ Ȱ��ȭ ����
	int     pad;
};

struct tRWParticleBuffer
{	
	int		SpawnCount;			// ���� ��ų ��ƼŬ ����
	int		padding[3];
};


struct tParticleModule
{
	// ���� ���
	Vec4    vSpawnColor;
	Vec4	vSpawnScaleMin;
	Vec4	vSpawnScaleMax;
	Vec3	vBoxShapeScale;	
	float	fSphereShapeRadius;

	int		SpawnShapeType;		// 0 : BOX, 1 : Sphere
	int		SpawnRate;			// �ʴ� ���� ����
	int		Space;				// ��ƼŬ ������Ʈ ��ǥ�� ( 0 : World,  1 : Local)
	float   MinLifeTime;		// �ּ� ����

	float   MaxLifeTime;		// �ִ� ����
	int     spawnpad[3];

	// Color Change ���
	Vec4	vStartColor;		// �ʱ� ����
	Vec4	vEndColor;			// ���� ����

	// Scale Change ���
	float	StartScale;			// �ʱ� ����
	float	EndScale;			// ���� ����	

	// ���� �ִ�ũ��
	int		iMaxParticleCount;
	int		iPerParticleCount;

	// Add Velocity ���
	Vec4	vVelocityDir;

	int     AddVelocityType;	// 0 : From Center, 1: To Center, 2 : Fixed Direction	
	float	OffsetAngle;		
	float	Speed;
	int     addvpad;

	// Drag ��� - �ӵ� ����
	float	StartDrag;
	float	EndDrag;

	// NoiseForce ��� - ���� �� ����	
	float	fNoiseTerm;		// ���� �� ���� ����
	float	fNoiseForce;	// ���� �� ũ��

	// Render ���
	int		VelocityAlignment;	// 1 : �ӵ����� ���(�̵� �������� ȸ��) 0 : ��� ����
	int		VelocityScale;		// 1 : �ӵ��� ���� ũ�� ��ȭ ���, 0 : ��� ����	
	float   vMaxSpeed;			// �ִ� ũ�⿡ �����ϴ� �ӷ�
	Vec4	vMaxVelocityScale;	// �ӷ¿� ���� ũ�� ��ȭ�� �ִ�ġ
	int		renderpad;

	// Module Check
	int		ModuleCheck[(UINT)PARTICLE_MODULE::END];
};

// ============
// Animation 3D
// ============
struct tFrameTrans
{
	Vec4	vTranslate;
	Vec4	vScale;
	Vec4	qRot;
};

struct tMTKeyFrame
{
	double	dTime;
	int		iFrame;
	Vec3	vTranslate;
	Vec3	vScale;
	Vec4	qRot;
};


struct tMTBone
{
	wstring				strBoneName;
	int					iDepth;
	int					iParentIndx;
	Matrix				matOffset;
	Matrix				matBone;
	Vec4				vBonePos;
	map<wstring ,vector<tMTKeyFrame>>	vecKeyFrame;
};

struct tMTAnimClip
{
	wstring			strAnimName;
	int				iStartFrame;
	int				iEndFrame;
	int				iFrameLength;
	int				iFrameCount;

	double			dStartTime;
	double			dEndTime;
	double			dTimeLength;
	float			fUpdateTime;

	vector<tFrameTrans>vecTransKeyFrame;

	FbxTime::EMode	eMode;
};

// ===================
// ������� ���� ����ü
// ===================
struct tTransform
{
	Matrix matWorld;
	Matrix matWorldInv;
	Matrix matView;
	Matrix matViewInv;
	Matrix matProj;
	Matrix matProjInv;

	Matrix matWV;
	Matrix matWVP;
};

extern tTransform g_transform;

struct tMtrlData
{
	Vec4 vDiff;
	Vec4 vSpec;
	Vec4 vAmb;
	Vec4 vEmv;
};


struct tMtrlConst
{
	tMtrlData mtrl;

	int arrInt[8];
	float arrFloat[8];
	Vec2 arrV2[4];
	Vec4 arrV4[4];
	Matrix arrMat[4];

	int arrTex[(UINT)TEX_PARAM::TEX_END];

	int	arrAnimData[3];
	float fTexFlowSpeed;

	Vec2 vTexDir;
	float fEmisCoeff;
	
	int arrTexFlow[(UINT)TEX_PARAM::TEX_END];
	int arrTexEmis[(UINT)TEX_PARAM::TEX_END];

	int pad;
};

struct tPaperBurnConst
{
	int paperBurn;
	float paperAcc;
	float paperTime;
	
	Vec2 vMtrlSize;
	int paperBurnEnd;
	float pad[2];
};

struct tGlobal
{
	Vec2  Resolution;
	float tDT;
	float tAccTime;
	UINT  Light2DCount;
	UINT  Light3DCount;
	int	  globalpadding[2];
};

extern tGlobal GlobalData;

// ���� ����ü
struct tRay
{
	// ���� : ���� Relative Position���� Ȯ���ϰ� �����.
	Vec3 vStart;
	Vec3 vDir;
};

// Raycast ����� ���� ����ü
struct tRaycastOut
{
	Vec2	vUV;
	float	fDist;
	int		bSuccess;
	Vec3	vCrossPos;
	float	pad;
};

struct tNode
{
	int		iCoordX;
	int		iCoordY;
	Vec3	vPos;
	Vec3	vLandPos;
};

struct tRayInfo
{
	int		iLayerIdx;
	float	fDamage;
	float	fDist;
	Vec3	vStart;
	Vec3	vDir;
	Matrix  matWorld;
	UINT	tRayType;
	UINT	iID;
};