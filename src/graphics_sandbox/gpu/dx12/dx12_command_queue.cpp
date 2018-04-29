
#include "gpu/gfx_config.h"

#if SI_USE_DX12

#include <dxgi1_4.h>
#include <comdef.h>
#include "base/base.h"
#include "gpu/dx12/dx12_fence.h"
#include "gpu/dx12/dx12_command_list.h"
#include "gpu/dx12/dx12_command_queue.h"

namespace SI
{
	BaseCommandQueue::BaseCommandQueue()
	{
	}

	BaseCommandQueue::~BaseCommandQueue()
	{
	}
	
	int BaseCommandQueue::Initialize(
		ID3D12Device& device)
	{
		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		
		HRESULT hr = device.CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue));
		if(FAILED(hr))
		{
			SI_ASSERT(0, "error CreateCommandQueue", _com_error(hr).ErrorMessage());
			return -1;
		}

		return 0;
	}
	
	void BaseCommandQueue::ExecuteCommandList(BaseCommandList& list)
	{
		ID3D12CommandList* commandLists[] = { list.GetCommandList() };
		m_commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);
	}
	
	void BaseCommandQueue::ExecuteCommandLists(int count, BaseCommandList* list)
	{
		SI_ASSERT(0<count);

		do
		{
			// 16個毎に分割してExecuteする.
			ID3D12CommandList* commandLists[16] = {};
			
			int commandListCount = min((int)ArraySize(commandLists), count);
			for(size_t listId = 0; listId<commandListCount; ++listId)
			{
				commandLists[listId] = list->GetCommandList();
			}
			
			m_commandQueue->ExecuteCommandLists(commandListCount, commandLists);

			count -= (int)ArraySize(commandLists);
			list = &list[ArraySize(commandLists)];
		}while(0<count);
	}

	int BaseCommandQueue::Signal(BaseFence& fence, uint64_t fenceValue)
	{
		HRESULT hr = m_commandQueue->Signal(fence.GetComPtrFence().Get(), fenceValue);
		if(FAILED(hr))
		{
			SI_ASSERT(0, "error CreateCommandQueue", _com_error(hr).ErrorMessage());
			return -1;
		}

		return 0;
	}

} // namespace SI

#endif // SI_USE_DX12
