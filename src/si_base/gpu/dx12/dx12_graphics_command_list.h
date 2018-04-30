#pragma once

#include "si_base/gpu/gfx_config.h"

#if SI_USE_DX12
#include <d3d12.h>
#include <wrl/client.h>
#include <comdef.h>
#include "si_base/core/core.h"
#include "si_base/gpu/dx12/dx12_enum.h"
#include "si_base/gpu/dx12/dx12_command_list.h"
#include "si_base/gpu/dx12/dx12_root_signature.h"
#include "si_base/gpu/dx12/dx12_texture.h"
#include "si_base/gpu/dx12/dx12_buffer.h"
#include "si_base/gpu/dx12/dx12_graphics_state.h"
#include "si_base/gpu/gfx_texture.h"
#include "si_base/gpu/gfx_buffer.h"
#include "si_base/gpu/gfx_viewport.h"

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
		
		inline int Reset(BaseGraphicsState* graphicsState)
		{
			ID3D12PipelineState* pipelineState = graphicsState? graphicsState->GetComPtrGraphicsState().Get() : nullptr;

			HRESULT hr = m_commandAllocator->Reset();
			if(FAILED(hr))
			{
				SI_ASSERT(0, "error m_commandAllocator->Reset", _com_error(hr).ErrorMessage());
				return -1;
			}

			hr = m_graphicsCommandList->Reset(m_commandAllocator.Get(), pipelineState);
			if(FAILED(hr))
			{
				SI_ASSERT(0, "error m_graphicsCommandList->Reset", _com_error(hr).ErrorMessage());
				return -1;
			}

			return 0;
		}

		inline int Close()
		{
			HRESULT hr = m_graphicsCommandList->Close();
			if(FAILED(hr))
			{
				SI_ASSERT(0, "error m_graphicsCommandList->Close: %s", _com_error(hr).ErrorMessage());
				return -1;
			}

			return 0;
		}
		
		void SetGraphicsState(BaseGraphicsState& graphicsState)
		{
			m_graphicsCommandList->SetPipelineState(graphicsState.GetComPtrGraphicsState().Get());
		}

		inline void ClearRenderTarget(BaseTexture& tex, float r, float g, float b, float a)
		{
			D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = {};
			rtvHandle.ptr = tex.GetPtr();

			float clearColor[] = { r, g, b, a };
			m_graphicsCommandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
		}

		inline void ResourceBarrier(
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
		
		inline void SetGraphicsRootSignature(BaseRootSignature& rootSignature)
		{
			m_graphicsCommandList->SetGraphicsRootSignature(rootSignature.GetComPtrRootSignature().Get());
		}
		
		inline void SetViewports(uint32_t count, GfxViewport* viewPorts)
		{
			D3D12_VIEWPORT d3dViewports[8];
			SI_ASSERT(count <=ArraySize(d3dViewports));
			uint32_t viewPortCount = Min(count, (uint32_t)ArraySize(d3dViewports));

			for(uint32_t v=0; v<viewPortCount; ++v)
			{
				D3D12_VIEWPORT& outV = d3dViewports[v];
				GfxViewport&    inV  = viewPorts[v];

				outV.TopLeftX = inV.GetTopLeftX();
				outV.TopLeftY = inV.GetTopLeftY();
				outV.Width    = inV.GetWidth();
				outV.Height   = inV.GetHeight();
				outV.MinDepth = inV.GetMinDepth();
				outV.MaxDepth = inV.GetMaxDepth();
			}
			m_graphicsCommandList->RSSetViewports(viewPortCount, d3dViewports);
		}
		
		inline void SetScissors(uint32_t count, GfxScissor* scissors)
		{
			D3D12_RECT d3dScissors[8];
			SI_ASSERT(count <=ArraySize(d3dScissors));
			uint32_t scissorCount = Min(count, (uint32_t)ArraySize(d3dScissors));

			for(uint32_t s=0; s<scissorCount; ++s)
			{
				D3D12_RECT&   outS = d3dScissors[s];
				GfxScissor&   inS  = scissors[s];
				
				outS.left    = inS.GetLeft();
				outS.top     = inS.GetTop();
				outS.right   = inS.GetRight();
				outS.bottom  = inS.GetBottom();
			}
			m_graphicsCommandList->RSSetScissorRects(scissorCount, d3dScissors);
		}

		inline void SetRenderTargets(
			uint32_t      renderTargetCount,
			GfxTexture*   renderTargets,
			GfxTexture*   depthStencilTarget)
		{
			D3D12_CPU_DESCRIPTOR_HANDLE handles[8];
			SI_ASSERT(renderTargetCount <= (uint32_t)ArraySize(handles));
			uint32_t handleCount = Min(renderTargetCount, (uint32_t)ArraySize(handles));

			for(uint32_t h=0; h<handleCount; ++h)
			{
				D3D12_CPU_DESCRIPTOR_HANDLE&  outH = handles[h];
				BaseTexture&                   inH = *renderTargets[h].GetBaseTexture();

				outH.ptr = inH.GetPtr();
			}
			
			if(depthStencilTarget)
			{
				D3D12_CPU_DESCRIPTOR_HANDLE depthStencilHandle;
				depthStencilHandle.ptr = depthStencilTarget->GetBaseTexture()->GetPtr();
				m_graphicsCommandList->OMSetRenderTargets(handleCount, handles, FALSE, &depthStencilHandle);
			}
			else
			{
				m_graphicsCommandList->OMSetRenderTargets(handleCount, handles, FALSE, nullptr);
			}
		}
		
		inline void SetPrimitiveTopology(GfxPrimitiveTopology topology)
		{
			m_graphicsCommandList->IASetPrimitiveTopology(GetDx12PrimitiveTopology(topology));
		}
		
		inline void SetVertexBuffers(uint32_t inputSlot, uint32_t viewCount, GfxVertexBufferView* bufferViews)
		{
			D3D12_VERTEX_BUFFER_VIEW d3Views[8];
			SI_ASSERT(viewCount <= (uint32_t)ArraySize(d3Views));
			uint32_t d3dViewCount = Min(viewCount, (uint32_t)ArraySize(d3Views));

			for(uint32_t v=0; v<d3dViewCount; ++v)
			{
				D3D12_VERTEX_BUFFER_VIEW& outV = d3Views[v];
				GfxVertexBufferView&       inV = bufferViews[v];
				
				const BaseBuffer* baseBuffer = inV.GetBuffer()->GetBaseBuffer();

				outV.BufferLocation = baseBuffer->GetLocation();
				outV.SizeInBytes    = (uint32_t)baseBuffer->GetSize();
				outV.StrideInBytes  = (uint32_t)inV.GetStride();
			}
			m_graphicsCommandList->IASetVertexBuffers(inputSlot, d3dViewCount, d3Views);
		}

		inline void DrawInstanced(
			uint32_t vertexCountPerInstance,
			uint32_t instanceCount,
			uint32_t startVertexLocation,
			uint32_t startInstanceLocation)
		{
			m_graphicsCommandList->DrawInstanced(
				vertexCountPerInstance,
				instanceCount,
				startVertexLocation,
				startInstanceLocation);
		}

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
