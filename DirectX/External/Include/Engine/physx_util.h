#pragma once

#ifndef PHYSX_UTIL_H
#define PHYSX_UTIL_H

// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//  * Neither the name of NVIDIA CORPORATION nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
// OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Copyright (c) 2008-2023 NVIDIA Corporation. All rights reserved.
#include "pch.h"

#include "foundation/Px.h"
#include "foundation/PxSimpleTypes.h"

#include "PhysXMgr.h"
#include "CCollider3D.h"
#include "CollisionMgr.h"

namespace physx
{
	namespace Util
	{
		class BRandom
		{
		public:
			BRandom(PxU32 seed = 0) : mRnd(seed) {}
			~BRandom() {}

			PX_FORCE_INLINE	void		setSeed(PxU32 seed) { mRnd = seed; }
			PX_FORCE_INLINE	PxU32		getCurrentValue()	const { return mRnd; }
			PxU32		randomize() { mRnd = mRnd * 2147001325 + 715136305; return mRnd; }

			PX_FORCE_INLINE	PxU32		rand() { return randomize() & 0xffff; }
			PX_FORCE_INLINE	PxU32		rand32() { return randomize() & 0xffffffff; }

			PxF32		rand(PxF32 a, PxF32 b)
			{
				const PxF32 r = rand32() / (static_cast<PxF32>(0xffffffff));
				return r * (b - a) + a;
			}

			PxI32		rand(PxI32 a, PxI32 b)
			{
				return a + static_cast<PxI32>(rand32() % (b - a));
			}

			PxF32		randomFloat()
			{
				return rand() / (static_cast<PxF32>(0xffff)) - 0.5f;
			}
			PxF32		randomFloat32()
			{
				return rand32() / (static_cast<PxF32>(0xffffffff)) - 0.5f;
			}

			PxF32		randomFloat32(PxReal a, PxReal b) { return rand32() / PxF32(0xffffffff) * (b - a) + a; }
			void		unitRandomPt(physx::PxVec3& v);
			void		unitRandomQuat(physx::PxQuat& v);

			PxVec3		unitRandomPt();
			PxQuat		unitRandomQuat();
		private:
			PxU32		mRnd;
		};
		/////

		/* Increment the specified location. Return the incremented value. */
		PxI32 atomicIncrement(volatile PxI32* val);

		/* Decrement the specified location. Return the decremented value. */
		PxI32 atomicDecrement(volatile PxI32* val);

		//******************************************************************************//

		/* Return the number of physical cores (does not include hyper-threaded cores), returns 0 on failure. */
		PxU32 getNbPhysicalCores();

		//******************************************************************************//

		/* Return the id of a thread. */
		PxU32 getThreadId();

		//******************************************************************************//

		/* Return the current time */
		PxU64 getCurrentTimeCounterValue();

		/* Convert to milliseconds an elapsed time computed from the difference of the times returned from two calls to getCurrentTimeCounterValue. */
		PxReal getElapsedTimeInMilliseconds(const PxU64 elapsedTime);

		/* Convert to microseconds an elapsed time computed from the difference of the times returned from two calls to getCurrentTimeCounterValue. */
		PxReal getElapsedTimeInMicroSeconds(const PxU64 elapsedTime);

		//******************************************************************************//

		struct Sync;

		/* Create a sync object. Returns a unique handle to the sync object so that it may be addressed through syncWait etc. */
		Sync* syncCreate();

		/* Wait indefinitely until the specified sync object is signaled. */
		void syncWait(Sync* sync);

		/* Signal the specified synchronization object, waking all threads waiting on it. */
		void syncSet(Sync* sync);

		/** Reset the specified synchronization object. */
		void syncReset(Sync* sync);

		/* Release the specified sync object so that it may be reused with syncCreate. */
		void syncRelease(Sync* sync);

		//******************************************************************************//

		struct Thread;

		/* Prototype of callback passed to threadCreate. */
		typedef void (*ThreadEntryPoint)(void*);

		/* Create a thread object and return a unique handle to the thread object so that it may be addressed through threadStart etc.
		entryPoint implements ThreadEntryPoint and data will be passed as a function argument, POSIX-style. */
		Thread* threadCreate(ThreadEntryPoint entryPoint, void* data);

		/* Cleanly shut down the specified thread. Called in the context of the spawned thread. */
		void threadQuit(Thread* thread);

		/* Stop the specified thread. Signals the spawned thread that it should stop, so the
		thread should check regularly. */
		void threadSignalQuit(Thread* thread);

