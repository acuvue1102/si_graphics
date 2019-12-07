#pragma once

#include "si_base/gpu/gfx_config.h"

#if SI_USE_DX12
#include <d3d12.h>
#include <wrl/client.h>
#include "si_base/core/singleton.h"

namespace SI
{
	class BaseCommandList;
	class BaseFence;
	class GfxCommandList;

	class BaseCommandQueue : public Singleton<BaseCommandQueue>
	{
		template<typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	public:
		BaseCommandQueue();
		~BaseCommandQueue();

		int Initialize(ID3D12Device& device);
		void Terminate();
		
		void ExecuteCommandList(BaseCommandList& list);
		void ExecuteCommandLists(int count, GfxCommandList** lists);
		
		int Signal(BaseFence& fence, uint64_t fenceIndex);
		int Wait(BaseFence& fence, uint64_t fenceIndex);

	public:		
		ComPtr<ID3D12CommandQueue>& GetComPtrCommandQueue()
		{
			return m_commandQueue;
		}

	private:
		ComPtr<ID3D12CommandQueue> m_commandQueue;
	};

} // namespace SI

#endif // SI_USE_DX12
