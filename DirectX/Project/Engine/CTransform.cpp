#include "pch.h"
#include "CTransform.h"

#include "CCollider3D.h"
#include "CDevice.h"
#include "CConstBuffer.h"
#include "CRigidBody.h"
#include "CTimeMgr.h"
#include "physx_util.h"
#include "RaycastMgr.h"

CTransform::CTransform()
	: CComponent(COMPONENT_TYPE::TRANSFORM)
	, m_vRelativeScale(Vec3(1.f, 1.f, 1.f))
	, m_bAbsolute(false)
	, m_bExceptParentRot(false)
	, m_vRelativeRot(Vec3::Zero)
	, m_vRelativePosOffset(Vec3::Zero)
	, m_qRotation(Quat(0.f, 0.f, 0.f, 0.f))
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
	CRigidBody* _rb = GetOwner()->RigidBody();
	CCollider3D* col =GetOwner()->Collider3D();
	
	m_vRelativePos = _vPos;

	if(_rb)
	{
		PxTransform trans = _rb->GetRigidBodyPos();
		trans.p = PxVec3(_vPos.x, _vPos.y, _vPos.z);
		_rb->SetRigidBodyTrans(trans);
	}
	if(col)
	{
		col->SetColliderPos(_vPos);
	}
}

void CTransform::SetRelativeRot(Vec3 _vRot)
{
	CRigidBody* _rb = GetOwner()->RigidBody();
	CCollider3D* col = GetOwner()->Collider3D();

	m_vRelativeRot = _vRot;

	if (_rb)
	{
		if (_rb->IsCreature())
		{
			_vRot.x = 0.f;
		}

		PxTransform trans = _rb->GetRigidBodyPos();
		
		Quat quat; quat = Util::Vector3ToQuaternion(_vRot);
		trans.q = PxQuat(quat.x, quat.y, quat.z, quat.w);
		_rb->SetRigidBodyTrans(trans);
	}
	if (col)
	{
		col->SetColliderRot(_vRot);
	}
}

//void CTransform::AddRelativeRot(Vec3 _vAddRot)
//{
//	CRigidBody* _rb = GetOwner()->RigidBody();
//	CCollider3D* col = GetOwner()->Collider3D();
//
//	Quat quat = Util::Vector3ToQuaternion(_vAddRot);
//	Quat curQuat = Util::Vector3ToQuaternion(m_vRelativeRot);
//	curQuat += quat;
//	m_vRelativeRot = Util::QuaternionToVector3(curQuat);
//	if (_rb)
//	{
//		PxTransform trans = _rb->GetRigidBodyPos();
//		trans.q = PxQuat(curQuat.x, curQuat.y, curQuat.z, curQuat.w);
//		_rb->SetRigidBodyTrans(trans);
//	}
//	if (col)
//	{
//		PxRigidStatic* _rs = GetOwner()->Collider3D()->GetColliderRigid();
//		if (_rs)
//		{
//			PxTransform trans = _rs->getGlobalPose();
//			trans.q = PxQuat(curQuat.x, curQuat.y, curQuat.z, curQuat.w);
//			_rs->setGlobalPose(trans);
//		}
//	}
//}

