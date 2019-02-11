
#include "pipeline_005.h"

#include <string>
#include <vector>
#include <si_base/core/core.h>
#include <si_app/file/path_storage.h>
#include <si_base/math/math.h>
#include "raytracing_in_one_weekend.h"

#define CPU_RATTRACING 0

namespace SI
{
namespace APP005
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
		
		std::vector<float> GenerateTextureData(uint32_t width, uint32_t height)
		{
			const uint32_t pixelSize = 4;
			const uint32_t rowPitch = width * pixelSize;
			const uint32_t textureSize = rowPitch * height;

			std::vector<float> data(textureSize);
			float* pData = &data[0];

			for (uint32_t n = 0; n < textureSize; n += pixelSize)
			{
				uint32_t x = n % rowPitch;
				uint32_t y = n / rowPitch;
				float u = ((float)x+0.5f)/(float)width;
				float v = ((float)y+0.5f)/(float)height;

				Vfloat3 color(u, v, 0);
				
				pData[n    ] = color.X();	// R
				pData[n + 1] = color.Y();	// G
				pData[n + 2] = color.Z();	// B
				pData[n + 3] = 1.0f;	    // A
			}

			return std::move(data);
		}
	}
	
	struct Pipeline::RaytracingShaderConstant
	{
		uint32_t m_width;
		uint32_t m_height;
	};
	
	struct Pipeline::TextureShaderConstant
	{
		float m_vertexScale[2];
		float m_uvScale[2];
	};

	//////////////////////////////////////////////////////////
	
	Pipeline::Pipeline(int observerSortKey)
		: PipelineBase(observerSortKey)
		, m_raytracingConstant(nullptr)
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
		// 四角形の頂点バッファのセットアップ.
		{
			m_quadVertexBuffer.InitializeAsVertex(
				"vertex", ArraySize(kVertexData), sizeof(kVertexData[0]), 0);
		}

		// textureシェーダのセットアップ.
		{
			std::string shaderPath = SI_PATH_STORAGE().GetExeDirPath();
			shaderPath += "shaders\\texture.hlsl";
			if(m_textureVS.LoadAndCompile(shaderPath.c_str()) != 0) return -1;
			if(m_texturePS.LoadAndCompile(shaderPath.c_str()) != 0) return -1;
		}
		
#if !CPU_RATTRACING
		{
			std::string shaderPath = SI_PATH_STORAGE().GetExeDirPath();
			shaderPath += "shaders\\raytracing_in_one_weekend.hlsl";
			if(m_raytracingCS.LoadAndCompile(shaderPath.c_str()) != 0) return -1;
		}
#endif

		// result textureのセットアップ.
		{
			m_resultTexture.InitializeAs2DUav(
				"copyedTexture",
#if 1
				1980,
				1080,
#else
				320,
				180,
#endif
				GfxFormat::R32G32B32A32_Float);
		}

		// コンスタントバッファのセットアップ
		{			
			m_constantBuffers.InitializeAsConstant("textureConstant", sizeof(TextureShaderConstant));
			m_textureConstant = static_cast<TextureShaderConstant*>(m_constantBuffers.GetMapPtr());
			m_textureConstant->m_vertexScale[0] = 1;//0.8f;// * ((float)info.m_height / (float)info.m_width);
			m_textureConstant->m_vertexScale[1] = 1;//0.8f;
			m_textureConstant->m_uvScale[0]     = 1.0f;
			m_textureConstant->m_uvScale[1]     = 1.0f;
		}

		// Raytracing コンスタントバッファのセットアップ
		{			
			m_raytracingConstantBuffers.InitializeAsConstant("raytracingConstant", sizeof(RaytracingShaderConstant));
			m_raytracingConstant = static_cast<RaytracingShaderConstant*>(m_raytracingConstantBuffers.GetMapPtr());
			m_raytracingConstant->m_width  = m_resultTexture.GetWidth();
			m_raytracingConstant->m_height = m_resultTexture.GetHeight();
		}
		
		// samplerのセットアップ
		{
			GfxSamplerDesc samplerDesc;
			m_sampler.Initialize(samplerDesc);
		}

		// root signatureのセットアップ
		{
			GfxRootSignatureDescEx rootSignatureDesc;
			rootSignatureDesc.ReserveTables(2);

			GfxDescriptorHeapTableEx& table0 = rootSignatureDesc.GetTable(0);
			table0.ReserveRanges(2);
			table0.GetRange(0).Set(GfxDescriptorRangeType::Srv, 1, 0, GfxDescriptorRangeFlag::Volatile);
			table0.GetRange(1).Set(GfxDescriptorRangeType::Cbv, 1, 1, GfxDescriptorRangeFlag::Volatile);

			GfxDescriptorHeapTableEx& table1 = rootSignatureDesc.GetTable(1);
			table1.ReserveRanges(1);
			table1.GetRange(0).Set(GfxDescriptorRangeType::Sampler, 1, 0, GfxDescriptorRangeFlag::DescriptorsVolatile);

			rootSignatureDesc.SetName("rootSig");
			m_rootSignatures.Initialize(rootSignatureDesc);
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
			stateDesc.m_rtvFormats[0]      = GfxFormat::R8G8B8A8_Unorm;
			stateDesc.m_dsvFormat          = GfxFormat::D32_Float;
			stateDesc.m_depthEnable        = true;
			stateDesc.m_depthFunc          = GfxComparisonFunc::LessEqual;
			stateDesc.m_depthWriteMask     = GfxDepthWriteMask::All;		
			stateDesc.m_vertexShader       = &m_textureVS;
			stateDesc.m_pixelShader        = &m_texturePS;
			stateDesc.m_inputElements      = kPosUvVertexElements;
			stateDesc.m_inputElementCount  = (int)ArraySize(kPosUvVertexElements);
			stateDesc.m_rootSignature      = &m_rootSignatures.GetRootSignature();
			stateDesc.m_rtvFormats[0]      = GfxFormat::R8G8B8A8_Unorm;
			stateDesc.m_dsvFormat          = GfxFormat::Unknown;
			stateDesc.m_depthEnable        = false;
			m_graphicsStates = m_device.CreateGraphicsState(stateDesc);
		}
		
