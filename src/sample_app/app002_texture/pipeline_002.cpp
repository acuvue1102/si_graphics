
#include "pipeline_002.h"

#include <string>
#include <vector>
#include <si_base/core/core.h>
#include <si_app/file/path_storage.h>

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
					pData[n] = 0x00;		// R
					pData[n + 1] = 0x00;	// G
					pData[n + 2] = 0x00;	// B
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
		
		static const PosUvVertex kVertexData[] = 
		{
			{ -1.0f,  1.0f, 0.5f ,  0.0f, 0.0f },
			{  1.0f, -1.0f, 0.5f ,  1.0f, 1.0f },
			{ -1.0f, -1.0f, 0.5f ,  0.0f, 1.0f },

			{ -1.0f,  1.0f, 0.5f ,  0.0f, 0.0f },
			{  1.0f,  1.0f, 0.5f ,  1.0f, 0.0f },
			{  1.0f, -1.0f, 0.5f ,  1.0f, 1.0f },
		};

		static const float kRtCleaColor[4] = {0.5f, 0.2f, 0.2f, 1.0f};
	}

	struct Pipeline::ShaderConstant
	{
		float m_vertexScale;
		float m_uvScale;
	};

	//////////////////////////////////////////////////////////
	
	Pipeline::Pipeline(int observerSortKey)
		: PipelineBase(observerSortKey)
	{
		for(auto addr : m_constantAddrs)
		{
			addr = nullptr;
		}
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
		// 頂点バッファのセットアップ.
		{
			GfxBufferDesc vertexBufferDesc;
			vertexBufferDesc.m_heapType = GfxHeapType::kDefault;
			vertexBufferDesc.m_bufferSizeInByte = sizeof(kVertexData);
			m_vertexBuffer = m_device.CreateBuffer(vertexBufferDesc);
		}

		// シェーダのセットアップ.
		{
			std::string shaderPath = PathStorage::GetInstance()->GetExeDirPath();
			shaderPath += "shaders\\texture.hlsl";
			if(m_vertexShader.LoadAndCompile(shaderPath.c_str()) != 0) return -1;
			if(m_pixelShader. LoadAndCompile(shaderPath.c_str()) != 0) return -1;
		}

		// コンスタントバッファのセットアップ
		{
			GfxBufferDesc constantBufferDesc;
			constantBufferDesc.m_heapType = GfxHeapType::kUpload;
			constantBufferDesc.m_bufferSizeInByte = (sizeof(ShaderConstant) + 255) & ~255; // should be multiple of 256 bytes.
			m_constantBuffers[0] = m_device.CreateBuffer(constantBufferDesc);
			m_constantAddrs[0] = static_cast<ShaderConstant*>(m_constantBuffers[0].Map());
			m_constantAddrs[0]->m_vertexScale = 0.8f;
			m_constantAddrs[0]->m_uvScale     = 1.0f;

			m_constantBuffers[1] = m_device.CreateBuffer(constantBufferDesc);
			m_constantAddrs[1] = static_cast<ShaderConstant*>(m_constantBuffers[1].Map());
			m_constantAddrs[1]->m_vertexScale = 0.8f;
			m_constantAddrs[1]->m_uvScale     = 1.0f;
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
			GfxTextureDesc rtDesc;
			rtDesc.m_width     = 1024;
			rtDesc.m_height    = 1024;
			rtDesc.m_mipLevels = 1;
			rtDesc.m_format    = GfxFormat::kR8G8B8A8_Unorm;
			rtDesc.m_dimension = GfxDimension::kTexture2D;
			rtDesc.m_resourceStates = GfxResourceState::kRenderTarget;
			rtDesc.m_resourceFlags = GfxResourceFlag::kAllowRenderTarget;
			rtDesc.m_heapType  = GfxHeapType::kDefault;
			rtDesc.m_name      = "RT0";
			rtDesc.m_clearColor[0] = kRtCleaColor[0];
			rtDesc.m_clearColor[1] = kRtCleaColor[1];
			rtDesc.m_clearColor[2] = kRtCleaColor[2];
			rtDesc.m_clearColor[3] = kRtCleaColor[3];
			m_rt = m_device.CreateTexture(rtDesc);
		
			GfxDescriptorHeapDesc rtvHeapDesc;
			rtvHeapDesc.m_type = GfxDescriptorHeapType::kRtv;
			rtvHeapDesc.m_descriptorCount = 1;
			rtvHeapDesc.m_flag = GfxDescriptorHeapFlag::kNone;
			m_rtvHeap = m_device.CreateDescriptorHeap(rtvHeapDesc);

			GfxRenderTargetViewDesc rtvDesc;
			m_device.CreateRenderTargetView(m_rtvHeap, 0, m_rt, rtvDesc);
		}
		
		// depth render targetのセットアップ.
		{
			GfxTextureDesc depthDesc;
			depthDesc.m_width          = 1024;
			depthDesc.m_height         = 1024;
			depthDesc.m_mipLevels      = 1;
			depthDesc.m_format         = GfxFormat::kR32_Typeless;
			depthDesc.m_dimension      = GfxDimension::kTexture2D;
			depthDesc.m_resourceStates = GfxResourceState::kGenericRead;
			depthDesc.m_resourceFlags  = GfxResourceFlag::kAllowDepthStencil;
			depthDesc.m_heapType       = GfxHeapType::kDefault;
			depthDesc.m_name           = "Depth";
			depthDesc.m_clearDepth     = 1.0f;
			m_depth = m_device.CreateTexture(depthDesc);
		
			GfxDescriptorHeapDesc dsvHeapDesc;
			dsvHeapDesc.m_type = GfxDescriptorHeapType::kDsv;
			dsvHeapDesc.m_descriptorCount = 1;
			dsvHeapDesc.m_flag = GfxDescriptorHeapFlag::kNone;
			m_dsvHeap = m_device.CreateDescriptorHeap(dsvHeapDesc);

			GfxDepthStencilViewDesc dsvDesc;
			m_device.CreateDepthStencilView(m_dsvHeap, 0, m_depth, dsvDesc);
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
			m_device.CreateShaderResourceView(m_cbvSrvUavHeaps[1], 0, m_rt, srvDesc);
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
			GfxDescriptorRange cbvSrvUavRanges[2];
			cbvSrvUavRanges[0].m_rangeType           = GfxDescriptorRangeType::kSrv;
			cbvSrvUavRanges[0].m_rangeFlag           = GfxDescriptorRangeFlag::kDynamic;
			cbvSrvUavRanges[0].m_descriptorCount     = 1;
			cbvSrvUavRanges[0].m_shaderRegisterIndex = 0;
			cbvSrvUavRanges[1].m_rangeType           = GfxDescriptorRangeType::kCbv;
			cbvSrvUavRanges[1].m_descriptorCount     = 1;
			cbvSrvUavRanges[1].m_shaderRegisterIndex = 1;
		
			GfxDescriptorRange samplerRanges[1];
			samplerRanges[0].m_rangeType       = GfxDescriptorRangeType::kSampler;
			samplerRanges[0].m_descriptorCount = 1;

			GfxDescriptorHeapTable tables[2];
			tables[0].m_rangeCount = (uint32_t)ArraySize(cbvSrvUavRanges);
			tables[0].m_ranges = cbvSrvUavRanges;
			tables[1].m_rangeCount = (uint32_t)ArraySize(samplerRanges);
			tables[1].m_ranges = samplerRanges;

			GfxRootSignatureDesc rootSignatureDesc;
			rootSignatureDesc.m_tableCount = (uint32_t)ArraySize(tables);
			rootSignatureDesc.m_tables = tables;

			m_rootSignatures[0] = m_device.CreateRootSignature(rootSignatureDesc);
			m_rootSignatures[1] = m_device.CreateRootSignature(rootSignatureDesc);
		}

		// PSOのセットアップ.
		{
			static const GfxInputElement kElements[] =
			{
				{"POSITION",  0, GfxFormat::kR32G32B32_Float,  0, 0},
				{"TEXCOORD",  0, GfxFormat::kR32G32_Float,     0, 12},
			};

			GfxGraphicsStateDesc stateDesc;
			stateDesc.m_inputElements      = kElements;
			stateDesc.m_inputElementCount  = (int)ArraySize(kElements);
			stateDesc.m_rootSignature      = &m_rootSignatures[0];
			stateDesc.m_vertexShader       = &m_vertexShader;
			stateDesc.m_pixelShader        = &m_pixelShader;
			stateDesc.m_rtvFormats[0]      = GfxFormat::kR8G8B8A8_Unorm;
			stateDesc.m_depthEnable        = true;
			stateDesc.m_depthWriteMask     = GfxDepthWriteMask::kAll;
			m_graphicsStates[0] = m_device.CreateGraphicsState(stateDesc);
		
			stateDesc.m_rootSignature      = &m_rootSignatures[1];
			stateDesc.m_depthEnable        = false;
			m_graphicsStates[1] = m_device.CreateGraphicsState(stateDesc);
		}

		// commandList経由でリソースのデータをアップロードする.
		BeginRender();
		{
			m_graphicsCommandList.UploadBuffer(m_device, m_vertexBuffer, kVertexData, sizeof(kVertexData));

			std::vector<uint8_t> texData = GenerateTextureData(m_texture.GetWidth(), m_texture.GetHeight());
			m_graphicsCommandList.UploadTexture(m_device, m_texture, &texData[0], texData.size());
		}
		EndRender();

		return 0;
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

		m_device.ReleaseDescriptorHeap(m_dsvHeap);
		m_device.ReleaseTexture(m_depth);
		
		m_device.ReleaseDescriptorHeap(m_rtvHeap);
		m_device.ReleaseTexture(m_rt);
		m_device.ReleaseTexture(m_texture);
		
		m_device.ReleaseBuffer(m_constantBuffers[1]);
		m_device.ReleaseBuffer(m_constantBuffers[0]);

		m_pixelShader.Release();
		m_vertexShader.Release();

		m_device.ReleaseBuffer(m_vertexBuffer);

		PipelineBase::OnTerminate();

		return 0;
	}
	
	int Pipeline::OnUpdate(const AppUpdateInfo&)
	{
		static float s_delta = 0.1f;
		if(8.0f <= m_constantAddrs[0]->m_uvScale)
		{
			s_delta = -0.1f;
		}
		else
		if(m_constantAddrs[0]->m_uvScale <= 1.0f)
		{
			s_delta = 0.1f;
		}
		m_constantAddrs[0]->m_uvScale += s_delta;
		return 0;
	}
	
	int Pipeline::OnRender(const AppRenderInfo&)
	{
		BeginRender();

		m_graphicsCommandList.SetGraphicsState(m_graphicsStates[0]);
		m_graphicsCommandList.SetGraphicsRootSignature(m_rootSignatures[0]);

		GfxDescriptorHeap* heaps[] = {&m_cbvSrvUavHeaps[0], &m_samplerHeaps[0]};
		m_graphicsCommandList.SetDescriptorHeaps((uint32_t)ArraySize(heaps), heaps);
		m_graphicsCommandList.SetGraphicsDescriptorTable(0, m_cbvSrvUavHeaps[0].GetGpuDescriptor(0));
		m_graphicsCommandList.SetGraphicsDescriptorTable(1, m_samplerHeaps[0].GetGpuDescriptor(0));
		
		GfxCpuDescriptor rtDescriptor[]  = {m_rtvHeap.GetCpuDescriptor(0)};
		GfxCpuDescriptor dsvDescriptor = m_dsvHeap.GetCpuDescriptor(0);
		m_graphicsCommandList.SetRenderTargets(1, rtDescriptor, dsvDescriptor);
		
		GfxViewport viewport0 = GfxViewport(0.0f, 0.0f, (float)m_rt.GetWidth(), (float)m_rt.GetHeight());
		GfxScissor scissor0  = GfxScissor(0, 0, m_rt.GetWidth(), m_rt.GetHeight());
		m_graphicsCommandList.SetViewports(1, &viewport0);
		m_graphicsCommandList.SetScissors(1, &scissor0);

		m_graphicsCommandList.ClearRenderTarget(
			rtDescriptor[0],
			kRtCleaColor[0],
			kRtCleaColor[1],
			kRtCleaColor[2],
			kRtCleaColor[3]);

		m_graphicsCommandList.ClearDepthStencilTarget(dsvDescriptor, 1.0f);

		m_graphicsCommandList.SetPrimitiveTopology(GfxPrimitiveTopology::kTriangleList);
		m_graphicsCommandList.SetVertexBuffers(
			0,
			1,
			&GfxVertexBufferView(&m_vertexBuffer, sizeof(PosUvVertex)));
			
		m_graphicsCommandList.DrawInstanced(6, 1, 0, 0);

		m_graphicsCommandList.ResourceBarrier(
			m_rt,
			GfxResourceState::kRenderTarget,
			GfxResourceState::kPixelShaderResource);
		
		GfxCpuDescriptor swapChainDescriptor = m_swapChain.GetSwapChainCpuDescriptor();
		GfxTexture swapChainTexture = m_swapChain.GetSwapChainTexture();
		GfxViewport viewport1(0.0f, 0.0f, (float)swapChainTexture.GetWidth(), (float)swapChainTexture.GetHeight());
		GfxScissor  scissor1(0, 0, swapChainTexture.GetWidth(), swapChainTexture.GetHeight());
		m_graphicsCommandList.SetRenderTargets(1, &swapChainDescriptor);
		m_graphicsCommandList.SetViewports(1, &viewport1);
		m_graphicsCommandList.SetScissors(1, &scissor1);
		
		m_graphicsCommandList.SetGraphicsState(m_graphicsStates[1]);
		m_graphicsCommandList.SetGraphicsRootSignature(m_rootSignatures[1]);

		GfxDescriptorHeap* heapsOut[] = {&m_cbvSrvUavHeaps[1], &m_samplerHeaps[1]};
 		m_graphicsCommandList.SetDescriptorHeaps((uint32_t)ArraySize(heapsOut), heapsOut);
		m_graphicsCommandList.SetGraphicsDescriptorTable(0, m_cbvSrvUavHeaps[1].GetGpuDescriptor(0));
		m_graphicsCommandList.SetGraphicsDescriptorTable(1, m_samplerHeaps[1].GetGpuDescriptor(0));

		m_graphicsCommandList.ClearRenderTarget(swapChainDescriptor, 0.0f, 0.2f, 0.4f, 1.0f);

		m_graphicsCommandList.SetPrimitiveTopology(GfxPrimitiveTopology::kTriangleList);
		m_graphicsCommandList.SetVertexBuffers(
			0,
			1,
			&GfxVertexBufferView(&m_vertexBuffer, sizeof(PosUvVertex)));
			
		m_graphicsCommandList.DrawInstanced(6, 1, 0, 0);
		
		m_graphicsCommandList.ResourceBarrier(
			m_rt,
			GfxResourceState::kPixelShaderResource,
			GfxResourceState::kRenderTarget);

		EndRender();

		return 0;
	}
	
} // namespace APP002
} // namespace SI