		/* Wait for the specified thread to stop. Should be called in the context of the spawning
		thread. Returns false if the thread has not been started.*/
		bool threadWaitForQuit(Thread* thread);

		/* Check whether the thread is signalled to quit. Called in the context of the
		spawned thread. */
		bool threadQuitIsSignalled(Thread* thread);

		/* Release the specified thread object so that it may be reused with threadCreate. */
		void threadRelease(Thread* thread);

		//******************************************************************************//

		struct Mutex;

		/* Create a mutex object and return a unique handle to the mutex object so that it may be addressed through mutexLock etc. */
		Mutex* mutexCreate();

		/* Acquire (lock) the specified mutex. If the mutex is already locked by another thread, this method blocks until the mutex is unlocked.*/
		void mutexLock(Mutex* mutex);

		/* Release (unlock) the specified mutex, the calling thread must have previously called lock() or method will error. */
		void mutexUnlock(Mutex* mutex);

		/* Release the specified mutex so that it may be reused with mutexCreate. */
		void mutexRelease(Mutex* mutex);

		enum PxTrigger
		{
			// Uses built-in triggers (PxShapeFlag::eTRIGGER_SHAPE).
			REAL_TRIGGERS,

			// Emulates triggers using a filter shader. Needs one reserved value in PxFilterData.
			FILTER_SHADER,
		};

		static bool isTrigger(const physx::PxFilterData& data)
		{
			//if (data.word0 == 0x0000000f)	 
			//	return true;
			if (data.word1 == 0x0000000f)
				return true;
			//if (data.word2 == 0x0000000f)
			//	return true;
			//if (data.word3 == 0x0000000f)
			//	return true;

			return false;

		}
		static bool isTriggerShape(physx::PxShape* shape)
		{
			if (shape->getFlags() & physx::PxShapeFlag::eTRIGGER_SHAPE)
				return true;

			if (isTrigger(shape->getSimulationFilterData()))
				return true;

			if (shape->userData)
				return true;

			return false;
		}

		static PxShape* createTriggerShape(const PxGeometry& geom, PxMaterial& gMaterial, bool isExclusive)
		{
			PxShapeFlags shapeFlags = PxShapeFlag::eVISUALIZATION | PxShapeFlag::eTRIGGER_SHAPE;

			return PhysXMgr::GetInst()->GPhysics()->createShape(geom, gMaterial, isExclusive, shapeFlags);
		}

		static PxFilterFlags SimulationFilterShader(
			PxFilterObjectAttributes attributes0, 
			PxFilterData filterData0,
			PxFilterObjectAttributes attributes1, 
			PxFilterData filterData1,
			PxPairFlags& pairFlags, const void* /*constantBlock*/, PxU32 /*constantBlockSize*/)
		{
			//if(!CollisionMgr::GetInst()->IsLayerIntersect(filterData0.word0, filterData1.word0))
			//{
			//	//pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
			//	return physx::PxFilterFlag::eSUPPRESS;
			//}

			// 충돌레이어 인데 하나가 trigger인 경우
			if (isTrigger(filterData0) || isTrigger(filterData1))
			{
				pairFlags = PxPairFlag::eTRIGGER_DEFAULT;

				return PxFilterFlag::eDEFAULT;
			}
			// 충돌레이어 인데 둘다 rigid인 경우

			pairFlags = physx::PxPairFlag::eCONTACT_DEFAULT | physx::PxPairFlag::eDETECT_CCD_CONTACT;

			return PxFilterFlag::eDEFAULT;
		}
		//class PxCustomRaycastCallBack :public PxRaycastCallback
		//{
		//public:
		//	PxCustomRaycastCallBack()
		//		: PxHitCallback<PxRaycastHit>(nullptr, 1)
		//	{}
		//	PxAgain processTouches(const PxRaycastHit* buffer, PxU32 nbHits) PX_OVERRIDE
		//	{
		//		maxNbTouches = nbHits;
		//		touches = const_cast<physx::PxRaycastHit*>(buffer);
		//		//for (physx::PxU32 i = 0; i < nbHits; i++)
		//		//{
		//		//	
		//		//}
		//		return true;
		//	}
		//	void finalizeQuery() PX_OVERRIDE
		//	{
		//		if (touches)
		//		{
		//			maxNbTouches = 1;
		//			touches = nullptr;
		//		}
		//	}
		//	~PxCustomRaycastCallBack() PX_OVERRIDE {}
		//};
		class PxCollisionCallBack : public PxSimulationEventCallback
		{
		public:
			//std::map<physx::PxShape*, CCollider3D*> Collisions;

