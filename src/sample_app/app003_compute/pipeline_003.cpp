
#include "pipeline_003.h"

#include <string>
#include <vector>
#include <si_base/core/core.h>
#include <si_app/file/path_storage.h>
#include <si_base/math/math.h>

namespace SI
{
namespace APP003
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

		std::vector<uint8_t> GenerateTextureData(uint32_t width, uint32_t height)
		{
			const uint32_t pixelSize = 4;
			const uint32_t rowPitch = width * pixelSize;
			const uint32_t cellPitch = rowPitch >> 3;		// The width of a cell in the checkboard texture.
			const uint32_t cellHeight = width >> 3;	// The height of a cell in the checkerboard texture.
			const uint32_t textureSize = rowPitch * height;

			std::vector<uint8_t> data(textureSize);
			uint8_t* pData = &data[0];

			for (uint32_t n = 0; n < textureSize; n += pixelSize)
			{
				uint32_t x = n % rowPitch;
				uint32_t y = n / rowPitch;
				uint32_t i = x / cellPitch;
				uint32_t j = y / cellHeight;

				if (i % 2 == j % 2)
				{
					pData[n] = 0x0;		// R
					pData[n + 1] = 0x0;	// G
					pData[n + 2] = 0x0;	// B
					pData[n + 3] = 0xff;	// A
				}
				else
				{
					pData[n] = 0xff;		// R
					pData[n + 1] = 0xff;	// G
					pData[n + 2] = 0xff;	// B
					pData[n + 3] = 0xff;	// A
				}
			}

			return data;
		}
	}
	
	struct Pipeline::CopyTextureShaderConstant
	{
		uint32_t m_width;
		uint32_t m_height;
	};
	
	struct Pipeline::TextureShaderConstant
	{
		float m_vertexScale;
		float m_uvScale;
	};

	//////////////////////////////////////////////////////////
	
	Pipeline::Pipeline(int observerSortKey)
		: PipelineBase(observerSortKey)
		, m_copyTextureConstant(nullptr)
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
				"vertex", sizeof(kVertexData), sizeof(kVertexData[0]), 0);
		}

		// textureシェーダのセットアップ.
		{
			std::string shaderPath = SI_PATH_STORAGE().GetExeDirPath();
			shaderPath += "shaders\\texture.hlsl";
			if(m_textureVS.LoadAndCompile(shaderPath.c_str()) != 0) return -1;
			if(m_texturePS.LoadAndCompile(shaderPath.c_str()) != 0) return -1;
		}

		// copyTextureシェーダのセットアップ.
		{
			std::string shaderPath = SI_PATH_STORAGE().GetExeDirPath();
			shaderPath += "shaders\\copy_texture_cs.hlsl";
			if(m_copyTextureCS.LoadAndCompile(shaderPath.c_str()) != 0) return -1;
		}

		// static textureのセットアップ.
		{
			m_texture.InitializeAs2DStatic("whiteBlack", 256, 256, GfxFormat::R8G8B8A8_Unorm);
		}

		// copyed textureのセットアップ.
		{
			m_copyedTexture.InitializeAs2DUav(
				"copyedTexture",
				m_texture.GetWidth(),
				m_texture.GetHeight(),
				m_texture.GetFormat());
		}

		// コンスタントバッファのセットアップ
		{			
			m_constantBuffers.InitializeAsConstant("textureConstant", sizeof(TextureShaderConstant));
			m_textureConstant = static_cast<TextureShaderConstant*>(m_constantBuffers.GetMapPtr());
			m_textureConstant->m_vertexScale = 0.8f;
			m_textureConstant->m_uvScale     = 1.0f;
		}

		// Copy コンスタントバッファのセットアップ
		{			
			m_copyConstantBuffers.InitializeAsConstant("copyTextureConstant", sizeof(CopyTextureShaderConstant));
			m_copyTextureConstant = static_cast<CopyTextureShaderConstant*>(m_copyConstantBuffers.GetMapPtr());
			m_copyTextureConstant->m_width  = m_texture.GetWidth();
			m_copyTextureConstant->m_height = m_texture.GetHeight();
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

		// compute root signatureのセットアップ
		{
			GfxRootSignatureDescEx computeRootSignatureDesc;
			computeRootSignatureDesc.ReserveTables(1);

			GfxDescriptorHeapTableEx& table0 = computeRootSignatureDesc.GetTable(0);
			table0.ReserveRanges(3);
			table0.GetRange(0).Set(GfxDescriptorRangeType::Srv, 1, 0, GfxDescriptorRangeFlag::Volatile);
			table0.GetRange(1).Set(GfxDescriptorRangeType::Cbv, 1, 0, GfxDescriptorRangeFlag::Volatile);
			table0.GetRange(2).Set(GfxDescriptorRangeType::Uav, 1, 0, GfxDescriptorRangeFlag::Volatile);

			computeRootSignatureDesc.SetName("computeRootSig");
			m_computeRootSignatures.Initialize(computeRootSignatureDesc);
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
		
		// compute PSOのセットアップ.
		{
			GfxComputeStateDesc computeStateDesc;
			computeStateDesc.m_rootSignature = &m_computeRootSignatures.GetRootSignature();
			computeStateDesc.m_computeShader = &m_copyTextureCS;
			m_computeStates = m_device.CreateComputeState(computeStateDesc);
		}

		// commandList経由でリソースのデータをアップロードする.
		BeginRender();
		{
			GfxGraphicsContext& context = m_contextManager.GetGraphicsContext(0);
			context.UploadBuffer(m_device, m_quadVertexBuffer, kVertexData, sizeof(kVertexData));
			
			std::vector<uint8_t> texData = GenerateTextureData(m_texture.GetWidth(), m_texture.GetHeight());
			context.UploadTexture(m_device, m_texture, &texData[0], texData.size());
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
		
		m_copyTextureConstant = nullptr;
		m_copyConstantBuffers.TerminateAsConstant();
		
		m_textureConstant = nullptr;
		m_constantBuffers.TerminateAsConstant();

		m_copyedTexture.TerminateUav();
		m_texture.TerminateStatic();

		m_texturePS.Release();
		m_textureVS.Release();

		m_copyTextureCS.Release();

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

		context.ResourceBarrier(m_copyedTexture, GfxResourceState::UnorderedAccess);

		{
			context.SetComputeRootSignature(m_computeRootSignatures);
			context.SetPipelineState(m_computeStates);

			context.SetDynamicViewDescriptor(0, 0, m_texture);
			context.SetDynamicViewDescriptor(0, 1, m_copyConstantBuffers);
			context.SetDynamicViewDescriptor(0, 2, m_copyedTexture);

			uint32_t threadGroupCountX = m_texture.GetWidth()  / 8;
			uint32_t threadGroupCountY = m_texture.GetHeight() / 8;
			context.Dispatch(threadGroupCountX, threadGroupCountY);
		}

		context.ResourceBarrier(m_copyedTexture, GfxResourceState::PixelShaderResource);

		{
			GfxTestureEx_SwapChain& swapChainTexture = m_swapChain.GetTexture();
			GfxViewport viewport1(0.0f, 0.0f, (float)swapChainTexture.GetWidth(), (float)swapChainTexture.GetHeight());
			GfxScissor  scissor1(0, 0, swapChainTexture.GetWidth(), swapChainTexture.GetHeight());
			context.SetRenderTarget(swapChainTexture);
			context.SetViewports(1, &viewport1);
			context.SetScissors(1, &scissor1);
		
			context.SetGraphicsRootSignature(m_rootSignatures);
			context.SetPipelineState(m_graphicsStates);

			context.SetDynamicViewDescriptor(0, 0, m_copyedTexture);
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
	
} // namespace APP002
} // namespace SI
