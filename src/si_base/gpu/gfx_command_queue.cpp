
#include "si_base/gpu/dx12/dx12_command_queue.h"
#include "si_base/gpu/gfx_fence.h"
#include "si_base/gpu/gfx_command_list.h"
#include "si_base/gpu/gfx_command_queue.h"

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

