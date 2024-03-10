#pragma once
#include <Engine\CScript.h>

class CEnemyScript :
	public CScript {
public:
	enum class ENEMY_TYPE {
		NOMAD,
		PSYCHO,
		GUN_LOADER,
		END,
	};
	wstring ENEMY_NAME[(UINT)ENEMY_TYPE::END]{
		L"Nomad",
		L"Psycho",
		L"Gun Loader",
	};
	bool ENEMY_WEAPON_RIGHT_HAND[(UINT)ENEMY_TYPE::END] = {
		true,
		true,
		true,
	};
	bool ENEMY_RANGE[(UINT)ENEMY_TYPE::END] = {
		true,
		false,
		true
	};
	float ENEMY_LOOK_DIST[(UINT)ENEMY_TYPE::END] = {
		5000.f,
		5000.f,
		5000.f,
	};
	float ENEMY_ATTACK_RANGE[(UINT)ENEMY_TYPE::END] = {
		5000.f,
		0.f,
		5000.f,
	};
	enum class ENEMY_STATE {
		RELOAD,
		FIRE,
		WALK,
		MELEE,
		DIE,
		IDLE,
	};

private:
	float fEnemySpeed;
	float fRotateSpeed;
	float fRateOfFire;
	float fRateOfFireAcc;
	int   iEnemyHp;
	int   iEnemyHpCapa;
	int   iAmmo;
	bool  bRotate;

	CGameObject* pHeadCollider;

	ENEMY_STATE tState;
	ENEMY_TYPE tType;

	vector<Vec3> m_vecQueryPath;

	Vec3 vDestPos;
	Vec3 vPrevPos;
	Vec3 vPrevPlayerPos;
	float fDestDist;
private:
	void Move();
	bool Rotate();
	bool RotateDest();
	void Reload();

	void makeCollider();
	void moveCollider();
	void doPathQuery();
public:
	void begin() override;

	virtual void tick() override;
	void finaltick() override;
	virtual void CatchRaycast() override;
	void BeginOverlap(CCollider3D* _Other) override;
	void Raycast(tRayInfo _RaycastInfo) override;
	void SaveToLevelFile(FILE* _File) override;
	void LoadFromLevelFile(FILE* _FILE) override;
	void Attacked(float fDamage);
	void Shoot();
	bool Melee();
	void Look();
	bool IsDetect();

	void PaperBurn();
	void IsDie();

	wstring GetEnemyName() { return ENEMY_NAME[(UINT)tType]; }
	bool GetEnemyWeaponHand() { return ENEMY_WEAPON_RIGHT_HAND[(UINT)tType]; }

public:
	void SetRotate(bool _Rotate) { bRotate = _Rotate; }
	void SetRotateSpeed(float _Speed) { fRotateSpeed = _Speed; }
	void SetSpeed(float _Speed) { fEnemySpeed = _Speed; }
	void SetMonsterHp(int _Hp) { iEnemyHp = _Hp; }
	void SetRateOfFire(float _ROF) { fRateOfFire = _ROF; }

	int GetHp() { return iEnemyHp; }
	int GetHpCapacity() { return iEnemyHpCapa; }

	float GetHpRatio() { return (float)iEnemyHp / (float)iEnemyHpCapa; }

	CLONE(CEnemyScript);
public:
	CEnemyScript(ENEMY_TYPE _Type);
	CEnemyScript();
	~CEnemyScript();
};


