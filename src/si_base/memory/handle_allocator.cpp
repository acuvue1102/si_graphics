
#include "si_base/memory/handle_allocator.h"

#include "si_base/core/core.h"

namespace SI
{
	HandleAllocator::Node  HandleAllocator::s_lastFreeNode = {};

	HandleAllocator::HandleAllocator()
		: m_maxItemCount(0)
		, m_currentItemCount(0)
		, m_nodes(nullptr)
	{
		m_topFreeNode.m_handle = kInvalidHandle;
		m_topFreeNode.m_next = &s_lastFreeNode;
	}

	HandleAllocator::~HandleAllocator()
	{
		Terminate();
	}
	
	void HandleAllocator::Initialize(uint32_t itemCount)
	{
		SI_ASSERT(m_nodes==nullptr);
		SI_ASSERT(0<itemCount);
		
		m_maxItemCount = itemCount;
		m_currentItemCount = 0;
		m_nodes = SI_NEW_ARRAY(Node, m_maxItemCount);
		
		m_nodes[0].m_handle = 0;
		m_topFreeNode.m_next = &m_nodes[0];
		for(uint32_t i=1; i<m_maxItemCount; ++i)
		{
			m_nodes[i].m_handle = i;
			m_nodes[i-1].m_next = &m_nodes[i];
		}
	}
	
	void HandleAllocator::Terminate()
	{
		SI_ASSERT(m_currentItemCount == 0, "開放忘れ");

		if(m_nodes)
		{
			m_maxItemCount = 0;
			m_currentItemCount = 0;
			m_topFreeNode.m_handle = kInvalidHandle;
			m_topFreeNode.m_next = nullptr;

			SI_DELETE_ARRAY(m_nodes);
			m_nodes = nullptr;
		}
	}

	uint32_t HandleAllocator::Allocate()
	{
		if(m_maxItemCount <= m_currentItemCount) return kInvalidHandle;
		if(m_topFreeNode.m_next == &s_lastFreeNode) return kInvalidHandle;

		Node* n = m_topFreeNode.m_next;
		m_topFreeNode.m_next = n->m_next;
		n->m_next = nullptr;

		++m_currentItemCount;

		return n->m_handle;
	}

	void HandleAllocator::Deallocate(uint32_t h)
	{
		SI_ASSERT(m_nodes);

		if(m_maxItemCount <= h)
		{
			SI_ASSERT(0, "開放済み");
			return;
		}
		
		Node& node = m_nodes[h];
		if(node.m_next)
		{
			SI_ASSERT(0, "開放済み. ここには絶対来ないはず.");
			return;
		}

		Node* next = m_topFreeNode.m_next;
		m_topFreeNode.m_next = &node;
		node.m_next = next;

		--m_currentItemCount;
	}

} // namespace SI
