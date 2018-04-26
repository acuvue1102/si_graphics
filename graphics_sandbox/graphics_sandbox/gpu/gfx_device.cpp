
#include "base/base.h"

#include "gpu/dx12/dx12_device.h"
#include "gpu/gfx_device.h"

namespace SI
{
	GfxDevice::GfxDevice()
		: m_base(nullptr)
	{
	}

	GfxDevice::~GfxDevice()
	{
		Terminate();
	}

	int GfxDevice::Initialize(const GfxDeviceConfig& config)
	{
		if(m_base) return 0;

		m_base = new BaseDevice();
		return m_base->Initialize(config);
	}

	int GfxDevice::Terminate()
	{
		if(!m_base) return 0;

		int ret = m_base->Terminate();
		SafeDelete(m_base);
		
		return ret;
	}
		
	GfxCommandQueue GfxDevice::CreateCommandQueue()
	{
		GfxCommandQueue cq(m_base->CreateCommandQueue());
		return cq;
	}
	
	void GfxDevice::ReleaseCommandQueue(GfxCommandQueue& commandQueue)
	{
		m_base->ReleaseCommandQueue(commandQueue.GetBaseCommandQueue());
		commandQueue = GfxCommandQueue(nullptr);
	}
	
	GfxSwapChain GfxDevice::CreateSwapChain(
		const GfxDeviceConfig& config,
		GfxCommandQueue& commandQueue)
	{
		GfxSwapChain sc(m_base->CreateSwapChain(config, *commandQueue.GetBaseCommandQueue()));
		return sc;
	}
	
	void GfxDevice::ReleaseSwapChain(GfxSwapChain& swapChain)
	{
		m_base->ReleaseSwapChain(swapChain.GetBaseSwapChain());
		swapChain = GfxSwapChain(nullptr);
	}
	
	GfxGraphicsCommandList GfxDevice::CreateGraphicsCommandList()
	{
		GfxGraphicsCommandList gcl(m_base->CreateGraphicsCommandList());
		return gcl;
	}

	void GfxDevice::ReleaseGraphicsCommandList(GfxGraphicsCommandList& commandList)
	{
		m_base->ReleaseGraphicsCommandList(commandList.GetBaseGraphicsCommandList());
		commandList = GfxGraphicsCommandList();
	}
	
	GfxGraphicsState GfxDevice::CreateGraphicsState(const GfxGraphicsStateDesc& desc)
	{
		GfxGraphicsState s(m_base->CreateGraphicsState(desc));
		return s;
	}

	void GfxDevice::ReleaseGraphicsState(GfxGraphicsState& state)
	{
		m_base->ReleaseGraphicsState(state.GetBaseGraphicsState());
		state = GfxGraphicsState();
	}
	
	GfxFence GfxDevice::CreateFence()
	{
		GfxFence f(m_base->CreateFence());
		return f;
	}

	void GfxDevice::ReleaseFence(GfxFence& fence)
	{
		m_base->ReleaseFence(fence.GetBaseFence());
		fence = GfxFence();
	}

	GfxFenceEvent GfxDevice::CreateFenceEvent()
	{
		GfxFenceEvent e(m_base->CreateFenceEvent());
		return e;
	}

	void GfxDevice::ReleaseFenceEvent(GfxFenceEvent& event)
	{
		m_base->ReleaseFenceEvent(event.GetBaseFenceEvent());
		event = GfxFenceEvent();
	}
}
