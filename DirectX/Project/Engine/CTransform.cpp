#include "pch.h"
#include "CTransform.h"

#include "CCollider3D.h"
#include "CDevice.h"
#include "CConstBuffer.h"
#include "CRigidBody.h"
#include "physx_util.h"

CTransform::CTransform()
	: CComponent(COMPONENT_TYPE::TRANSFORM)
	, m_vRelativeScale(Vec3(1.f, 1.f, 1.f))
	, m_bAbsolute(false)
	, m_bExceptParentRot(false)
	, m_vRelativeDir{
		  Vec3(1.f, 0.f, 0.f)
		, Vec3(0.f, 1.f, 0.f)
		, Vec3(0.f, 0.f, 1.f)}	
{
	SetName(L"Transform");
}

CTransform::~CTransform()
{
}

void CTransform::SetRelativePos(Vec3 _vPos)
{
	CRigidBody* _db = GetOwner()->RigidBody();

	m_vRelativePos = _vPos;

	if(_db)
	{
		PxTransform trans = _db->GetRigidBodyPos();
		trans.p = PxVec3(_vPos.x, _vPos.y, _vPos.z);
		_db->SetRigidBodyTrans(trans);
	}
}

void CTransform::SetRelativeRot(Vec3 _vRot)
{
	CRigidBody* _db = GetOwner()->RigidBody();
	
	m_vRelativeRot = _vRot;

	if (_db)
	{
		PxTransform trans = _db->GetRigidBodyPos();
		if (_db->IsCreature())
		{
			m_vCreatureRelativeRotX = _vRot.x;
			//_vRot.x = 0.f;
		}

		Quat quat; quat = Util::Vector3ToQuaternion(_vRot);
		trans.q = PxQuat(quat.x, quat.y, quat.z, quat.w);
		_db->SetRigidBodyTrans(trans);
	}
}

void CTransform::finaltick()
{
	m_matWorldScale = XMMatrixIdentity();
	m_matWorldScale = XMMatrixScaling(m_vRelativeScale.x, m_vRelativeScale.y, m_vRelativeScale.z);

	Matrix m_Rot = XMMatrixIdentity();
	Matrix matTranslation = XMMatrixIdentity();

	Vec3 vDefaultDir[3] = {
	  Vec3(1.f, 0.f, 0.f)
	, Vec3(0.f, 1.f, 0.f)
	, Vec3(0.f, 0.f, 1.f)
	};

	CRigidBody* _db = GetOwner()->RigidBody();
	
	if (nullptr != _db)
	{
		physx::PxTransform trans = _db->GetRigidBodyPos();
		m_vRelativePos = Vec3(trans.p.x, trans.p.y, trans.p.z);

		m_qRotation = Quat(trans.q.x, trans.q.y, trans.q.z, trans.q.w);
		m_vRelativeRot = Util::QuaternionToVector3(m_qRotation);
		m_Rot = Matrix::CreateFromQuaternion(m_qRotation);

		//if(!_db->IsCreature())
		//{
		//	m_qRotation = Quat(trans.q.x, trans.q.y, trans.q.z, trans.q.w);
		//	m_vRelativeRot = Util::QuaternionToVector3(m_qRotation);
		//	m_Rot = Matrix::CreateFromQuaternion(m_qRotation);
		//}
		//else
		//{
		//	m_Rot = XMMatrixRotationX(m_vRelativeRot.x);
		//	m_Rot *= XMMatrixRotationY(m_vRelativeRot.y);
		//	m_Rot *= XMMatrixRotationZ(m_vRelativeRot.z);
		//}
	}
	else
	{
		//m_vRelativeRot
		m_Rot = XMMatrixRotationX(m_vRelativeRot.x);
		m_Rot *= XMMatrixRotationY(m_vRelativeRot.y);
		m_Rot *= XMMatrixRotationZ(m_vRelativeRot.z);
	}

	matTranslation = XMMatrixTranslation(m_vRelativePos.x, m_vRelativePos.y, m_vRelativePos.z);

	m_matWorld = m_matWorldScale * m_Rot * matTranslation;
	m_noRotWorld = m_matWorldScale * matTranslation;

	for (int i = 0; i < 3; ++i)
	{
		m_vWorldDir[i] = m_vRelativeDir[i] = XMVector3TransformNormal(vDefaultDir[i], m_Rot);
	}

	CGameObject* pParent = GetOwner()->GetParent();
	if (pParent)
	{
		if (m_bAbsolute)
		{
			Matrix matParentWorld = pParent->Transform()->m_matWorld;
			Matrix matParentScale = pParent->Transform()->m_matWorldScale;
			Matrix matParentScaleInv = XMMatrixInverse(nullptr, matParentScale);

			m_matWorld = m_matWorld * matParentScaleInv * matParentWorld;
		}
		else
		{
			m_matWorldScale = pParent->Transform()->m_matWorldScale;
			m_matWorld *= pParent->Transform()->m_matWorld;
		}


		for (int i = 0; i < 3; ++i)
		{
			m_vWorldDir[i] = XMVector3TransformNormal(vDefaultDir[i], m_matWorld);
			m_vWorldDir[i].Normalize();
		}
	}
	//if (nullptr != GetOwner()->GetFollowObj())
	//{
	//	Vec3 vFollowPos = GetOwner()->GetFollowObj()->Transform()->GetRelativePos();
	//	vFollowPos += m_FollowOffset;
	//	m_vRelativePos = vFollowPos;
	//}

	m_matWorldInv = XMMatrixInverse(nullptr, m_matWorld);
}

void CTransform::UpdateData()
{
	CConstBuffer* pTransformBuffer = CDevice::GetInst()->GetConstBuffer(CB_TYPE::TRANSFORM);

	g_transform.matWorld = m_matWorld;
	g_transform.matWorldInv = m_matWorldInv;
	g_transform.matWV = g_transform.matWorld * g_transform.matView;
	g_transform.matWVP = g_transform.matWV * g_transform.matProj;


	pTransformBuffer->SetData(&g_transform);
	pTransformBuffer->UpdateData();
}

void CTransform::SaveToLevelFile(FILE* _File)
{
	fwrite(&m_vRelativePos	, sizeof(Vec3), 1, _File);
	fwrite(&m_vRelativeScale, sizeof(Vec3), 1, _File);
	fwrite(&m_vRelativeRot	, sizeof(Vec3), 1, _File);
	fwrite(&m_FollowOffset, sizeof(Vec3), 1, _File);
	fwrite(&m_bAbsolute, sizeof(bool), 1, _File);	    
}

void CTransform::LoadFromLevelFile(FILE* _FILE)
{	
	fread(&m_vRelativePos, sizeof(Vec3), 1, _FILE);
	fread(&m_vRelativeScale, sizeof(Vec3), 1, _FILE);
	fread(&m_vRelativeRot, sizeof(Vec3), 1, _FILE);
	fread(&m_FollowOffset, sizeof(Vec3), 1, _FILE);
	fread(&m_bAbsolute, sizeof(bool), 1, _FILE);
}