			void onConstraintBreak(PxConstraintInfo* /*constraints*/, PxU32 /*count*/)	PX_OVERRIDE
			{
				OutputDebugStringW(L"onConstraintBreak\n");
			}

			void onWake(PxActor** /*actors*/, PxU32 /*count*/)	PX_OVERRIDE
			{
				OutputDebugStringW(L"onWake\n");
			}

			void onSleep(PxActor** /*actors*/, PxU32 /*count*/)	PX_OVERRIDE
			{
				OutputDebugStringW(L"onSleep\n");
			}

			void onTrigger(PxTriggerPair* pairs, PxU32 count)	PX_OVERRIDE
			{
				OutputDebugStringW(L"onTrigger\n");

				while (count--)
				{
				
					if (pairs[count].triggerActor->userData
						&& pairs[count].otherActor->userData)
					{
						CCollider3D* col1 = static_cast<CCollider3D*>(pairs[count].triggerActor->userData);
						CCollider3D* col2 = static_cast<CCollider3D*>(pairs[count].otherActor->userData);

						if (nullptr != col1 && nullptr != col2)
						{
							UINT col1LayerIdx = col1->GetOwner()->GetLayerIndex();
							UINT col2LayerIdx = col2->GetOwner()->GetLayerIndex();
							if (!CollisionMgr::GetInst()->IsLayerIntersect(col1LayerIdx, col2LayerIdx))
							{
								continue;
							}

							if (pairs[count].status == physx::PxPairFlag::eNOTIFY_TOUCH_FOUND)
							{
								OutputDebugStringW(L"Trigger Begin\n");
								col1->BeginOverlap(col2);
								col2->BeginOverlap(col1);
							}

							else // pairs[count].events == physx::PxPairFlag::eNOTIFY_TOUCH_LOST
							{
								OutputDebugStringW(L"Trigger Exit\n");
								col1->EndOverlap(col2);
								col2->EndOverlap(col1);
							}
						}
					}
				}
			}

			void onAdvance(const PxRigidBody* const*, const PxTransform*, const PxU32)	PX_OVERRIDE
			{
				OutputDebugStringW(L"onAdvance\n");
			}

