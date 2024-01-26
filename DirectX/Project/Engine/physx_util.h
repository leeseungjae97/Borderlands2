#pragma once

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

#include "CCollider3D.h"
#include "global.h"
#include "PhysXMgr.h"

#include "foundation/Px.h"
#include "foundation/PxSimpleTypes.h"

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

		PxU32			Bunny_getNbVerts();
		PxU32			Bunny_getNbFaces();
		const PxVec3* Bunny_getVerts();
		const PxU32* Bunny_getFaces();

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

			// Emulates triggers using a filter callback. Doesn't use PxFilterData but needs user-defined way to mark a shape as a trigger.
			FILTER_CALLBACK,
		};

		static bool isTrigger(const physx::PxFilterData& data)
		{
			if (data.word0 != 0xffffffff)
				return false;
			if (data.word1 != 0xffffffff)
				return false;
			if (data.word2 != 0xffffffff)
				return false;
			if (data.word3 != 0xffffffff)
				return false;
			return true;

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

		static PxShape* createTriggerShape(const PxGeometry& geom, PxMaterial& gMaterial, PxTrigger trigger, bool isExclusive)
		{
			PxPhysics* gPhysics = PhysXMgr::GetInst()->GPhysics();
			PxShape* shape = nullptr;
			if (trigger == REAL_TRIGGERS)
			{
				const PxShapeFlags shapeFlags = PxShapeFlag::eVISUALIZATION | PxShapeFlag::eTRIGGER_SHAPE;
				shape = gPhysics->createShape(geom, gMaterial, isExclusive, shapeFlags);
			}
			else if (trigger == FILTER_SHADER)
			{
				PxShapeFlags shapeFlags = PxShapeFlag::eVISUALIZATION | PxShapeFlag::eSIMULATION_SHAPE;
				shape = gPhysics->createShape(geom, gMaterial, isExclusive, shapeFlags);

				// For this method to work, you need a way to mark shapes as triggers without using PxShapeFlag::eTRIGGER_SHAPE
				// (so that trigger-trigger pairs are reported), and without calling a PxShape function (so that the data is
				// available in a filter shader).
				//
				// One way is to reserve a special PxFilterData value/mask for triggers. It may not always be possible depending
				// on how you otherwise use the filter data).
				const PxFilterData triggerFilterData(0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
				shape->setSimulationFilterData(triggerFilterData);
			}
			else if (trigger == FILTER_CALLBACK)
			{
				// We will have access to shape pointers in the filter callback so we just mark triggers in an arbitrary way here,
				// for example using the shape's userData.
				shape = gPhysics->createShape(geom, gMaterial, isExclusive);
				shape->userData = shape;	// Arbitrary rule: it's a trigger if non null
			}
			return shape;
		}

		class TriggersFilterCallback : public PxSimulationFilterCallback
		{
			virtual		PxFilterFlags	pairFound(PxU64 /*pairID*/,
				PxFilterObjectAttributes /*attributes0*/, PxFilterData /*filterData0*/, const PxActor* /*a0*/, const PxShape* s0,
				PxFilterObjectAttributes /*attributes1*/, PxFilterData /*filterData1*/, const PxActor* /*a1*/, const PxShape* s1,
				PxPairFlags& pairFlags)	PX_OVERRIDE
			{
				//		printf("pairFound\n");

				if (s0->userData || s1->userData)	// See createTriggerShape() function
				{
					pairFlags = PxPairFlag::eTRIGGER_DEFAULT;

				if ()
					pairFlags |= PxPairFlag::eDETECT_CCD_CONTACT | PxPairFlag::eNOTIFY_TOUCH_CCD;
				}
				else
					pairFlags = PxPairFlag::eCONTACT_DEFAULT;

				return PxFilterFlags();
			}

			virtual	void pairLost(PxU64 /*pairID*/,
				PxFilterObjectAttributes /*attributes0*/, PxFilterData /*filterData0*/,
				PxFilterObjectAttributes /*attributes1*/, PxFilterData /*filterData1*/,
				bool /*objectRemoved*/)	PX_OVERRIDE
			{
				// printf("pairLost\n");
			}

			virtual	bool statusChange(PxU64& /*pairID*/, PxPairFlags& /*pairFlags*/, PxFilterFlags& /*filterFlags*/)	PX_OVERRIDE
			{
				// printf("statusChange\n");
				return false;
			}
		}gTriggersFilterCallback;

		class PxCollisionCallBack : public PxSimulationEventCallback
		{
		public:
			std::map<physx::PxShape*, CCollider3D*> Collisions;

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
				wchar_t buffer[256];
				std::swprintf(buffer, sizeof(buffer) / sizeof(*buffer),
					L"onTrigger: %d trigger pairs\n", count);

				OutputDebugStringW(buffer);

				//while (count--)
				//{
				//	const PxTriggerPair& current = *pairs++;
				//	if (current.status & PxPairFlag::eNOTIFY_TOUCH_FOUND)
				//		OutputDebugStringW(L"Shape is entering trigger volume\n");
				//	if (current.status & PxPairFlag::eNOTIFY_TOUCH_LOST)
				//		OutputDebugStringW(L"Shape is leaving trigger volume\n");
				//}
			}

			void onAdvance(const PxRigidBody* const*, const PxTransform*, const PxU32)	PX_OVERRIDE
			{
				OutputDebugStringW(L"onAdvance\n");
			}

			void onContact(const PxContactPairHeader& /*pairHeader*/, const PxContactPair* pairs, PxU32 count)	PX_OVERRIDE
			{
				//printf("onContact: %d pairs\n", count);

				wchar_t buffer[256];
				std::swprintf(buffer, sizeof(buffer) / sizeof(*buffer),
					L"onContact: %d pairs\n", count);

				OutputDebugStringW(buffer);

				while (count--)
				{
					//if (pairs[count].flags & (physx::PxContactPairFlag::eREMOVED_SHAPE_0 | physx::PxContactPairFlag::eREMOVED_SHAPE_1))
					//{
					//	continue;
					//}

					/*auto iter1 = Collisions.find(pairs[count].shapes[0]);
					auto iter2 = Collisions.find(pairs[count].shapes[1]);
					if (iter1 != Collisions.end() && iter2 != Collisions.end())
					{
						CCollider3D* col1 = (*iter1).second;
						CCollider3D* col2 = (*iter2).second;
						if (col1 && col2)
						{
							CGameObject* col1Own = col1->GetOwner();
							CGameObject* col2Own = col2->GetOwner();

							if (!col1Own->IsDead() && !col2Own->IsDead())
							{
								if (pairs[count].events == physx::PxPairFlag::eNOTIFY_TOUCH_FOUND)
								{
									OutputDebugStringW(L"Begin\n");
									col1->BeginOverlap(col2);
									col2->BeginOverlap(col1);
								}
								else if (pairs[count].events == physx::PxPairFlag::eNOTIFY_TOUCH_PERSISTS)
								{
									OutputDebugStringW(L"On\n");
									col1->OnOverlap(col2);
									col2->OnOverlap(col1);
								}
							}

							if (pairs[count].events == physx::PxPairFlag::eNOTIFY_TOUCH_LOST)
							{
								OutputDebugStringW(L"Exit\n");
								col1->EndOverlap(col2);
								col2->EndOverlap(col1);
							}
						}
					}*/
					//const PxContactPair& current = *pairs++;

					//if (current.events & (PxPairFlag::eNOTIFY_TOUCH_FOUND | PxPairFlag::eNOTIFY_TOUCH_CCD))
					//	OutputDebugStringW(L"Shape is entering trigger volume\n");
					//if (current.events & PxPairFlag::eNOTIFY_TOUCH_LOST)
					//	OutputDebugStringW(L"Shape is leaving trigger volume\n");

					//if (isTriggerShape(current.shapes[0]) && isTriggerShape(current.shapes[1]))
					//	OutputDebugStringW(L"Trigger-trigger overlap detected\n");
				}
			}
		};

		static void toRotationMatrix(Quaternion quat, float matrix[3][3])
		{
			float x = quat.x;
			float y = quat.y;
			float z = quat.z;
			float w = quat.w;

			matrix[0][0] = 1 - 2 * y * y - 2 * z * z;
			matrix[0][1] = 2 * x * y - 2 * w * z;
			matrix[0][2] = 2 * x * z + 2 * w * y;

			matrix[1][0] = 2 * x * y + 2 * w * z;
			matrix[1][1] = 1 - 2 * x * x - 2 * z * z;
			matrix[1][2] = 2 * y * z - 2 * w * x;

			matrix[2][0] = 2 * x * z - 2 * w * y;
			matrix[2][1] = 2 * y * z + 2 * w * x;
			matrix[2][2] = 1 - 2 * x * x - 2 * y * y;
		}
		static void QuaternionToVector3(Quaternion quat, Vec3& vec)
		{
			float matrix[3][3];
			toRotationMatrix(quat, matrix);
			vec.x = matrix[0][0];
			vec.y = matrix[1][1];
			vec.z = matrix[2][2];
		}
		static void QuaternionToVector3(Quaternion quat, float& vx, float& vy, float& vz)
		{
			float matrix[3][3];
			toRotationMatrix(quat, matrix);
			vx = matrix[0][0];
			vy = matrix[1][1];
			vz = matrix[2][2];
		}
		static Matrix WorldMatFromGlobalPose(physx::PxTransform pos, Vec3 vScale)
		{
			Vec3 vPos = Vec3(pos.p.x, pos.p.y, pos.p.z);
			Matrix matTranslation = XMMatrixTranslation(vPos.x, vPos.y, vPos.z);

			Quaternion rotation(pos.q.x, pos.q.y, pos.q.z, pos.q.w);
			Matrix m_Rot = Matrix::CreateFromQuaternion(rotation);
			Matrix matWorldScale = XMMatrixScaling(vScale.x, vScale.y, vScale.z);

			return matWorldScale * m_Rot * matTranslation;
		}
	}

}
