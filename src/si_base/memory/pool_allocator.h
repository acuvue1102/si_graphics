#pragma once

#include <cstdint>
#include "si_base/core/basic_function.h"
#include "si_base/core/non_copyable.h"
#include "si_base/memory/allocator_base.h"

namespace SI
{
	// プールのメモリ領域は外で管理するプールアロケータ.
	class PoolAllocator :public AllocatorBase
	{
	public:
		PoolAllocator();
		virtual ~PoolAllocator();

		void Initialize(void* poolMemory, size_t poolMemorySize);
		void Terminate();
		
		void* Allocate(size_t size) override;
		void* Allocate(size_t size, size_t alignment) override;

		void Deallocate(void* p) override;

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


