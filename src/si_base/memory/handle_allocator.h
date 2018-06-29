#pragma once

#include <cstdint>
#include "si_base/core/non_copyable.h"

namespace SI
{
	static const uint32_t kInvalidHandle = (uint32_t)(-1);

	// ハンドルを確保・開放するためのアロケータ.
	class HandleAllocator : private NonCopyable
	{
	public:

		struct Node
		{
			Node*    m_next;
			uint32_t m_handle;
		};

	public:
		HandleAllocator();
		~HandleAllocator();

		void Initialize(uint32_t itemCount);
		void Terminate();
		
		uint32_t Allocate();
		void Deallocate(uint32_t handle);

		uint32_t GetAllocatedCount() const{ return m_currentItemCount; }

	private:
		uint32_t m_maxItemCount;
		uint32_t m_currentItemCount;
		Node* m_nodes;
		Node  m_topFreeNode;

		static Node  s_lastFreeNode; // 番兵
	};

} // namespace SI


