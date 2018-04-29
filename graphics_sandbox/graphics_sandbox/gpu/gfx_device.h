﻿#pragma once

#include "gpu/gfx_command_queue.h"
#include "gpu/gfx_graphics_command_list.h"
#include "gpu/gfx_texture.h"
#include "gpu/gfx_swap_chain.h"
#include "gpu/gfx_graphics_state.h"
#include "gpu/gfx_fence.h"
#include "gpu/gfx_root_signature.h"
#include "gpu/gfx_buffer.h"

namespace SI
{
	class BaseDevice;
	struct GfxDeviceConfig;
	struct GfxGraphicsStateDesc;

	class GfxDevice
	{
	public:
		GfxDevice();
		~GfxDevice();

		int Initialize(const GfxDeviceConfig& config);
		int Terminate();

	public:
		GfxCommandQueue CreateCommandQueue();
		void ReleaseCommandQueue(GfxCommandQueue& commandQueue);

		GfxSwapChain CreateSwapChain(
			const GfxDeviceConfig& config,
			GfxCommandQueue& commandQueue);
		void ReleaseSwapChain(GfxSwapChain& swapChain);

		GfxGraphicsCommandList CreateGraphicsCommandList();
		void ReleaseGraphicsCommandList(GfxGraphicsCommandList& commandList);

		GfxGraphicsState CreateGraphicsState(const GfxGraphicsStateDesc& desc);
		void ReleaseGraphicsState(GfxGraphicsState& state);

		GfxFence CreateFence();
		void ReleaseFence(GfxFence& fence);

		GfxFenceEvent CreateFenceEvent();
		void ReleaseFenceEvent(GfxFenceEvent& event);

		GfxRootSignature CreateRootSignature();
		void ReleaseRootSignature(GfxRootSignature& signature);

		GfxBuffer CreateBuffer(const GfxBufferDesc& desc);
		void ReleaseBuffer(GfxBuffer& buffer);

	private:
		BaseDevice* m_base;
	};

} // namespace SI
