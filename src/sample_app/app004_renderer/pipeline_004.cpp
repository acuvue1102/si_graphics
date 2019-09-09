
#include "pipeline_004.h"

#include <string>
#include <vector>
#include <si_base/core/core.h>
#include <si_app/file/path_storage.h>
#include <si_base/math/math.h>
#include <si_base/file/file_utility.h>
#include <si_base/container/array.h>
//#include <si_base/renderer/model_reader.h>
//#include <si_base/renderer/renderer.h>
#include <si_base/renderer/gltf_loader.h>

namespace SI
{
namespace APP004
{
	namespace
	{
		struct PosUvVertex
		{
			float m_x;
			float m_y;
			float m_z;

			float m_u;
			float m_v;
		};
		
		static const PosUvVertex kVertexData[] = 
		{
			{ -1.0f,  1.0f, 0.5f ,  0.0f, 0.0f },
			{  1.0f, -1.0f, 0.5f ,  1.0f, 1.0f },
			{ -1.0f, -1.0f, 0.5f ,  0.0f, 1.0f },

			{ -1.0f,  1.0f, 0.5f ,  0.0f, 0.0f },
			{  1.0f,  1.0f, 0.5f ,  1.0f, 0.0f },
			{  1.0f, -1.0f, 0.5f ,  1.0f, 1.0f },
		};
	}

	struct Pipeline::TextureShaderConstant
	{
		float m_vertexScale[2];
		float m_uvScale[2];
	};

	//////////////////////////////////////////////////////////
	
	Pipeline::Pipeline(int observerSortKey)
		: PipelineBase(observerSortKey)
		, m_textureConstant(nullptr)
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
		RootScenePtr s;
		GltfLoader loader;
		s = loader.Load("C:\\Users\\keiji\\Documents\\si_graphics\\asset\\model\\cornel_box.gltf");
		//s = loader.Load("C:\\Users\\keiji\\Documents\\si_graphics\\src\\external\\gltfsdk_original\\GLTFSDK.Test\\Resources\\gltf\\Cube.gltf");

		//ModelReader modelReader;
		//modelReader.Read(m_modelInstance, "asset\\model\\cornel_box.json");

		//m_renderer.Add(m_modelInstance);
		
		// textureシェーダのセットアップ.
		{
			if(m_textureVS.LoadAndCompile("asset\\shader\\texture.hlsl") != 0) return -1;
			if(m_texturePS.LoadAndCompile("asset\\shader\\texture.hlsl") != 0) return -1;
		}

		// コンスタントバッファのセットアップ
		{
			m_constantBuffer.InitializeAsConstant("textureConstant", sizeof(TextureShaderConstant));
			m_textureConstant = static_cast<TextureShaderConstant*>(m_constantBuffer.GetMapPtr());
			m_textureConstant->m_vertexScale[0] = 0.8f;
			m_textureConstant->m_vertexScale[1] = 0.8f;
			m_textureConstant->m_uvScale[0]     = 1.0f;
			m_textureConstant->m_uvScale[1]     = 1.0f;
		}
		
		// render targetのセットアップ.
		{
			m_rt.InitializeAs2DRt("RT0", 1024, 1024, GfxFormat::R8G8B8A8_Unorm, GfxColorRGBA(0.15f,0.15f,0.15f,1.0f));
		}
		
		// depth render targetのセットアップ.
		{
			m_depth.InitializeAs2DDepthRt("depth", 1024, 1024, 1.0f);
		}
		
		// samplerのセットアップ
		{
			GfxSamplerDesc samplerDesc;
			m_sampler.Initialize(samplerDesc);
		}

		// root signatureのセットアップ
		{
			GfxRootSignatureDescEx rootSignatureDesc(2, 1);

			GfxDescriptorHeapTableEx& table0 = rootSignatureDesc.GetTable(0);
			table0.ReserveRanges(1);
			table0.GetRange(0).Set(GfxDescriptorRangeType::Srv, 1, 0, GfxDescriptorRangeFlag::Volatile);

			GfxDescriptorHeapTableEx& table1 = rootSignatureDesc.GetTable(1);
			table1.ReserveRanges(1);
			table1.GetRange(0).Set(GfxDescriptorRangeType::Sampler, 1, 0, GfxDescriptorRangeFlag::DescriptorsVolatile);

			GfxRootDescriptor& rootDescriptor0 = rootSignatureDesc.GetRootDescriptor(0);
			rootDescriptor0.m_type = GfxRootDescriptorType::CBV;
			rootDescriptor0.m_shaderRegisterIndex = 1;
			rootDescriptor0.m_flags = GfxRootDescriptorFlag::DataVolatile;

			rootSignatureDesc.SetName("rootSig");
			rootSignatureDesc.SetFlags(GfxRootSignatureFlag::AllowInputAssemblerInputLayout);
			m_rootSignature.Initialize(rootSignatureDesc);
		}

