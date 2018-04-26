
#include "gpu/dx12/dx12_command_queue.h"
#include "gpu/gfx_fence.h"
#include "gpu/gfx_command_list.h"
#include "gpu/gfx_command_queue.h"

namespace SI
{
	GfxCommandQueue::GfxCommandQueue(BaseCommandQueue* base)
		: m_base(base)
	{
	}

	GfxCommandQueue::~GfxCommandQueue()
	{
	}
	
	void GfxCommandQueue::ExecuteCommandList(GfxCommandList& list)
	{
		m_base->ExecuteCommandList(*list.GetBaseCommandList());
	}
	
	int GfxCommandQueue::Signal(GfxFence& fence, uint64_t fenceValue)
	{
		return m_base->Signal(*fence.GetBaseFence(), fenceValue);
	}

} // namespace SI

