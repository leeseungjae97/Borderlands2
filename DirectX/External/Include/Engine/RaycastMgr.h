#pragma once

using namespace physx;

struct tDrawRaycast
{
	Vec3 vOrign;
	Vec3 vDir;
	Vec4 vColor;
	Matrix matWorld;
};
class CCollider3D;
class RaycastMgr
	: public CSingleton<RaycastMgr>
{
private:
	vector<tDrawRaycast> m_vecRayDraw;
	vector<tRayInfo> m_vecEnemyRayInfo;
	vector<tRayInfo> m_vecReplyRayInfo;
	tRayInfo tPlayerRayInfo;
	//bool m_bDoRaycast;
	float m_fDrawRayDistance;

public:
	void tick();

	void SetDrawRayDistance(float _Distance) { m_fDrawRayDistance = _Distance; }
	void SetPlayerRayInfo(tRayInfo _PlayerRayInfo) { tPlayerRayInfo = _PlayerRayInfo; }

	tRayInfo GetPlayerRayInfo() { return tPlayerRayInfo; }
	vector<tRayInfo> GetRayInfo() { return m_vecEnemyRayInfo; }
	float GetDrawRayDistance() { return m_fDrawRayDistance; }

	void AddRaycast(tRayInfo _RaycastInfo);
	void ReplyRaycast(tRayInfo _RaycastInfo);
	void ClearRayInfo();

public:
	void AddRaycastDraw(Vec3 _vDir, Vec3 _vOrigin, Matrix matWorld, Vec4 _vColor);
	bool DoOneHitRaycast(tRayInfo _RaycastInfo, RAYCAST_GROUP_TYPE _Except);
	bool DoOneHitRaycast(tRayInfo _RaycastInfo, Vec3* HitPosition, RAYCAST_GROUP_TYPE _Except);
	bool DoOneHitRaycast(tRayInfo _RaycastInfo, CGameObject** _Hover, RAYCAST_GROUP_TYPE _Except);

	bool DoAllHitRaycast(tRayInfo _RaycastInfo, Vec3* HitPosition);
	bool DoAllHitRaycast(tRayInfo _RaycastInfo);
	PxVec3 GetShapeCenter(PxShape* shape, const PxTransform& pose);
	
public:
	RaycastMgr();
	virtual ~RaycastMgr();
};

