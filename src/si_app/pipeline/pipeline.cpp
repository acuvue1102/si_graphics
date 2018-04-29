
#include "si_base/core/base.h"
#include "si_base/gpu/gfx.h"

#include <string>
#include "si_app/file/path_storage.h"
#include "si_app/pipeline/pipeline.h"

namespace SI
{
	struct PosColorVertex
	{
		float m_x;
		float m_y;
		float m_z;
		float m_r;
		float m_g;
		float m_b;
		float m_a;
	};

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
			m_rootSignature = m_device.CreateRootSignature();

			if( LoadAsset(desc) != 0 )
			{
				return -1;
			}

			return 0;
		}

		int LoadAsset(const PipelineDesc& desc)
		{
			std::string shaderPath = PathStorage::GetInstance()->GetExeDirPath();
			shaderPath += "shader\\color.hlsl";

			if(m_vertexShader.LoadAndCompile(shaderPath.c_str()) != 0) return -1;
			if(m_pixelShader. LoadAndCompile(shaderPath.c_str()) != 0) return -1;

			static const GfxInputElement kElements[] =
			{
				{"POSITION", 0, kGfxFormat_R32G32B32_Float,    0, 0},
				{"COLOR",    0, kGfxFormat_R32G32B32A32_Float, 0, 12},
			};

			GfxGraphicsStateDesc stateDesc;
			stateDesc.m_inputElements      = kElements;
			stateDesc.m_inputElementCount  = (int)ArraySize(kElements);
			stateDesc.m_rootSignature      = &m_rootSignature;
			stateDesc.m_vertexShader       = &m_vertexShader;
			stateDesc.m_pixelShader        = &m_pixelShader;
			m_graphicsState = m_device.CreateGraphicsState(stateDesc);

			float aspect = (float)desc.m_width/(float)desc.m_height;

			static const PosColorVertex kVertexData[] = 
			{
				{ 0.0f,   0.25f * aspect, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f},
				{ 0.25f, -0.25f * aspect, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f},
				{-0.25f, -0.25f * aspect, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f}
			};

			GfxBufferDesc bufferDesc;
			bufferDesc.m_heapType = kGfxHeapType_Upload;
			bufferDesc.m_bufferSizeInByte = sizeof(kVertexData);
			bufferDesc.m_initialData = (const void*)kVertexData;
			m_vertexBuffer = m_device.CreateBuffer(bufferDesc);
			
			m_viewport = GfxViewport(0.0f, 0.0f, (float)desc.m_width, (float)desc.m_height);
			m_scissor  = GfxScissor(0, 0, desc.m_width, desc.m_height);

			return 0;
		}

		int Terminate()
		{
			m_swapChain.Flip();

			m_device.ReleaseBuffer(m_vertexBuffer);

			m_device.ReleaseGraphicsState(m_graphicsState);

			m_device.ReleaseRootSignature(m_rootSignature);

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

			//m_graphicsCommandList.SetGraphicsState(m_graphicsState);
						
			m_graphicsCommandList.SetGraphicsRootSignature(m_rootSignature);

			m_graphicsCommandList.SetRenderTargets(1, &tex);
			m_graphicsCommandList.SetViewports(1, &m_viewport);
			m_graphicsCommandList.SetScissors(1, &m_scissor);

			m_graphicsCommandList.ClearRenderTarget(tex, 0.0f, 0.2f, 0.4f, 1.0f);

			m_graphicsCommandList.SetPrimitiveTopology(GfxPrimitiveTopology_TriangleList);
			m_graphicsCommandList.SetVertexBuffers(
				0,
				1,
				&GfxVertexBufferView(&m_vertexBuffer, sizeof(PosColorVertex)));
			
			m_graphicsCommandList.DrawInstanced(3, 1, 0, 0);

			EndRender();

			return 0;
		}
	
		int BeginRender()
		{
			m_graphicsCommandList.Reset(m_graphicsState);

			GfxTexture swapChainTexture = m_swapChain.GetSwapChainTexture();
			m_graphicsCommandList.ResourceBarrier(
				swapChainTexture,
				GfxResourceState::kCommon,
				GfxResourceState::kRenderTarget);

			return 0;
		}

		int EndRender()
		{
			GfxTexture swapChainTexture = m_swapChain.GetSwapChainTexture();
			m_graphicsCommandList.ResourceBarrier(
				swapChainTexture,
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
		GfxRootSignature         m_rootSignature;
		GfxGraphicsState         m_graphicsState;
		
		GfxVertexShader          m_vertexShader;
		GfxPixelShader           m_pixelShader;

		GfxBuffer                m_vertexBuffer;

		GfxViewport              m_viewport;
		GfxScissor               m_scissor;
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

		m_impl = SI_NEW(PipelineImpl);
		return m_impl->Initialize(desc);
	}

	int Pipeline::Terminate()
	{
		if(m_impl)
		{
			m_impl->Terminate();
			SI_DELETE(m_impl);
		}

		return 0;
	}

	int Pipeline::Render()
	{
		return m_impl->Render();
	}

} // namespace SI
