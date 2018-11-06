
#include "pipeline_004.h"

#include <string>
#include <vector>
#include <si_base/core/core.h>
#include <si_app/file/path_storage.h>
#include <si_base/math/math.h>
#include <si_app/file/file_utility.h>
#include <si_base/container/array.h>
#include <si_base/renderer/model_reader.h>

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
		
		struct PosNormalUvVertex
		{
			float m_x;
			float m_y;
			float m_z;

			float m_nx;
			float m_ny;
			float m_nz;

			float m_u;
			float m_v;
		};
		
		static const PosNormalUvVertex kBoxVertexData[] =
		{
			{ -0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,   0.0f, 0.0f },
			{ -0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,   0.0f, 1.0f },
			{  0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,   1.0f, 1.0f },
			{  0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,   1.0f, 0.0f },
											 
			{ -0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,   0.0f, 0.0f },
			{  0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,   1.0f, 0.0f },
			{  0.5f,  0.5f,  0.5f,    0.0f,  0.0f,  1.0f,   1.0f, 1.0f },
			{ -0.5f,  0.5f,  0.5f,    0.0f,  0.0f,  1.0f,   0.0f, 1.0f },
						
			{ -0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,   0.0f, 0.0f },
			{  0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,   1.0f, 0.0f },
			{  0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,   1.0f, 1.0f },
			{ -0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,   0.0f, 1.0f },
											 
			{ -0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,   0.0f, 0.0f },
			{ -0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,   0.0f, 1.0f },
			{  0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,   1.0f, 1.0f },
			{  0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,   1.0f, 0.0f },
						
			{ -0.5f, -0.5f, -0.5f,   -1.0f,  0.0f,  0.0f,   0.0f, 0.0f },
			{ -0.5f, -0.5f,  0.5f,   -1.0f,  0.0f,  0.0f,   0.0f, 1.0f },
			{ -0.5f,  0.5f,  0.5f,   -1.0f,  0.0f,  0.0f,   1.0f, 1.0f },
			{ -0.5f,  0.5f, -0.5f,   -1.0f,  0.0f,  0.0f,   1.0f, 0.0f },
											   
			{  0.5f, -0.5f, -0.5f,    1.0f,  0.0f,  0.0f,   0.0f, 0.0f },
			{  0.5f,  0.5f, -0.5f,    1.0f,  0.0f,  0.0f,   1.0f, 0.0f },
			{  0.5f,  0.5f,  0.5f,    1.0f,  0.0f,  0.0f,   1.0f, 1.0f },
			{  0.5f, -0.5f,  0.5f,    1.0f,  0.0f,  0.0f,   0.0f, 1.0f },
		};

		static const uint16_t kBoxIndexData[] =
		{
			 0, 1, 2,  0, 2, 3,
			 4, 5, 6,  4, 6, 7,
			 8, 9,10,  8,10,11,
			12,13,14, 12,14,15,
			16,17,18, 16,18,19,
			20,21,22, 20,22,23,
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
					pData[n] = 0x80;		// R
					pData[n + 1] = 0x80;	// G
					pData[n + 2] = 0x80;	// B
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
	
	struct Pipeline::TextureShaderConstant
	{
		float m_vertexScale[2];
		float m_uvScale[2];
	};

	struct PointLightInfo
	{
		Vfloat3    m_lightPos;
		Vfloat3    m_lightColor;
	};

	struct Pipeline::LambertShaderConstant
	{
		Vfloat4x4  m_world[8];
		Vfloat4x4  m_view;
		Vfloat4x4  m_viewProj;
		float      m_uvScale[2];
		float      m_padding[2];
		PointLightInfo m_pointLightInfo[4];
	};

	//////////////////////////////////////////////////////////
	
	Pipeline::Pipeline(int observerSortKey)
		: PipelineBase(observerSortKey)
		, m_textureConstant(nullptr)
		, m_lambertConstant(nullptr)
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
		//std::string modelPath = "asset\\model\\sphere.json";
		//ModelReader modelReader;
		//modelReader.Read(m_modelInstance, modelPath.c_str());

		// textureシェーダのセットアップ.
		{
			std::string shaderPath = SI_PATH_STORAGE().GetExeDirPath();
			shaderPath += "shaders\\texture.hlsl";
			if(m_textureVS.LoadAndCompile(shaderPath.c_str()) != 0) return -1;
			if(m_texturePS.LoadAndCompile(shaderPath.c_str()) != 0) return -1;
		}

		// lambertシェーダのセットアップ.
		{
			std::string shaderPath = SI_PATH_STORAGE().GetExeDirPath();
			shaderPath += "shaders\\lambert.hlsl";
			if(m_lambertVS.LoadAndCompile(shaderPath.c_str()) != 0) return -1;
			if(m_lambertPS.LoadAndCompile(shaderPath.c_str()) != 0) return -1;
		}

		// コンスタントバッファのセットアップ
		{
			m_constantBuffers->InitializeAsConstant("lambertConstant", sizeof(LambertShaderConstant));
			m_lambertConstant = static_cast<LambertShaderConstant*>(m_constantBuffers[0].GetMapPtr());
			memset(m_lambertConstant, 0, sizeof(LambertShaderConstant));

			for(size_t i=0; i<ArraySize(m_lambertConstant->m_world); ++i)
			{
				float t = (float)i / (float)ArraySize(m_lambertConstant->m_world);
				t *= 2.0f * kPi;

				Vfloat3 pos(1.5f*cosf(t), 0.0f, 1.5f*sinf(t));

				m_lambertConstant->m_world[i] = Math::Translate4x4(pos);
			}
			m_lambertConstant->m_view        = Vfloat4x4();
			m_lambertConstant->m_viewProj    = Vfloat4x4();
			m_lambertConstant->m_pointLightInfo[0].m_lightColor = Vfloat3(1.0f, 3.0f, 5.0f);
			m_lambertConstant->m_pointLightInfo[1].m_lightColor = Vfloat3(5.0f, 0.5f, 0.5f);
			m_lambertConstant->m_pointLightInfo[2].m_lightColor = Vfloat3(0.5f, 2.0f, 1.0f);
			m_lambertConstant->m_pointLightInfo[3].m_lightColor = Vfloat3(3.0f, 1.0f, 3.0f);
			m_lambertConstant->m_uvScale[0]  = 1.0f;
			m_lambertConstant->m_uvScale[1]  = 1.0f;
			
			m_constantBuffers[1].InitializeAsConstant("textureConstant", sizeof(TextureShaderConstant));
			m_textureConstant = static_cast<TextureShaderConstant*>(m_constantBuffers[1].GetMapPtr());
			m_textureConstant->m_vertexScale[0] = 0.8f;
			m_textureConstant->m_vertexScale[1] = 0.8f;
			m_textureConstant->m_uvScale[0]     = 1.0f;
			m_textureConstant->m_uvScale[1]     = 1.0f;
		}

		std::vector<uint8_t> texData;
		GfxDdsMetaData texMetaData;
		{
			char ddsFilePath[260];
			sprintf_s(ddsFilePath, "%stexture\\test_texture.dds", SI_PATH_STORAGE().GetAssetDirPath());
			int ret = FileUtility::Load(texData, ddsFilePath);
			SI_ASSERT(ret==0);

			m_texture.InitializeDDS("test_texture", &texData[0], texData.size(), texMetaData);
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
			GfxRootSignatureDescEx rootSignatureDesc;
			rootSignatureDesc.ReserveTables(2);

			GfxDescriptorHeapTableEx& table0 = rootSignatureDesc.GetTable(0);
			table0.ReserveRanges(2);
			table0.GetRange(0).Set(GfxDescriptorRangeType::Srv, 1, 0, GfxDescriptorRangeFlag::Volatile);
			table0.GetRange(1).Set(GfxDescriptorRangeType::Cbv, 1, 1, GfxDescriptorRangeFlag::Volatile);

			GfxDescriptorHeapTableEx& table1 = rootSignatureDesc.GetTable(1);
			table1.ReserveRanges(1);
			table1.GetRange(0).Set(GfxDescriptorRangeType::Sampler, 1, 0, GfxDescriptorRangeFlag::DescriptorsVolatile);

			rootSignatureDesc.SetName("rootSig0");
			m_rootSignatures[0].Initialize(rootSignatureDesc);

			rootSignatureDesc.SetName("rootSig1");
			m_rootSignatures[1].Initialize(rootSignatureDesc);
		}

		// PSOのセットアップ.
		{
			static const GfxInputElement kPosNormalUvVertexElements[] =
			{
				{"POSITION",  0, GfxFormat::R32G32B32_Float,  0, 0},
				{"NORMAL",    0, GfxFormat::R32G32B32_Float,  0, 12},
				{"TEXCOORD",  0, GfxFormat::R32G32_Float,     0, 24},
			};

			static const GfxInputElement kPosUvVertexElements[] =
			{
				{"POSITION",  0, GfxFormat::R32G32B32_Float,  0, 0},
				{"TEXCOORD",  0, GfxFormat::R32G32_Float,     0, 12},
			};

			GfxGraphicsStateDesc stateDesc;
			stateDesc.m_cullMode           = GfxCullMode::Back;
			stateDesc.m_inputElements      = kPosNormalUvVertexElements;
			stateDesc.m_inputElementCount  = (int)ArraySize(kPosNormalUvVertexElements);
			stateDesc.m_rootSignature      = &m_rootSignatures[0].GetRootSignature();
			stateDesc.m_vertexShader       = &m_lambertVS;
			stateDesc.m_pixelShader        = &m_lambertPS;
			stateDesc.m_rtvFormats[0]      = GfxFormat::R8G8B8A8_Unorm;
			stateDesc.m_dsvFormat          = GfxFormat::D32_Float;
			stateDesc.m_depthEnable        = true;
			stateDesc.m_depthFunc          = GfxComparisonFunc::LessEqual;
			stateDesc.m_depthWriteMask     = GfxDepthWriteMask::All;
			m_graphicsStates[0] = m_device.CreateGraphicsState(stateDesc);
		
			stateDesc.m_vertexShader       = &m_textureVS;
			stateDesc.m_pixelShader        = &m_texturePS;
			stateDesc.m_inputElements      = kPosUvVertexElements;
			stateDesc.m_inputElementCount  = (int)ArraySize(kPosUvVertexElements);
			stateDesc.m_rootSignature      = &m_rootSignatures[1].GetRootSignature();
			stateDesc.m_rtvFormats[0]      = GfxFormat::R8G8B8A8_Unorm;
			stateDesc.m_dsvFormat          = GfxFormat::Unknown;
			stateDesc.m_depthEnable        = false;
			m_graphicsStates[1] = m_device.CreateGraphicsState(stateDesc);
		}

		// commandList経由でリソースのデータをアップロードする.
		BeginRender();
		{
			GfxGraphicsContext& context = m_contextManager.GetGraphicsContext(0);
			
			context.UploadTexture(m_device, m_texture, texMetaData.m_image, texMetaData.m_imageSise);
		}
		EndRender();

		return 0;
	}
	
	void Pipeline::SetView(Vfloat4x4_arg view)
	{
		Vfloat4x4 projMatrix = Math::Perspective(1.6f, 0.9f, 0.5f, 10.0f);

		m_lambertConstant->m_view        = view;
		m_lambertConstant->m_viewProj    = view * projMatrix;
	}

	int Pipeline::OnTerminate()
	{
		m_swapChain.Wait();
		
		m_device.ReleaseGraphicsState(m_graphicsStates[1]);
		m_device.ReleaseGraphicsState(m_graphicsStates[0]);
		
		m_rootSignatures[1].Terminate();
		m_rootSignatures[0].Terminate();
		
		m_sampler.Terminate();

		m_depth.TerminateDepthRt();
		
		m_rt.TerminateRt();
		m_texture.TerminateStatic();

		m_constantBuffers[1].TerminateAsConstant();
		m_constantBuffers[0].TerminateAsConstant();

		m_lambertPS.Release();
		m_lambertVS.Release();

		m_texturePS.Release();
		m_textureVS.Release();

		PipelineBase::OnTerminate();

		return 0;
	}
	
	void Pipeline::OnUpdate(const App& app, const AppUpdateInfo&)
	{
		static float s_t = 0.0f;
		s_t += 0.03f;		
		m_lambertConstant->m_pointLightInfo[0].m_lightPos   = Vfloat3(4*cos(0.5f*s_t), 1.5f, 4*sin(0.5f*s_t));
		m_lambertConstant->m_pointLightInfo[1].m_lightPos   = Vfloat3(3*cos(0.6f*s_t), -0.5f, 3*sin(0.6f*s_t));
		m_lambertConstant->m_pointLightInfo[2].m_lightPos   = Vfloat3(3*sin(0.4f*s_t), 2*sin(0.3f*s_t), -1.5f);
		m_lambertConstant->m_pointLightInfo[3].m_lightPos   = Vfloat3(2*sin(0.4f*s_t), 3*cos(0.2f*s_t), 0.5f);
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

		// 箱をレンダーターゲットに対して描く
		{
			context.SetPipelineState(m_graphicsStates[0]);
			context.SetGraphicsRootSignature(m_rootSignatures[0]);
			
			context.SetDynamicViewDescriptor(0, 0, m_texture);
			context.SetDynamicViewDescriptor(0, 1, m_constantBuffers[0]);
			context.SetDynamicSamplerDescriptor(1, 0, m_sampler);
		
			context.SetRenderTarget(m_rt, m_depth);

			GfxViewport viewport0 = GfxViewport(0.0f, 0.0f, (float)m_rt.GetWidth(), (float)m_rt.GetHeight());
			GfxScissor scissor0  = GfxScissor(0, 0, m_rt.GetWidth(), m_rt.GetHeight());
			context.SetViewports(1, &viewport0);
			context.SetScissors(1, &scissor0);

			context.ClearRenderTarget(m_rt);
			context.ClearDepthStencilTarget(m_depth);

			context.SetPrimitiveTopology(GfxPrimitiveTopology::TriangleList);
			
			context.SetDynamicVB(0, ArraySize(kBoxVertexData), sizeof(kBoxVertexData[0]), kBoxVertexData);
			context.SetDynamicIB16(ArraySize(kBoxIndexData), kBoxIndexData);
			
			context.DrawIndexedInstanced((uint32_t)ArraySize(kBoxIndexData), 8);
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
		
			context.SetPipelineState(m_graphicsStates[1]);
			context.SetGraphicsRootSignature(m_rootSignatures[1]);

			context.SetDynamicViewDescriptor(0, 0, m_rt);
			context.SetDynamicViewDescriptor(0, 1, m_constantBuffers[1]);
			context.SetDynamicSamplerDescriptor(1, 0, m_sampler);

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
