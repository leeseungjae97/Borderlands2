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
	vector<tRayInfo> m_vecMonsterRayInfo;
	tRayInfo tPlayerRayInfo;
	//bool m_bDoRaycast;
	float m_fDrawRayDistance;

public:
	void tick();

	//bool IsRaycast() { return m_bDoRaycast; }
	//void UseRayInfo();

	void SetDrawRayDistance(float _Distance) { m_fDrawRayDistance = _Distance; }
	void SetPlayerRayInfo(tRayInfo _PlayerRayInfo) { tPlayerRayInfo = _PlayerRayInfo; }

	tRayInfo GetPlayerRayInfo() { return tPlayerRayInfo; }
	vector<tRayInfo> GetRayInfo() { return m_vecMonsterRayInfo; }
	float GetDrawRayDistance() { return m_fDrawRayDistance; }

	void AddRaycast(tRayInfo _RaycastInfo);
	void ClearRayInfo();

public:
	void AddRaycastDraw(Vec3 _vDir, Vec3 _vOrigin, Matrix matWorld, Vec4 _vColor);
	void DoRaycast(tRayInfo _RaycastInfo);
	PxVec3 GetShapeCenter(PxShape* shape, const PxTransform& pose);
	
public:
	RaycastMgr();
	virtual ~RaycastMgr();
};
