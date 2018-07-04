﻿
#include "pipeline_002.h"

#include <string>
#include <vector>
#include <si_base/core/core.h>
#include <si_app/file/path_storage.h>
#include <si_base/math/math.h>

namespace SI
{
namespace APP002
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

		static const float kRtCleaColor[4] = {0.15f, 0.15f, 0.15f, 1.0f};
	}
	
	struct Pipeline::TextureShaderConstant
	{
		float m_vertexScale;
		float m_uvScale;
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
		// 四角形の頂点バッファのセットアップ.
		{
			GfxBufferDesc vertexBufferDesc;
			vertexBufferDesc.m_heapType = GfxHeapType::kDefault;
			vertexBufferDesc.m_bufferSizeInByte = sizeof(kVertexData);
			m_quadVertexBuffer = m_device.CreateBuffer(vertexBufferDesc);
		}

		// ボックスの頂点バッファのセットアップ.
		{
			GfxBufferDesc vertexBufferDesc;
			vertexBufferDesc.m_heapType = GfxHeapType::kDefault;
			vertexBufferDesc.m_bufferSizeInByte = sizeof(kBoxVertexData);
			m_boxVertexBuffer = m_device.CreateBuffer(vertexBufferDesc);
			
			GfxBufferDesc indexBufferDesc;
			indexBufferDesc.m_heapType = GfxHeapType::kDefault;
			indexBufferDesc.m_bufferSizeInByte = sizeof(kBoxIndexData);
			m_boxIndexBuffer = m_device.CreateBuffer(indexBufferDesc);
		}

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
			GfxBufferDesc lambertConstantBufferDesc;
			lambertConstantBufferDesc.m_name = "lambertConstant";
			lambertConstantBufferDesc.m_heapType = GfxHeapType::kUpload;
			lambertConstantBufferDesc.m_bufferSizeInByte = (sizeof(LambertShaderConstant) + 255) & ~255; // should be multiple of 256 bytes.
			lambertConstantBufferDesc.m_resourceStates = GfxResourceState::kGenericRead;
			m_constantBuffers[0] = m_device.CreateBuffer(lambertConstantBufferDesc);
			m_lambertConstant = static_cast<LambertShaderConstant*>(m_constantBuffers[0].Map());
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
			
			GfxBufferDesc textureConstantBufferDesc;
			textureConstantBufferDesc.m_name = "textureConstant";
			textureConstantBufferDesc.m_heapType = GfxHeapType::kUpload;
			textureConstantBufferDesc.m_bufferSizeInByte = (sizeof(TextureShaderConstant) + 255) & ~255; // should be multiple of 256 bytes.
			textureConstantBufferDesc.m_resourceStates = GfxResourceState::kGenericRead;
			m_constantBuffers[1] = m_device.CreateBuffer(textureConstantBufferDesc);
			m_textureConstant = static_cast<TextureShaderConstant*>(m_constantBuffers[1].Map());
			m_textureConstant->m_vertexScale = 0.8f;
			m_textureConstant->m_uvScale     = 1.0f;
		}

		// static textureのセットアップ.
		{
			GfxTextureDesc textureDesc;
			textureDesc.m_width     = 256;
			textureDesc.m_height    = 256;
			textureDesc.m_mipLevels = 1;
			textureDesc.m_format    = GfxFormat::kR8G8B8A8_Unorm;
			textureDesc.m_dimension = GfxDimension::kTexture2D;
			textureDesc.m_name      = "whiteBlack";
			m_texture = m_device.CreateTexture(textureDesc);
		}
		
		// render targetのセットアップ.
		{
			//GfxTextureDesc rtDesc;
			//rtDesc.m_width     = 1024;
			//rtDesc.m_height    = 1024;
			//rtDesc.m_mipLevels = 1;
			//rtDesc.m_format    = GfxFormat::kR8G8B8A8_Unorm;
			//rtDesc.m_dimension = GfxDimension::kTexture2D;
			//rtDesc.m_resourceStates = GfxResourceState::kRenderTarget;
			//rtDesc.m_resourceFlags = GfxResourceFlag::kAllowRenderTarget;
			//rtDesc.m_heapType  = GfxHeapType::kDefault;
			//rtDesc.m_name      = "RT0";
			//rtDesc.m_clearColor[0] = kRtCleaColor[0];
			//rtDesc.m_clearColor[1] = kRtCleaColor[1];
			//rtDesc.m_clearColor[2] = kRtCleaColor[2];
			//rtDesc.m_clearColor[3] = kRtCleaColor[3];
			//m_rt = m_device.CreateTexture(rtDesc);
		
			//GfxDescriptorHeapDesc rtvHeapDesc;
			//rtvHeapDesc.m_type = GfxDescriptorHeapType::kRtv;
			//rtvHeapDesc.m_descriptorCount = 1;
			//rtvHeapDesc.m_flag = GfxDescriptorHeapFlag::kNone;
			//m_rtvHeap = m_device.CreateDescriptorHeap(rtvHeapDesc);

			//GfxRenderTargetViewDesc rtvDesc;
			//m_device.CreateRenderTargetView(m_rtvHeap, 0, m_rt, rtvDesc);
			m_rt.InitializeAs2DRt("RT0", 1024, 1024, GfxFormat::kR8G8B8A8_Unorm, GfxColorRGBA(kRtCleaColor));
		}
		
		// depth render targetのセットアップ.
		{
			//GfxTextureDesc depthDesc;
			//depthDesc.m_width          = 1024;
			//depthDesc.m_height         = 1024;
			//depthDesc.m_mipLevels      = 1;
			//depthDesc.m_format         = GfxFormat::kR32_Typeless;
			//depthDesc.m_dimension      = GfxDimension::kTexture2D;
			//depthDesc.m_resourceStates = GfxResourceState::kGenericRead;
			//depthDesc.m_resourceFlags  = GfxResourceFlag::kAllowDepthStencil;
			//depthDesc.m_heapType       = GfxHeapType::kDefault;
			//depthDesc.m_name           = "Depth";
			//depthDesc.m_clearDepth     = 1.0f;
			//m_depth = m_device.CreateTexture(depthDesc);
		
			//GfxDescriptorHeapDesc dsvHeapDesc;
			//dsvHeapDesc.m_type = GfxDescriptorHeapType::kDsv;
			//dsvHeapDesc.m_descriptorCount = 1;
			//dsvHeapDesc.m_flag = GfxDescriptorHeapFlag::kNone;
			//m_dsvHeap = m_device.CreateDescriptorHeap(dsvHeapDesc);

			//GfxDepthStencilViewDesc dsvDesc;
			//m_device.CreateDepthStencilView(m_dsvHeap, 0, m_depth, dsvDesc);

			m_depth.InitializeAs2DDepthRt("depth", 1024, 1024, 1.0f);
		}

		// cbv/srv/uav Heapのセットアップ
		{
			GfxDescriptorHeapDesc cbvSrvUavHeapDesc;
			cbvSrvUavHeapDesc.m_type = GfxDescriptorHeapType::kCbvSrvUav;
			cbvSrvUavHeapDesc.m_descriptorCount = 2;
			cbvSrvUavHeapDesc.m_flag = GfxDescriptorHeapFlag::kShaderVisible;

			GfxShaderResourceViewDesc srvDesc;
			srvDesc.m_format = GfxFormat::kR8G8B8A8_Unorm;
			srvDesc.m_miplevels = 1;
			srvDesc.m_srvDimension = GfxDimension::kTexture2D;

			GfxConstantBufferViewDesc cbvDesc;

			// 0番目
			cbvDesc.m_buffer = &m_constantBuffers[0];
			m_cbvSrvUavHeaps[0] = m_device.CreateDescriptorHeap(cbvSrvUavHeapDesc);
			m_device.CreateShaderResourceView(m_cbvSrvUavHeaps[0], 0, m_texture, srvDesc);
			m_device.CreateConstantBufferView(m_cbvSrvUavHeaps[0], 1, cbvDesc);

			// 1番目
			cbvDesc.m_buffer = &m_constantBuffers[1];
			m_cbvSrvUavHeaps[1] = m_device.CreateDescriptorHeap(cbvSrvUavHeapDesc);
			GfxTexture rtTex = m_rt.GetTexture();
			m_device.CreateShaderResourceView(m_cbvSrvUavHeaps[1], 0, rtTex, srvDesc);
			m_device.CreateConstantBufferView(m_cbvSrvUavHeaps[1], 1, cbvDesc);
		}
		
		// samplerのセットアップ
		{
			GfxDescriptorHeapDesc samplerHeapDesc;
			samplerHeapDesc.m_type = GfxDescriptorHeapType::kSampler;
			samplerHeapDesc.m_descriptorCount = 1;
			samplerHeapDesc.m_flag = GfxDescriptorHeapFlag::kShaderVisible;
			m_samplerHeaps[0] = m_device.CreateDescriptorHeap(samplerHeapDesc);
			m_samplerHeaps[1] = m_device.CreateDescriptorHeap(samplerHeapDesc);
			
			GfxSamplerDesc samplerDesc;
			m_device.CreateSampler(m_samplerHeaps[0], 0, samplerDesc);
			m_device.CreateSampler(m_samplerHeaps[1], 0, samplerDesc);
		}

		// root signatureのセットアップ
		{
			GfxRootSignatureDescEx rootSignatureDesc;
			rootSignatureDesc.ReserveTables(2);

			GfxDescriptorHeapTableEx& table0 = rootSignatureDesc[0];
			table0.ReserveRanges(2);
			table0.GetRange(0).Set(GfxDescriptorRangeType::kSrv, 1, 0, GfxDescriptorRangeFlag::kDynamic);
			table0.GetRange(1).Set(GfxDescriptorRangeType::kCbv, 1, 1);

			GfxDescriptorHeapTableEx& table1 = rootSignatureDesc[1];
			table1.ReserveRanges(1);
			table1.GetRange(0).Set(GfxDescriptorRangeType::kSampler, 1, 0);

			m_rootSignatures[0] = m_device.CreateRootSignature(rootSignatureDesc);
			m_rootSignatures[1] = m_device.CreateRootSignature(rootSignatureDesc);
		}

		// PSOのセットアップ.
		{
			static const GfxInputElement kPosNormalUvVertexElements[] =
			{
				{"POSITION",  0, GfxFormat::kR32G32B32_Float,  0, 0},
				{"NORMAL",    0, GfxFormat::kR32G32B32_Float,  0, 12},
				{"TEXCOORD",  0, GfxFormat::kR32G32_Float,     0, 24},
			};

			static const GfxInputElement kPosUvVertexElements[] =
			{
				{"POSITION",  0, GfxFormat::kR32G32B32_Float,  0, 0},
				{"TEXCOORD",  0, GfxFormat::kR32G32_Float,     0, 12},
			};

			GfxGraphicsStateDesc stateDesc;
			stateDesc.m_cullMode           = GfxCullMode::kBack;
			stateDesc.m_inputElements      = kPosNormalUvVertexElements;
			stateDesc.m_inputElementCount  = (int)ArraySize(kPosNormalUvVertexElements);
			stateDesc.m_rootSignature      = &m_rootSignatures[0];
			stateDesc.m_vertexShader       = &m_lambertVS;
			stateDesc.m_pixelShader        = &m_lambertPS;
			stateDesc.m_rtvFormats[0]      = GfxFormat::kR8G8B8A8_Unorm;
			stateDesc.m_dsvFormat          = GfxFormat::kD32_Float;
			stateDesc.m_depthEnable        = true;
			stateDesc.m_depthFunc          = GfxComparisonFunc::kLessEqual;
			stateDesc.m_depthWriteMask     = GfxDepthWriteMask::kAll;
			m_graphicsStates[0] = m_device.CreateGraphicsState(stateDesc);
		
			stateDesc.m_vertexShader       = &m_textureVS;
			stateDesc.m_pixelShader        = &m_texturePS;
			stateDesc.m_inputElements      = kPosUvVertexElements;
			stateDesc.m_inputElementCount  = (int)ArraySize(kPosUvVertexElements);
			stateDesc.m_rootSignature      = &m_rootSignatures[1];
			stateDesc.m_rtvFormats[0]      = GfxFormat::kR8G8B8A8_Unorm;
			stateDesc.m_dsvFormat          = GfxFormat::kUnknown;
			stateDesc.m_depthEnable        = false;
			m_graphicsStates[1] = m_device.CreateGraphicsState(stateDesc);
		}

		// commandList経由でリソースのデータをアップロードする.
		BeginRender();
		{
			GfxGraphicsContext& context = m_contextManager.GetGraphicsContext(0);
			context.UploadBuffer(m_device, m_quadVertexBuffer, kVertexData, sizeof(kVertexData));
			context.UploadBuffer(m_device, m_boxVertexBuffer, kBoxVertexData, sizeof(kBoxVertexData));
			context.UploadBuffer(m_device, m_boxIndexBuffer, kBoxIndexData, sizeof(kBoxIndexData));

			std::vector<uint8_t> texData = GenerateTextureData(m_texture.GetWidth(), m_texture.GetHeight());
			context.UploadTexture(m_device, m_texture, &texData[0], texData.size());
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
		m_swapChain.Flip(); // wait previous frame.
		
		m_device.ReleaseGraphicsState(m_graphicsStates[1]);
		m_device.ReleaseGraphicsState(m_graphicsStates[0]);
		
		m_device.ReleaseRootSignature(m_rootSignatures[1]);
		m_device.ReleaseRootSignature(m_rootSignatures[0]);
		
		m_device.ReleaseDescriptorHeap(m_samplerHeaps[1]);
		m_device.ReleaseDescriptorHeap(m_samplerHeaps[0]);
		
		m_device.ReleaseDescriptorHeap(m_cbvSrvUavHeaps[1]);
		m_device.ReleaseDescriptorHeap(m_cbvSrvUavHeaps[0]);

		//m_device.ReleaseDescriptorHeap(m_dsvHeap);
		//m_device.ReleaseTexture(m_depth);
		m_depth.TerminateDepthRt();
		
		//m_device.ReleaseDescriptorHeap(m_rtvHeap);
		//m_device.ReleaseTexture(m_rt);
		m_rt.TerminateRt();
		m_device.ReleaseTexture(m_texture);
		
		m_device.ReleaseBuffer(m_constantBuffers[1]);
		m_device.ReleaseBuffer(m_constantBuffers[0]);

		m_lambertPS.Release();
		m_lambertVS.Release();

		m_texturePS.Release();
		m_textureVS.Release();

		m_device.ReleaseBuffer(m_boxIndexBuffer);
		m_device.ReleaseBuffer(m_boxVertexBuffer);

		m_device.ReleaseBuffer(m_quadVertexBuffer);

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
			GfxResourceState::kDepthWrite);

		context.ResourceBarrier(
			m_rt,
			GfxResourceState::kRenderTarget);

		// 箱をレンダーターゲットに対して描く
		{
			context.SetPipelineState(m_graphicsStates[0]);
			context.SetGraphicsRootSignature(m_rootSignatures[0]);

			GfxDescriptorHeap* heaps[] = {&m_cbvSrvUavHeaps[0], &m_samplerHeaps[0]};
			context.SetDescriptorHeaps((uint32_t)ArraySize(heaps), heaps);
			context.SetGraphicsDescriptorTable(0, m_cbvSrvUavHeaps[0].GetGpuDescriptor(0));
			context.SetGraphicsDescriptorTable(1, m_samplerHeaps[0].GetGpuDescriptor(0));
		
			context.SetRenderTarget(m_rt, m_depth);

			GfxViewport viewport0 = GfxViewport(0.0f, 0.0f, (float)m_rt.GetWidth(), (float)m_rt.GetHeight());
			GfxScissor scissor0  = GfxScissor(0, 0, m_rt.GetWidth(), m_rt.GetHeight());
			context.SetViewports(1, &viewport0);
			context.SetScissors(1, &scissor0);

			context.ClearRenderTarget(m_rt);
			context.ClearDepthStencilTarget(m_depth);

			context.SetPrimitiveTopology(GfxPrimitiveTopology::kTriangleList);

			GfxVertexBufferView vertexBufferView(m_boxVertexBuffer, m_boxVertexBuffer.GetSize(), sizeof(PosNormalUvVertex));
			GfxVertexBufferView* vertexBufferViews[] = { &vertexBufferView };
			context.SetVertexBuffers(0, 1, vertexBufferViews);

			GfxIndexBufferView indexBufferView(m_boxIndexBuffer, GfxFormat::kR16_Uint, m_boxIndexBuffer.GetSize());
			context.SetIndexBuffer(&indexBufferView);
			
			context.DrawIndexedInstanced((uint32_t)ArraySize(kBoxIndexData), 8);
		}
		
		context.ResourceBarrier(m_rt, GfxResourceState::kPixelShaderResource);		
		context.ResourceBarrier(m_depth, GfxResourceState::kGenericRead);
		
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

			GfxDescriptorHeap* heapsOut[] = {&m_cbvSrvUavHeaps[1], &m_samplerHeaps[1]};
			context.SetDescriptorHeaps((uint32_t)ArraySize(heapsOut), heapsOut);
			context.SetGraphicsDescriptorTable(0, m_cbvSrvUavHeaps[1].GetGpuDescriptor(0));
			context.SetGraphicsDescriptorTable(1, m_samplerHeaps[1].GetGpuDescriptor(0));

			swapChainTexture.SetClearColor(GfxColorRGBA(0.0f, 0.2f, 0.4f, 1.0f));
			context.ClearRenderTarget(swapChainTexture);

			context.SetPrimitiveTopology(GfxPrimitiveTopology::kTriangleList);

			GfxVertexBufferView vertexBufferView(m_quadVertexBuffer, m_quadVertexBuffer.GetSize(), sizeof(PosUvVertex));
			GfxVertexBufferView* vertexBufferViews[] = {&vertexBufferView};
			context.SetVertexBuffers(0, 1, vertexBufferViews);
			
			context.DrawInstanced(6, 1, 0, 0);
		}

		EndRender();
	}
	
} // namespace APP002
} // namespace SI
