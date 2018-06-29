#pragma once

#include <cstdint>
#include "si_base/core/basic_function.h"
#include "si_base/core/non_copyable.h"

namespace SI
{
	// プールのメモリ領域は外で管理するプールアロケータ.
	class PoolAllocator : private NonCopyable
	{
	public:
		PoolAllocator();
		virtual ~PoolAllocator();

		void Initialize(void* poolMemory, size_t poolMemorySize);
		void Terminate();
		
		void* Allocate(size_t size);
		void* Allocate(size_t size, size_t alignment);

		void Deallocate(void* p);

		template<typename T,  class... Args>
		inline T* New(Args&&... args)
		{
			void* buf = Allocate(sizeof(T), alignof(T));
			return new(buf) T(args...);
		}

		template<typename T>
		inline void Delete(T*& p)
		{
			if(p==nullptr) return;

			p->~T();
			Deallocate(p);

			p=nullptr;
		}
		
		template<typename T>
		inline T* NewArray(size_t arrayCount)
		{
			// 配列数を保持するための領域を先頭に用意する.
			static const int kExtraBufferSize = (int)sizeof(uint64_t);
			static const int kTypeSize   = Max((int)sizeof(T), (int)alignof(T));
			static const int kExtraCount = kExtraBufferSize/kTypeSize + (kExtraBufferSize%kTypeSize==0)? 0 : 1;
			static const int kAlignment  = Max((int)alignof(T), 8);

			if(arrayCount<=0) return nullptr;

			void* buf = Allocate(kTypeSize * (arrayCount + kExtraCount), kAlignment);

			T* bufT = (T*)buf;
			T* p = &bufT[kExtraCount];

			SI_ASSERT(8 <= (uintptr_t)p - (uintptr_t)buf, "error"); // 念のため.			
			*reinterpret_cast<uint64_t*>(bufT) = (uint64_t)arrayCount;   // 配列数保存.

			for(size_t i=0; i<arrayCount; ++i)
			{
				new(&p[i]) T;
			}

			return p;
		}

		template<typename T>
		inline void DeleteArray(T*& p)
		{
			// 配列数を保持するための領域を先頭に用意する.
			static const int kExtraBufferSize = (int)sizeof(uint64_t);
			static const int kTypeSize   = Max((int)sizeof(T), (int)alignof(T));
			static const int kExtraCount = kExtraBufferSize/kTypeSize + (kExtraBufferSize%kTypeSize==0)? 0 : 1;

			if(p==nullptr) return;
			
			void* buf = (void*)&p[-kExtraCount]; // 前の部分.
			uint64_t arrayCount = *reinterpret_cast<uint64_t*>(buf); // 配列数を取得.

			for(size_t i=0; i<arrayCount; ++i)
			{
				p[i].~T();
			}
			Deallocate(buf);
			
			p=nullptr;
		}
		
		template<typename T>
		inline T* NewArrayRaw(size_t arrayCount)
		{
			if(arrayCount<=0) return nullptr;

			void* buf = Allocate(arrayCount * sizeof(T), alignof(T));

			T* p = (T*)buf;
			for(size_t i=0; i<arrayCount; ++i)
			{
				new(&p[i]) T;
			}
			return p;
		}

		template<typename T>
		inline void DeleteArrayRaw(T*& p, size_t arrayCount)
		{
			for(size_t i=0; i<arrayCount; ++i)
			{
				p[i].~T();
			}
			Deallocate((void*)p);

			p=nullptr;
		}

	protected:
		void*  m_poolMemory;
		size_t m_poolMemorySize;

	private:
		void*  m_mspace;
	};

	// プールのメモリ領域も管理するプールアロケータ.
	class PoolAllocatorEx : public PoolAllocator
	{
	public:
		PoolAllocatorEx();
		virtual ~PoolAllocatorEx();

		void InitializeEx(size_t poolMemorySize);
		void TerminateEx();
	};

} // namespace SI


