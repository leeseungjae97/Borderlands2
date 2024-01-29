#include "pch.h"
#include "CBulletScript.h"
CBulletScript::CBulletScript()
	: CScript((UINT)SCRIPT_TYPE::BULLETSCRIPT)
{

}

CBulletScript::~CBulletScript()
{

}

void CBulletScript::BeginOverlap(CCollider3D* _Other)
{
	CScript::BeginOverlap(_Other);
}

void CBulletScript::tick()
{

	//const PxU32 maxHits = 1;
	//const PxHitFlags hitFlags = PxHitFlag::ePOSITION | PxHitFlag::eNORMAL | PxHitFlag::eUV;
	//float d2 =PxGeometryQuery::raycast(startPos, rayDir, 0, );
}
