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
#include "physx_util.h"

#include "foundation/PxSimpleTypes.h"
#include "foundation/PxMat33.h"
#include "foundation/PxQuat.h"
#include "foundation/PxAtomic.h"
#include "foundation/PxMutex.h"
#include "foundation/PxAllocator.h"
#include "foundation/PxString.h"
#include "foundation/PxSync.h"
#include "foundation/PxThread.h"
#include "foundation/PxTime.h"

#include "extensions/PxDefaultAllocator.h"

namespace physx
{

	namespace
	{
		PxDefaultAllocator gUtilAllocator;

		struct UtilAllocator // since we're allocating internal classes here, make sure we align properly
		{
			void* allocate(size_t size, const char* file, PxU32 line) { return gUtilAllocator.allocate(size, NULL, file, int(line)); }
			void deallocate(void* ptr) { gUtilAllocator.deallocate(ptr); }
		};
	}

	namespace Util
	{
		PxVec3 BRandom::unitRandomPt()
		{
			PxVec3 v;
			do
			{
				v.x = randomFloat();
				v.y = randomFloat();
				v.z = randomFloat();
			} while (v.normalize() < 1e-6f);
			return v;
		}

		PxQuat BRandom::unitRandomQuat()
		{
			PxQuat v;
			do
			{
				v.x = randomFloat();
				v.y = randomFloat();
				v.z = randomFloat();
				v.w = randomFloat();
			} while (v.normalize() < 1e-6f);

			return v;
		}

		void BRandom::unitRandomPt(PxVec3& v)
		{
			v = unitRandomPt();
		}
		void BRandom::unitRandomQuat(PxQuat& v)
		{
			v = unitRandomQuat();
		}

		PxI32 atomicIncrement(volatile PxI32* val)
		{
			return PxAtomicIncrement(val);
		}

		PxI32 atomicDecrement(volatile PxI32* val)
		{
			return PxAtomicDecrement(val);
		}

		//******************************************************************************//

		PxU32 getNbPhysicalCores()
		{
			return PxThread::getNbPhysicalCores();
		}

		//******************************************************************************//

		PxU32 getThreadId()
		{
			return static_cast<PxU32>(PxThread::getId());
		}

		//******************************************************************************//

		PxU64 getCurrentTimeCounterValue()
		{
			return PxTime::getCurrentCounterValue();
		}

		PxReal getElapsedTimeInMilliseconds(const PxU64 elapsedTime)
		{
			return PxTime::getCounterFrequency().toTensOfNanos(elapsedTime) / (100.0f * 1000.0f);
		}

		PxReal getElapsedTimeInMicroSeconds(const PxU64 elapsedTime)
		{
			return PxTime::getCounterFrequency().toTensOfNanos(elapsedTime) / (100.0f);
		}

		//******************************************************************************//

		struct Sync : public PxSyncT<UtilAllocator> {};

		Sync* syncCreate()
		{
			return new(gUtilAllocator.allocate(sizeof(Sync), 0, 0, 0)) Sync();
		}

		void syncWait(Sync* sync)
		{
			sync->wait();
		}

		void syncSet(Sync* sync)
		{
			sync->set();
		}

		void syncReset(Sync* sync)
		{
			sync->reset();
		}

		void syncRelease(Sync* sync)
		{
			sync->~Sync();
			gUtilAllocator.deallocate(sync);
		}

		//******************************************************************************//

		struct Thread : public PxThreadT<UtilAllocator>
		{
			Thread(ThreadEntryPoint entryPoint, void* data) :
				PxThreadT<UtilAllocator>(),
				mEntryPoint(entryPoint),
				mData(data)
			{
			}

			virtual void execute(void)
			{
				mEntryPoint(mData);
			}

			ThreadEntryPoint mEntryPoint;
			void* mData;
		};

		Thread* threadCreate(ThreadEntryPoint entryPoint, void* data)
		{
			Thread* createThread = static_cast<Thread*>(gUtilAllocator.allocate(sizeof(Thread), 0, 0, 0));
			PX_PLACEMENT_NEW(createThread, Thread(entryPoint, data));
			createThread->start();
			return createThread;
		}

		void threadQuit(Thread* thread)
		{
			thread->quit();
		}

		void threadSignalQuit(Thread* thread)
		{
			thread->signalQuit();
		}

		bool threadWaitForQuit(Thread* thread)
		{
			return thread->waitForQuit();
		}

		bool threadQuitIsSignalled(Thread* thread)
		{
			return thread->quitIsSignalled();
		}

		void threadRelease(Thread* thread)
		{
			thread->~Thread();
			gUtilAllocator.deallocate(thread);
		}

		//******************************************************************************//

		struct Mutex : public PxMutexT<UtilAllocator> {};

		Mutex* mutexCreate()
		{
			return new(gUtilAllocator.allocate(sizeof(Mutex), 0, 0, 0)) Mutex();
		}

		void mutexLock(Mutex* mutex)
		{
			mutex->lock();
		}

		void mutexUnlock(Mutex* mutex)
		{
			mutex->unlock();
		}

		void mutexRelease(Mutex* mutex)
		{
			mutex->~Mutex();
			gUtilAllocator.deallocate(mutex);
		}
	}// namespace Util
} // namespace physx