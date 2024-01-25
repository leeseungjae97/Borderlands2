#include "pch.h"
#include "LandScapeMgr.h"

#include "CLandScape.h"
#include "CTransform.h"

LandScapeMgr::LandScapeMgr()
{
}

LandScapeMgr::~LandScapeMgr()
{
}

void LandScapeMgr::ClearLand()
{
	m_vecLandScape.clear();
}

void LandScapeMgr::DeleteLandScape(CLandScape* _LS)
{
	//auto iter = m_vecLandScape.begin();
	//while(iter != m_vecLandScape.end())
	//{
	//	if (iter == _LS)
	//	{
	//		iter = m_vecLandScape.erase(iter);
	//		//return;
	//	}
	//	else
	//	{
	//		++iter;
	//	}
	//}
}

Vec3 LandScapeMgr::GetLandScapeCrossPos()
{
	for(CLandScape* _LS : m_vecLandScape)
	{
		tRaycastOut ray = _LS->GetLandScapeRayCastOut();
		if (ray.bSuccess)
		{
			return XMVector3TransformCoord(ray.vCrossPos, _LS->Transform()->GetWorldMat());
		}	
	}

	return Vec3(0, 0, 0);
}