void CTransform::finaltick()
{
	m_matWorldScale = XMMatrixIdentity();
	m_matWorldScale = XMMatrixScaling(m_vRelativeScale.x, m_vRelativeScale.y, m_vRelativeScale.z);

	Matrix m_Rot = XMMatrixIdentity();
	Matrix m_RotNoX = XMMatrixIdentity();
	Matrix matTranslation = XMMatrixIdentity();

	Vec3 vDefaultDir[3] = {
	  Vec3(1.f, 0.f, 0.f)
	, Vec3(0.f, 1.f, 0.f)
	, Vec3(0.f, 0.f, 1.f)
	};

	CRigidBody* _rb = GetOwner()->RigidBody();
	CCollider3D* _col = GetOwner()->Collider3D();
	// Rigid Body가 있는 경우
	if (_rb && _rb->IsRigidBodyCreate())
	{
		physx::PxTransform trans = _rb->GetRigidBodyPos();

		//if (GetOwner()->Animator3D())
		//	m_vRelativePos = Vec3(trans.p.x, trans.p.y - (m_vRelativeScale.y / 2.f), trans.p.z);
		//else
		//Vec3 curPos = Vec3(trans.p.x, trans.p.y, trans.p.z);
		//m_vRelativePos= XMVectorLerpV(m_vRelativePos, curPos, Vec3(_mt, _mt, _mt));
		Vec3 tmp = Vec3(trans.p.x, trans.p.y, trans.p.z);
		m_vRelativePos = tmp;
		//m_qRotation = Quat(trans.q.x, trans.q.y, trans.q.z, trans.q.w);
		//Quat quat = Util::Vector3ToQuaternion(m_vRelativeRot);
		//m_vRelativeRot = Util::QuaternionToVector3(m_qRotation);
		
		if (!_rb->IsCreature())
		{
			m_qRotation = Quat(trans.q.x, trans.q.y, trans.q.z, trans.q.w);
			m_vRelativeRot = Util::QuaternionToVector3(m_qRotation);
			m_Rot = Matrix::CreateFromQuaternion(m_qRotation);
		}
		else
		{
			m_RotNoX = XMMatrixRotationX(0.f);
			m_RotNoX *= XMMatrixRotationY(m_vRelativeRot.y);
			m_RotNoX *= XMMatrixRotationZ(m_vRelativeRot.z);

			m_Rot = XMMatrixRotationX(m_vRelativeRot.x);
			m_Rot *= XMMatrixRotationY(m_vRelativeRot.y);
			m_Rot *= XMMatrixRotationZ(m_vRelativeRot.z);
			//Quat quat = m_qRotation;
			//quat.x = 0.f;
			//m_qRotation = Quat(trans.q.x, trans.q.y, trans.q.z, trans.q.w);
			//m_vRelativeRot = Util::QuaternionToVector3(m_qRotation);

			//m_RotNoX = Matrix::CreateFromQuaternion(quat);
			//m_Rot = Matrix::CreateFromQuaternion(m_qRotation);
		}
	}
	// Collider3D만 있는 경우
	// Collider만 있다면 중력을 받지 않고
	// 움직일 일이 setPosition으로 밖에는 없음.
	// 예외가 된다면 주석해제
	//else if(_col && _col->IsColliderRigidCreate())
	//{
	//	physx::PxTransform trans = _col->GetColliderPos();
	//	m_vRelativePos = Vec3(trans.p.x, trans.p.y, trans.p.z);
	//	m_qRotation = Quat(trans.q.x, trans.q.y, trans.q.z, trans.q.w);
	//	m_vRelativeRot = Util::QuaternionToVector3(m_qRotation);
	//	m_Rot = Matrix::CreateFromQuaternion(m_qRotation);
	//}
	// 둘다 없는 경우
	else
	{
		Quat quat = Util::Vector3ToQuaternion(m_vRelativeRot);
		m_Rot = Matrix::CreateFromQuaternion(quat);
		//m_Rot = XMMatrixRotationX(m_vRelativeRot.x);
		//m_Rot *= XMMatrixRotationY(m_vRelativeRot.y);
		//m_Rot *= XMMatrixRotationZ(m_vRelativeRot.z);
	}

	Vec3 vFinalPos = m_vRelativePos + m_vRelativePosOffset;
	
	if (GetOwner()->Animator3D()) // 바닥이 있는 Mesh들 바닥에 안닿는거
		vFinalPos.y -= m_vRelativeScale.y / 2.f;

	matTranslation = XMMatrixTranslation(vFinalPos.x, vFinalPos.y, vFinalPos.z);

	//matTranslation = XMMatrixTranslation(m_vRelativePos.x, m_vRelativePos.y, m_vRelativePos.z);

	m_matWorld = m_matWorldScale * m_Rot * matTranslation;
	m_noRotWorld = m_matWorldScale * matTranslation;

	// Raycast Draw용 월드 Matrix
	float dist = RaycastMgr::GetInst()->GetDrawRayDistance();
	Matrix rayScale = XMMatrixScaling(dist, dist, dist);
	m_DrawRayWorld = rayScale * m_Rot * matTranslation;

	for (int i = 0; i < 3; ++i)
	{
		if (_rb && _rb->IsCreature())
			m_vWorldDir[i] = m_vRelativeDir[i] = XMVector3TransformNormal(vDefaultDir[i], m_RotNoX);
		else
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
	fwrite(&m_vRelativePosOffset, sizeof(Vec3), 1, _File);
	fwrite(&m_FollowOffset, sizeof(Vec3), 1, _File);
	fwrite(&m_bAbsolute, sizeof(bool), 1, _File);

}

void CTransform::LoadFromLevelFile(FILE* _FILE)
{	
	fread(&m_vRelativePos, sizeof(Vec3), 1, _FILE);
	fread(&m_vRelativeScale, sizeof(Vec3), 1, _FILE);
	fread(&m_vRelativeRot, sizeof(Vec3), 1, _FILE);
	fread(&m_vRelativePosOffset, sizeof(Vec3), 1, _FILE);
	fread(&m_FollowOffset, sizeof(Vec3), 1, _FILE);
	fread(&m_bAbsolute, sizeof(bool), 1, _FILE);
}