#if CPU_RATTRACING
		auto textureData = GenerateRaytracingTextureData(m_resultTexture.GetWidth(), m_resultTexture.GetHeight());
#else
		// compute root signatureのセットアップ
		{
			GfxRootSignatureDescEx computeRootSignatureDesc;
			computeRootSignatureDesc.ReserveTables(1);

			GfxDescriptorHeapTableEx& table0 = computeRootSignatureDesc.GetTable(0);
			table0.ReserveRanges(2);
			table0.GetRange(0).Set(GfxDescriptorRangeType::Cbv, 1, 0, GfxDescriptorRangeFlag::Volatile);
			table0.GetRange(1).Set(GfxDescriptorRangeType::Uav, 1, 0, GfxDescriptorRangeFlag::Volatile);

			computeRootSignatureDesc.SetName("computeRootSig");
			m_computeRootSignatures.Initialize(computeRootSignatureDesc);
		}

		// compute PSOのセットアップ.
		{
			GfxComputeStateDesc computeStateDesc;
			computeStateDesc.m_rootSignature = &m_computeRootSignatures.GetRootSignature();
			computeStateDesc.m_computeShader = &m_raytracingCS;
			m_computeStates = m_device.CreateComputeState(computeStateDesc);
		}
#endif

		// commandList経由でリソースのデータをアップロードする.
		BeginRender();
		{
			GfxGraphicsContext& context = m_contextManager.GetGraphicsContext(0);
			context.UploadBuffer(m_device, m_quadVertexBuffer, kVertexData, sizeof(kVertexData));
#if CPU_RATTRACING
			context.UploadTexture(m_device, m_resultTexture, &textureData[0], sizeof(textureData[0]) * textureData.size());
#endif
		}
		EndRender();

		return 0;
	}

	int Pipeline::OnTerminate()
	{
		m_swapChain.Wait();
		
		m_device.ReleaseComputeState(m_computeStates);
		m_device.ReleaseGraphicsState(m_graphicsStates);

		m_computeRootSignatures.Terminate();
		m_rootSignatures.Terminate();

		m_sampler.Terminate();
		
		m_raytracingConstant = nullptr;
		m_raytracingConstantBuffers.TerminateAsConstant();
		
		m_textureConstant = nullptr;
		m_constantBuffers.TerminateAsConstant();

		m_resultTexture.TerminateUav();

		m_texturePS.Release();
		m_textureVS.Release();

		m_raytracingCS.Release();

		m_quadVertexBuffer.TerminateAsVertex();

		PipelineBase::OnTerminate();

		return 0;
	}
	
	void Pipeline::OnUpdate(const App& app, const AppUpdateInfo&)
	{
	}
	
	void Pipeline::OnRender(const App& app, const AppUpdateInfo&)
	{
		BeginRender();
		
		GfxGraphicsContext& context = m_contextManager.GetGraphicsContext(0);

#if !CPU_RATTRACING
		static bool isFirst = true;
		if(isFirst)
		{
			context.ResourceBarrier(m_resultTexture, GfxResourceState::UnorderedAccess);

			{
				context.SetComputeRootSignature(m_computeRootSignatures);
				context.SetPipelineState(m_computeStates);

				context.SetDynamicViewDescriptor(0, 0, m_raytracingConstantBuffers);
				context.SetDynamicViewDescriptor(0, 1, m_resultTexture);

				uint32_t threadGroupCountX = m_resultTexture.GetWidth()  / 4;
				uint32_t threadGroupCountY = m_resultTexture.GetHeight() / 4;
				context.Dispatch(threadGroupCountX, threadGroupCountY);
			}

			context.ResourceBarrier(m_resultTexture, GfxResourceState::PixelShaderResource);
		}
		isFirst=false;
#endif

		{
			GfxTestureEx_SwapChain& swapChainTexture = m_swapChain.GetTexture();
			GfxViewport viewport1(0.0f, 0.0f, (float)swapChainTexture.GetWidth(), (float)swapChainTexture.GetHeight());
			GfxScissor  scissor1(0, 0, swapChainTexture.GetWidth(), swapChainTexture.GetHeight());
			context.SetRenderTarget(swapChainTexture);
			context.SetViewports(1, &viewport1);
			context.SetScissors(1, &scissor1);
		
			context.SetGraphicsRootSignature(m_rootSignatures);
			context.SetPipelineState(m_graphicsStates);

			context.SetDynamicViewDescriptor(0, 0, m_resultTexture);
			context.SetDynamicViewDescriptor(0, 1, m_constantBuffers);
			context.SetDynamicSamplerDescriptor(1, 0, m_sampler);

			swapChainTexture.SetClearColor(GfxColorRGBA(0.0f, 0.2f, 0.4f, 1.0f));
			context.ClearRenderTarget(swapChainTexture);

			context.SetPrimitiveTopology(GfxPrimitiveTopology::TriangleList);

			context.SetVertexBuffer(0, m_quadVertexBuffer);
			
			context.DrawInstanced(6, 1, 0, 0);
		}

		EndRender();
	}
	
} // namespace APP003
} // namespace SI