		// PSOのセットアップ.
		{
			static const GfxInputElement kPosUvVertexElements[] =
			{
				{"POSITION",  0, GfxFormat::R32G32B32_Float,  0, 0},
				{"TEXCOORD",  0, GfxFormat::R32G32_Float,     0, 12},
			};

			GfxGraphicsStateDesc stateDesc;
			stateDesc.m_cullMode           = GfxCullMode::Back;
			stateDesc.m_depthFunc          = GfxComparisonFunc::LessEqual;
			stateDesc.m_depthWriteMask     = GfxDepthWriteMask::All;		
			stateDesc.m_vertexShader       = &m_textureVS;
			stateDesc.m_pixelShader        = &m_texturePS;
			stateDesc.m_inputElements      = kPosUvVertexElements;
			stateDesc.m_inputElementCount  = (int)ArraySize(kPosUvVertexElements);
			stateDesc.m_rootSignature      = &m_rootSignature.Get();
			stateDesc.m_rtvFormats[0]      = GfxFormat::R8G8B8A8_Unorm;
			stateDesc.m_dsvFormat          = GfxFormat::Unknown;
			stateDesc.m_depthEnable        = false;
			m_graphicsState = m_device.CreateGraphicsState(stateDesc);
		}

		return 0;
	}
	
	void Pipeline::SetView(Vfloat4x4_arg view)
	{
		Vfloat4x4 projMatrix = Math::Perspective(1.6f, 0.9f, 0.5f, 1000.0f);
		
		m_view = view;
		m_proj = projMatrix;
	}

	int Pipeline::OnTerminate()
	{
		m_swapChain.Wait();
		
		m_device.ReleaseGraphicsState(m_graphicsState);
		
		m_rootSignature.Terminate();
		
		m_sampler.Terminate();

		m_depth.TerminateDepthRt();
		
		m_rt.TerminateRt();

		m_constantBuffer.TerminateAsConstant();
		m_texturePS.Release();
		m_textureVS.Release();
		
		//m_modelInstance.reset();

		PipelineBase::OnTerminate();

		return 0;
	}
	
	void Pipeline::OnUpdate(const App& app, const AppUpdateInfo&)
	{
		//m_renderer.Update();
	}
	
	void Pipeline::OnRender(const App& app, const AppUpdateInfo&)
	{
		BeginRender();
		
		GfxGraphicsContext& context = m_contextManager.GetGraphicsContext(0);

		context.ResourceBarrier(
			m_depth,
			GfxResourceState::DepthWrite);

		context.ResourceBarrier(
			m_rt,
			GfxResourceState::RenderTarget);
		
		context.SetRenderTarget(m_rt, m_depth);

		GfxViewport viewport0 = GfxViewport(0.0f, 0.0f, (float)m_rt.GetWidth(), (float)m_rt.GetHeight());
		GfxScissor scissor0  = GfxScissor(0, 0, m_rt.GetWidth(), m_rt.GetHeight());
		context.SetViewports(1, &viewport0);
		context.SetScissors(1, &scissor0);

		context.ClearRenderTarget(m_rt);
		context.ClearDepthStencilTarget(m_depth);

		{
			//m_renderer.SetViewMatrix(m_view);
			//m_renderer.SetProjectionMatrix(m_proj);

			//RendererGraphicsStateDesc renderDesc;
			//renderDesc.m_rtvFormats[0] = GfxFormat::R8G8B8A8_Unorm;
			//renderDesc.m_dsvFormat = GfxFormat::D32_Float;
			//renderDesc.m_depthEnable = true;
			//renderDesc.m_depthWriteMask = GfxDepthWriteMask::All;
			//m_renderer.Render(
			//	context,
			//	RendererDrawStageType::RendererDrawStageType_Opaque,
			//	renderDesc);
		}
		
		context.ResourceBarrier(m_rt, GfxResourceState::PixelShaderResource);		
		context.ResourceBarrier(m_depth, GfxResourceState::GenericRead);
		
		// 箱を描いたレンダーターゲットテクスチャをスワップチェインに描画する
		{
			GfxTestureEx_SwapChain& swapChainTexture = m_swapChain.GetTexture();
			GfxViewport viewport1(0.0f, 0.0f, (float)swapChainTexture.GetWidth(), (float)swapChainTexture.GetHeight());
			GfxScissor  scissor1(0, 0, swapChainTexture.GetWidth(), swapChainTexture.GetHeight());
			context.SetRenderTarget(swapChainTexture);
			context.SetViewports(1, &viewport1);
			context.SetScissors(1, &scissor1);
		
			context.SetPipelineState(m_graphicsState);
			context.SetGraphicsRootSignature(m_rootSignature);

			context.SetDynamicViewDescriptor(0, 0, m_rt);
			context.SetDynamicSamplerDescriptor(1, 0, m_sampler);
			context.SetGraphicsRootCBV(2, m_constantBuffer.Get());

			swapChainTexture.SetClearColor(GfxColorRGBA(0.0f, 0.2f, 0.4f, 1.0f));
			context.ClearRenderTarget(swapChainTexture);

			context.SetPrimitiveTopology(GfxPrimitiveTopology::TriangleList);

			context.SetDynamicVB(0, ArraySize(kVertexData), sizeof(kVertexData[0]), kVertexData);

			context.DrawInstanced(6, 1, 0, 0);
		}

		EndRender();
	}
	
} // namespace APP004
} // namespace SI
