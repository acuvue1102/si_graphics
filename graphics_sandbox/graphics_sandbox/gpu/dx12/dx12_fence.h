#pragma once

#include "gpu/gfx_config.h"

#if SI_USE_DX12
#include <d3d12.h>
#include <wrl/client.h>

namespace SI
{
	class BaseFenceEvent
	{
	public:
		BaseFenceEvent();
		~BaseFenceEvent();

		int Initialize();
		int Terminate();

		HANDLE GetHandle(){ return m_fenceEvent; }

	private:
		HANDLE                            m_fenceEvent;
	};

	//////////////////////////////////////////////////////////////////////////

	class BaseFence
	{
		template<typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	public:
		BaseFence();
		~BaseFence();

		int Initialize(ID3D12Device& device);
		int Terminate();

	public:
		ComPtr<ID3D12Fence>& GetComPtrFence()
		{
			return m_fence;
		}

	private:
		ComPtr<ID3D12Fence>               m_fence;
	};

} // namespace SI

#endif // SI_USE_DX12
