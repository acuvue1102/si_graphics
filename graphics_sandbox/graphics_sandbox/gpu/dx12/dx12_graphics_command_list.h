#pragma once

#include "gpu/gfx_config.h"

#if SI_USE_DX12
#include <d3d12.h>
#include <wrl/client.h>
#include <comdef.h>
#include <base/base.h>
#include "gpu/dx12/dx12_command_list.h"

namespace SI
{
	class BaseTexture;
	class BaseGraphicsState;
	class GfxResourceState;

	class BaseGraphicsCommandList : public BaseCommandList
	{
	private:
		template<typename T>
		using ComPtr = Microsoft::WRL::ComPtr<T>;

	public:
		BaseGraphicsCommandList();
		virtual ~BaseGraphicsCommandList();

		int Initialize(ID3D12Device& device);
		int Terminate();

		int Reset(BaseGraphicsState& graphicsState);

		int Close()
		{
			HRESULT hr = m_graphicsCommandList->Close();
			if(FAILED(hr))
			{
				SI_ASSERT(0, "error m_graphicsCommandList->Close: %s", _com_error(hr).ErrorMessage());
				return -1;
			}

			return 0;
		}

	public:
		void ClearRenderTarget(BaseTexture& tex, float r, float g, float b, float a);
		
		void ResourceBarrier(
			BaseTexture& texture,
			const GfxResourceState& before,
			const GfxResourceState& after);

	public:
		ID3D12GraphicsCommandList* GetGraphicsCommandList()
		{
			return m_graphicsCommandList.Get();
		}

		ID3D12CommandList* GetCommandList() override
		{
			return m_graphicsCommandList.Get();
		}

	private:
		ComPtr<ID3D12CommandAllocator>    m_commandAllocator;
		ComPtr<ID3D12GraphicsCommandList> m_graphicsCommandList;
	};
} // namespace SI

#endif // SI_USE_DX12
