
#include "base/base.h"
#include "gpu/gfx.h"

#include "pipeline/pipeline.h"

namespace SI
{
	class PipelineImpl
	{
	public:
		PipelineImpl()
		{
		}

		~PipelineImpl()
		{
		}
		int Initialize(const PipelineDesc& desc)
		{
			GfxDeviceConfig deviceConfig;
			deviceConfig.m_width = desc.m_width;
			deviceConfig.m_height = desc.m_height;
			deviceConfig.m_hWnd = desc.m_hWnd;
			if(m_device.Initialize(deviceConfig) != 0)
			{
				return -1;
			}

			m_commandQueue = m_device.CreateCommandQueue();

			m_swapChain = m_device.CreateSwapChain(deviceConfig, m_commandQueue);

			m_graphicsCommandList = m_device.CreateGraphicsCommandList();

			return 0;
		}

		int Terminate()
		{
			m_swapChain.Flip();

			m_device.ReleaseGraphicsCommandList(m_graphicsCommandList);
			
			m_device.ReleaseSwapChain(m_swapChain);

			m_device.ReleaseCommandQueue(m_commandQueue);

			m_device.Terminate();

			return 0;
		}

		int Render()
		{
			BeginRender();

			GfxTexture tex = m_swapChain.GetSwapChainTexture();
			m_graphicsCommandList.ClearRenderTarget(tex, 0.0f, 0.2f, 0.4f, 1.0f);

			EndRender();

			return 0;
		}
	
		int BeginRender()
		{
			m_graphicsCommandList.Reset(m_graphicsState);

		
			m_graphicsCommandList.ResourceBarrier(
				m_swapChain.GetSwapChainTexture(),
				GfxResourceState::kCommon,
				GfxResourceState::kRenderTarget);

			return 0;
		}

		int EndRender()
		{
			m_graphicsCommandList.ResourceBarrier(
				m_swapChain.GetSwapChainTexture(),
				GfxResourceState::kRenderTarget,
				GfxResourceState::kCommon);

			int ret = m_graphicsCommandList.Close();
			if(ret != 0) return -1;
		
			m_commandQueue.ExecuteCommandList(m_graphicsCommandList);
			
			ret = m_swapChain.Present(1);
			if(ret != 0) return -1;

			if( m_swapChain.Flip() != 0 )
			{
				return -1;
			}

			return 0;
		}

	private:
		GfxDevice                m_device;
		GfxCommandQueue          m_commandQueue;
		GfxSwapChain             m_swapChain;
		GfxGraphicsCommandList   m_graphicsCommandList;
		GfxGraphicsState         m_graphicsState;
	};

	//////////////////////////////////////////////////////////
	
	Pipeline::Pipeline()
		: m_impl(nullptr)
	{
	}

	Pipeline::~Pipeline()
	{
	}

	int Pipeline::Initialize(const PipelineDesc& desc)
	{
		Terminate();

		m_impl = new PipelineImpl();
		return m_impl->Initialize(desc);
	}

	int Pipeline::Terminate()
	{
		if(m_impl)
		{
			m_impl->Terminate();
			SafeDelete(m_impl);
		}

		return 0;
	}

	int Pipeline::Render()
	{
		return m_impl->Render();
	}

} // namespace SI
