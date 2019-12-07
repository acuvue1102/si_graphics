#pragma once

#include "si_base/core/singleton.h"

namespace SI
{
	class BaseCommandQueue;
	class GfxCommandList;
	class GfxFence;

	class GfxCommandQueue
	{
	public:
		GfxCommandQueue(BaseCommandQueue* base=nullptr);
		~GfxCommandQueue();

	public:
		void ExecuteCommandList(GfxCommandList& list);
		void ExecuteCommandLists(uint32_t count, GfxCommandList** lists);
		int Signal(GfxFence& fence, uint64_t fenceValue);

	public:
		BaseCommandQueue* GetBaseCommandQueue(){ return m_base; }
		const BaseCommandQueue* GetBaseCommandQueue() const{ return m_base; }

	private:
		BaseCommandQueue* m_base;
	};

} // namespace SI