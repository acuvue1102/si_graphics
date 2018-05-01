
#include "pipeline_001.h"

#include <string>
#include <si_base/core/core.h>
#include <si_app/file/path_storage.h>

namespace SI
{
namespace APP001
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

	//////////////////////////////////////////////////////////
	
	Pipeline::Pipeline(int observerSortKey)
		: PipelineBase(observerSortKey)
	{
	}

	Pipeline::~Pipeline()
	{
	}
	
	int Pipeline::OnInitialize(const AppInitializeInfo& info)
	{
		if( PipelineBase::OnInitialize(info) != 0) return -1;
		if( LoadAsset(info) != 0 ) return -1;

		return 0;
	}

	int Pipeline::LoadAsset(const AppInitializeInfo& info)
	{
		m_rootSignature = m_device.CreateRootSignature();

		std::string shaderPath = PathStorage::GetInstance()->GetExeDirPath();
		shaderPath += "shaders\\color.hlsl";

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

		float aspect = (float)info.m_width/(float)info.m_height;

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
			
		m_viewport = GfxViewport(0.0f, 0.0f, (float)info.m_width, (float)info.m_height);
		m_scissor  = GfxScissor(0, 0, info.m_width, info.m_height);

		return 0;
	}

	int Pipeline::OnTerminate()
	{
		m_swapChain.Flip(); // wait previous frame.

		m_device.ReleaseBuffer(m_vertexBuffer);

		m_device.ReleaseGraphicsState(m_graphicsState);

		m_device.ReleaseRootSignature(m_rootSignature);

		PipelineBase::OnTerminate();

		return 0;
	}
	
	int Pipeline::OnRender(const AppRenderInfo&)
	{
		BeginRender();

		GfxTexture tex = m_swapChain.GetSwapChainTexture();

		m_graphicsCommandList.SetGraphicsState(m_graphicsState);
						
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
	
} // namespace APP001
} // namespace SI