			void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 count)	PX_OVERRIDE
			{
				OutputDebugStringW(L"onContact\n");


				if (pairHeader.actors[0]->userData
					&& pairHeader.actors[1]->userData)
				{

					CCollider3D* col1 = static_cast<CCollider3D*>(pairHeader.actors[0]->userData);
					CCollider3D* col2 = static_cast<CCollider3D*>(pairHeader.actors[1]->userData);

					if (nullptr != col1 && nullptr != col2)
					{
						UINT col1LayerIdx = col1->GetOwner()->GetLayerIndex();
						UINT col2LayerIdx = col2->GetOwner()->GetLayerIndex();
						if (!CollisionMgr::GetInst()->IsLayerIntersect(col1LayerIdx, col2LayerIdx))
						{
							return;
						}

						if (pairs[0].events == physx::PxPairFlag::eNOTIFY_TOUCH_FOUND)
						{
							OutputDebugStringW(L"Contact Begin\n");
							col1->BeginOverlap(col2);
							col2->BeginOverlap(col1);
						}
						else if (pairs[0].events == physx::PxPairFlag::eNOTIFY_TOUCH_PERSISTS)
						{
							OutputDebugStringW(L"Contact On\n");
							col1->OnOverlap(col2);
							col2->OnOverlap(col1);
						}

						else // pairs[count].events == physx::PxPairFlag::eNOTIFY_TOUCH_LOST
						{
							OutputDebugStringW(L"Contact Exit\n");
							col1->EndOverlap(col2);
							col2->EndOverlap(col1);
						}
					}
				}
			}
		};
		static float DegToRad() { return XM_PI / 180.f; }
		static float RadToDeg() { return 180.f / XM_PI; }
		static Quaternion Vector3ToQuaternion(float yaw, float pitch, float roll)
		{
			float yawOver2 = yaw * 0.5f;
			float sinYawOver2 = (float)sin((double)yawOver2);
			float cosYawOver2 = (float)cos((double)yawOver2);

			float pitchOver2 = pitch * 0.5f;
			float sinPitchOver2 = (float)sin((double)pitchOver2);
			float cosPitchOver2 = (float)cos((double)pitchOver2);

			float rollOver2 = roll * 0.5f;
			float sinRollOver2 = (float)sin((double)rollOver2);
			float cosRollOver2 = (float)cos((double)rollOver2);
			
			Quaternion result;
			result.w = cosYawOver2 * cosPitchOver2 * cosRollOver2 + sinYawOver2 * sinPitchOver2 * sinRollOver2;
			result.x = cosYawOver2 * sinPitchOver2 * cosRollOver2 + sinYawOver2 * cosPitchOver2 * sinRollOver2;
			result.y = sinYawOver2 * cosPitchOver2 * cosRollOver2 - cosYawOver2 * sinPitchOver2 * sinRollOver2;
			result.z = cosYawOver2 * cosPitchOver2 * sinRollOver2 - sinYawOver2 * sinPitchOver2 * cosRollOver2;

			return result;
		}
		static Quaternion Vector3ToQuaternion(PxVec3 _vec)
		{
			return Vector3ToQuaternion(_vec.y, _vec.x, _vec.z);
		}
		static Quaternion Vector3ToQuaternion(Vec3 _vec)
		{
			return Vector3ToQuaternion(_vec.y, _vec.x, _vec.z);
		}
		static Vector3 QuaternionToVector3(Quat quat)
		{
			float w = quat.w;
			float x = quat.x;
			float y = quat.y;
			float z = quat.z;

			float sqW = w * w;
			float sqX = x * x;
			float sqY = y * y;
			float sqZ = z * z;
			float unit = sqX + sqY + sqZ + sqW;
			float test = x * w - y * z;
			Vec3 v;

			if (test > 0.4955f * unit)
			{
				v.y = 2.f * atan2f(y, x);
				v.x = XM_PI / 2.f;
				v.z = 0;
			}
			else if (test < -0.4995f * unit)
			{
				v.x = -2.f * atan2f(y, x);
				v.x = -XM_PI / 2.f;
				v.z = 0;
			}
			else
			{
				Quat quat(w, z, x, y);
				v.x = (float)asinf(2.f * (quat.x * quat.z - quat.w * quat.y));
				v.y = (float)atan2(2.f * quat.x * quat.w + 2.f * quat.y * quat.z, 1.f - 2.f * (quat.z * quat.z + quat.w * quat.w));
				v.z = (float)atan2(2.f * quat.x * quat.y + 2.f * quat.z * quat.w, 1.f - 2.f * (quat.y * quat.y + quat.z * quat.z));
			}
			return v;
		}
		static PxTransform PxTransformFromVec3PosRot(PxVec3 _vPos, PxVec3 _vRot)
		{
			Quat quat; quat = Util::Vector3ToQuaternion(_vRot);
			return PxTransform(_vPos, PxQuat(quat.x, quat.y, quat.z, quat.w));

		}
		static PxTransform PxTransformFromVec3PosRot(Vec3 _vPos, Vec3 _vRot)
		{
			Quat quat; quat = Util::Vector3ToQuaternion(_vRot);
			return PxTransform(PxVec3(_vPos.x, _vPos.y, _vPos.z), PxQuat(quat.x, quat.y, quat.z, quat.w));
		}
		static Matrix WorldMatFromGlobalPose(physx::PxTransform pos, Vec3 vScale, Vec3 vRot, Vec3 Offset)
		{
			Vec3 vPos = Vec3(pos.p.x, pos.p.y, pos.p.z);

			vPos.x += Offset.x;
			vPos.y += Offset.y;
			vPos.z += Offset.z;

			Matrix matTranslation = XMMatrixTranslation(vPos.x, vPos.y, vPos.z);

			Matrix mRot = XMMatrixIdentity();

			mRot = XMMatrixRotationX(vRot.x);
			mRot *= XMMatrixRotationY(vRot.y);
			mRot *= XMMatrixRotationZ(vRot.z);

			Matrix matWorldScale = XMMatrixIdentity();
			matWorldScale = XMMatrixScaling(vScale.x, vScale.y, vScale.z);

			return matWorldScale * mRot * matTranslation;
		}
		static Matrix WorldMatFromGlobalPose(physx::PxTransform pos, Vec3 vScale)
		{
			Vec3 vPos = Vec3(pos.p.x, pos.p.y, pos.p.z);
			Matrix matTranslation = XMMatrixTranslation(vPos.x, vPos.y, vPos.z);

			Quat rotation(pos.q.x, pos.q.y, pos.q.z, pos.q.w);
			Matrix mRot = Matrix::CreateFromQuaternion(rotation);
			Matrix matWorldScale = XMMatrixScaling(vScale.x, vScale.y, vScale.z);

			return matWorldScale * mRot * matTranslation;
		}
	}

}

#endif

