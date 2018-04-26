
#include "gpu\gfx_config.h"

#if SI_USE_DX12

#include <dxgi1_4.h>
#include <comdef.h>
#include "base/base.h"
#include "gpu/gfx_enum.h"
#include "gpu/dx12/dx12_texture.h"
#include "gpu/dx12/dx12_graphics_state.h"
#include "gpu/dx12/dx12_graphics_command_list.h"

namespace SI
{
	BaseGraphicsCommandList::BaseGraphicsCommandList()
	{
	}

	
	BaseGraphicsCommandList::~BaseGraphicsCommandList()
	{
		Terminate();
	}
	
	int BaseGraphicsCommandList::Initialize(ID3D12Device& device)
	{
		HRESULT hr = device.CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator));
		if(FAILED(hr))
		{
			SI_ASSERT(0, "error CreateCommandAllocator", _com_error(hr).ErrorMessage());
			return -1;
		}

		hr = device.CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_graphicsCommandList));
		if(FAILED(hr))
		{
			SI_ASSERT(0, "error CreateCommandList", _com_error(hr).ErrorMessage());
			return -1;
		}

		hr = m_graphicsCommandList->Close();
		if(FAILED(hr))
		{
			SI_ASSERT(0, "error m_graphicsCommandList->Close", _com_error(hr).ErrorMessage());
			return -1;
		}

		return 0;
	}

	int BaseGraphicsCommandList::Terminate()
	{
		m_graphicsCommandList.Reset();
		m_commandAllocator.Reset();

		return 0;
	}	
	
	int BaseGraphicsCommandList::Reset(BaseGraphicsState& graphicsState)
	{
		//ID3D12PipelineState* pipelineState = graphicsState.GetGraphicsState();

		HRESULT hr = m_commandAllocator->Reset();
		if(FAILED(hr))
		{
			SI_ASSERT(0, "error m_commandAllocator->Reset", _com_error(hr).ErrorMessage());
			return -1;
		}

		hr = m_graphicsCommandList->Reset(m_commandAllocator.Get(), nullptr);//pipelineState);
		if(FAILED(hr))
		{
			SI_ASSERT(0, "error m_graphicsCommandList->Reset", _com_error(hr).ErrorMessage());
			return -1;
		}

		return 0;
	}
		
	void BaseGraphicsCommandList::ClearRenderTarget(BaseTexture& tex, float r, float g, float b, float a)
	{
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = {};
		rtvHandle.ptr = tex.GetPtr();

		// Record commands.
		float clearColor[] = { r, g, b, a };
		m_graphicsCommandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	}

	void BaseGraphicsCommandList::ResourceBarrier(
		BaseTexture& texture,
		const GfxResourceState& before,
		const GfxResourceState& after)
	{
		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = texture.GetComPtrResource().Get();
		barrier.Transition.StateBefore = (D3D12_RESOURCE_STATES)before.GetStateFlags();
		barrier.Transition.StateAfter  = (D3D12_RESOURCE_STATES)after.GetStateFlags();
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		
		m_graphicsCommandList->ResourceBarrier(1, &barrier);
	}

} // namespace SI

#endif // SI_USE_DX12
