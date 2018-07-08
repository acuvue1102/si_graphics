
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
		GfxRootSignatureDesc rootSignatureDesc;
		m_rootSignature.Initialize(rootSignatureDesc);

		std::string shaderPath = SI_PATH_STORAGE().GetExeDirPath();
		shaderPath += "shaders\\color.hlsl";

		if(m_vertexShader.LoadAndCompile(shaderPath.c_str()) != 0) return -1;
		if(m_pixelShader. LoadAndCompile(shaderPath.c_str()) != 0) return -1;

		static const GfxInputElement kElements[] =
		{
			{"POSITION", 0, GfxFormat::kR32G32B32_Float,    0, 0},
			{"COLOR",    0, GfxFormat::kR32G32B32A32_Float, 0, 12},
		};

		GfxGraphicsStateDesc stateDesc;
		stateDesc.m_inputElements      = kElements;
		stateDesc.m_inputElementCount  = (int)ArraySize(kElements);
		stateDesc.m_rootSignature      = &m_rootSignature.GetRootSignature();
		stateDesc.m_vertexShader       = &m_vertexShader;
		stateDesc.m_pixelShader        = &m_pixelShader;
		stateDesc.m_rtvFormats[0]      = GfxFormat::kR8G8B8A8_Unorm;
		m_graphicsState = m_device.CreateGraphicsState(stateDesc);

		float aspect = (float)info.m_width/(float)info.m_height;

		static const PosColorVertex kVertexData[] = 
		{
			{ 0.0f,   0.25f * aspect, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f},
			{ 0.25f, -0.25f * aspect, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f},
			{-0.25f, -0.25f * aspect, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f}
		};

		GfxBufferDesc bufferDesc;
		bufferDesc.m_heapType = GfxHeapType::kUpload;
		bufferDesc.m_resourceStates = GfxResourceState::kGenericRead;
		bufferDesc.m_bufferSizeInByte = sizeof(kVertexData);
		m_vertexBuffer = m_device.CreateBuffer(bufferDesc);
		{
			void* outBuffer = m_vertexBuffer.Map();
			memcpy(outBuffer, kVertexData, sizeof(kVertexData));
			m_vertexBuffer.Unmap();
		}

		return 0;
	}

	int Pipeline::OnTerminate()
	{
		m_swapChain.Flip(); // wait previous frame.

		m_device.ReleaseBuffer(m_vertexBuffer);

		m_device.ReleaseGraphicsState(m_graphicsState);

		//m_device.ReleaseRootSignature(m_rootSignature);
		m_rootSignature.Terminate();

		PipelineBase::OnTerminate();

		return 0;
	}
	
	void Pipeline::OnRender(const App& app, const AppUpdateInfo&)
	{
		BeginRender();
		
		GfxTestureEx_SwapChain& swapChainTexture = m_swapChain.GetTexture();
		
		GfxGraphicsContext& context = m_contextManager.GetGraphicsContext(0);

		context.SetPipelineState(m_graphicsState);
						
		context.SetGraphicsRootSignature(m_rootSignature);

		context.SetRenderTarget(swapChainTexture);
			
		GfxViewport viewport(0.0f, 0.0f, (float)swapChainTexture.GetWidth(), (float)swapChainTexture.GetHeight());
		GfxScissor  scissor(0, 0, swapChainTexture.GetWidth(), swapChainTexture.GetHeight());
		context.SetViewports(1, &viewport);
		context.SetScissors(1, &scissor);

		swapChainTexture.SetClearColor(GfxColorRGBA(0.0f, 0.2f, 0.4f, 1.0f));
		context.ClearRenderTarget(swapChainTexture);

		context.SetPrimitiveTopology(GfxPrimitiveTopology::kTriangleList);
		GfxVertexBufferView vbView(m_vertexBuffer, m_vertexBuffer.GetSize(), sizeof(PosColorVertex));
		GfxVertexBufferView* vbViews[] = {&vbView};
		context.SetVertexBuffers(
			0,
			1,
			vbViews);
			
		context.DrawInstanced(3, 1, 0, 0);

		EndRender();
	}
	
} // namespace APP001
} // namespace SI
