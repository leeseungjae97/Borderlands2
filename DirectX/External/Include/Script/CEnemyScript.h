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

	Ptr<CSound> m_pVoice;

	float fMoveSoundAcc;

	wstring wsEnemyWalkSound[5] =
	{
		L"sound\\walk\\player_walk_1.ogg",
		L"sound\\walk\\player_walk_2.ogg",
		L"sound\\walk\\player_walk_3.ogg",
		L"sound\\walk\\player_walk_4.ogg",
		L"sound\\walk\\player_walk_5.ogg",
	};


	wstring wsPsychoAggroSound[3] =
	{
		L"sound\\psycho\\VOBD_Psycho1_HUM_Start_Aggro_01.ogg",
		L"sound\\psycho\\VOBD_Psycho1_HUM_Start_Aggro_02.ogg",
		L"sound\\psycho\\VOBD_Psycho1_HUM_Start_Aggro_04.ogg",
	};

	wstring wsPsychoMeleeSound[3] =
	{
		L"sound\\psycho\\VOBD_Psycho1_GEN_Start_Melee_01.ogg",
		L"sound\\psycho\\VOBD_Psycho1_GEN_Start_Melee_02.ogg",
		L"sound\\psycho\\VOBD_Psycho1_GEN_Start_Melee_03.ogg",
	};

	wstring wsPsychoDeathSound[4] =
	{
		L"sound\\psycho\\VOBD_Psycho1_GEN_React_Death_01.ogg",
		L"sound\\psycho\\VOBD_Psycho1_GEN_React_Death_02.ogg",
		L"sound\\psycho\\VOBD_Psycho1_GEN_React_Death_03.ogg",
		L"sound\\psycho\\VOBD_Psycho1_GEN_React_Death_04.ogg",
	};

	wstring wsNomadAggroSound[3] =
	{
		L"sound\\nomad\\VOBD_Nomad_HUM_Start_Aggro_01.ogg",
		L"sound\\nomad\\VOBD_Nomad_HUM_Start_Aggro_02.ogg",
		L"sound\\nomad\\VOBD_Nomad_HUM_Start_Aggro_03.ogg",
	};
	wstring wsNomadDeathSound[4] =
	{
		L"sound\\nomad\\VOBD_Nomad_GEN_React_Death_01.ogg",
		L"sound\\nomad\\VOBD_Nomad_GEN_React_Death_02.ogg",
		L"sound\\nomad\\VOBD_Nomad_GEN_React_Death_02.ogg",
		L"sound\\nomad\\VOBD_Nomad_GEN_React_Death_02.ogg",
	};

	wstring wsLoaderAggroSound[3] =
	{
		L"sound\\gun_loader\\VOBD_Loader_HUM_Start_Aggro_01.ogg",
		L"sound\\gun_loader\\VOBD_Loader_HUM_Start_Aggro_02.ogg",
		L"sound\\gun_loader\\VOBD_Loader_HUM_Start_Aggro_03.ogg",
	};
	wstring wsLoaderDeathSound[4] =
	{
		L"sound\\gun_loader\\VOBD_Loader_GEN_React_Death_01.ogg",
		L"sound\\gun_loader\\VOBD_Loader_GEN_React_Death_02.ogg",
		L"sound\\gun_loader\\VOBD_Loader_GEN_React_Death_03.ogg",
		L"sound\\gun_loader\\VOBD_Loader_GEN_React_Death_04.ogg",
	};


	wstring* aggroSound;
	wstring* deathSound;

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
	bool Look();
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


