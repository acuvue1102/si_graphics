
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
	}

	struct Pipeline::ShaderConstant
	{
		float m_uvScale;
	};

	//////////////////////////////////////////////////////////
	
	Pipeline::Pipeline(int observerSortKey)
		: PipelineBase(observerSortKey)
		, m_constantAddr(nullptr)
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
		GfxDescriptorRange cbvSrvUavRanges[2];
		cbvSrvUavRanges[0].m_rangeType           = kGfxDescriptorRangeType_Srv;
		cbvSrvUavRanges[0].m_descriptorCount     = 1;
		cbvSrvUavRanges[0].m_shaderRegisterIndex = 0;
		cbvSrvUavRanges[1].m_rangeType           = kGfxDescriptorRangeType_Cbv;
		cbvSrvUavRanges[1].m_descriptorCount     = 1;
		cbvSrvUavRanges[1].m_shaderRegisterIndex = 1;
		
		GfxDescriptorRange samplerRanges[1];
		samplerRanges[0].m_rangeType       = kGfxDescriptorRangeType_Sampler;
		samplerRanges[0].m_descriptorCount = 1;

		GfxDescriptorHeapTable tables[2];
		tables[0].m_rangeCount = (uint32_t)ArraySize(cbvSrvUavRanges);
		tables[0].m_ranges = cbvSrvUavRanges;
		tables[1].m_rangeCount = (uint32_t)ArraySize(samplerRanges);
		tables[1].m_ranges = samplerRanges;

		GfxRootSignatureDesc rootSignatureDesc;
		rootSignatureDesc.m_tableCount = (uint32_t)ArraySize(tables);
		rootSignatureDesc.m_tables = tables;
		m_rootSignature = m_device.CreateRootSignature(rootSignatureDesc);

		std::string shaderPath = PathStorage::GetInstance()->GetExeDirPath();
		shaderPath += "shaders\\texture.hlsl";

		if(m_vertexShader.LoadAndCompile(shaderPath.c_str()) != 0) return -1;
		if(m_pixelShader. LoadAndCompile(shaderPath.c_str()) != 0) return -1;

		static const GfxInputElement kElements[] =
		{
			{"POSITION",  0, kGfxFormat_R32G32B32_Float,  0, 0},
			{"TEXCOORD",  0, kGfxFormat_R32G32_Float,     0, 12},
		};

		GfxGraphicsStateDesc stateDesc;
		stateDesc.m_inputElements      = kElements;
		stateDesc.m_inputElementCount  = (int)ArraySize(kElements);
		stateDesc.m_rootSignature      = &m_rootSignature;
		stateDesc.m_vertexShader       = &m_vertexShader;
		stateDesc.m_pixelShader        = &m_pixelShader;
		m_graphicsState = m_device.CreateGraphicsState(stateDesc);

		float aspect = (float)info.m_width/(float)info.m_height;

		static const PosUvVertex kVertexData[] = 
		{
			{ -0.95f / aspect,  0.95f, 0.0f ,  0.0f, 0.0f },
			{  0.95f / aspect, -0.95f, 0.0f ,  1.0f, 1.0f },
			{ -0.95f / aspect, -0.95f, 0.0f ,  0.0f, 1.0f },


			{ -0.95f / aspect,  0.95f, 0.0f ,  0.0f, 0.0f },
			{  0.95f / aspect,  0.95f, 0.0f ,  1.0f, 0.0f },
			{  0.95f / aspect, -0.95f, 0.0f ,  1.0f, 1.0f },
		};

		GfxBufferDesc vertexBufferDesc;
		vertexBufferDesc.m_heapType = kGfxHeapType_Default;
		vertexBufferDesc.m_bufferSizeInByte = sizeof(kVertexData);
		m_vertexBuffer = m_device.CreateBuffer(vertexBufferDesc);

		GfxBufferDesc constantBufferDesc;
		constantBufferDesc.m_heapType = kGfxHeapType_Upload;
		constantBufferDesc.m_bufferSizeInByte = (sizeof(ShaderConstant) + 255) & ~255; // should be multiple of 256 bytes.
		m_constantBuffer = m_device.CreateBuffer(constantBufferDesc);
		m_constantAddr = static_cast<ShaderConstant*>(m_constantBuffer.Map());
		m_constantAddr->m_uvScale = 1;

		m_viewport = GfxViewport(0.0f, 0.0f, (float)info.m_width, (float)info.m_height);
		m_scissor  = GfxScissor(0, 0, info.m_width, info.m_height);

		GfxTextureDesc textureDesc;
		textureDesc.m_width     = 256;
		textureDesc.m_height    = 256;
		textureDesc.m_mipLevels = 1;
		textureDesc.m_format    = kGfxFormat_R8G8B8A8_Unorm;
		textureDesc.m_dimension = kGfxDimension_Texture2D;
		m_texture = m_device.CreateTexture(textureDesc);

		GfxDescriptorHeapDesc cbvSrvUavHeapDesc;
		cbvSrvUavHeapDesc.m_type = kGfxDescriptorHeapType_CbvSrvUav;
		cbvSrvUavHeapDesc.m_descriptorCount = 2;
		cbvSrvUavHeapDesc.m_flag = kGfxDescriptorHeapFlag_ShaderVisible;
		m_cbvSrvUavHeap = m_device.CreateDescriptorHeap(cbvSrvUavHeapDesc);
		
		GfxDescriptorHeapDesc samplerHeapDesc;
		samplerHeapDesc.m_type = kGfxDescriptorHeapType_Sampler;
		samplerHeapDesc.m_descriptorCount = 1;
		samplerHeapDesc.m_flag = kGfxDescriptorHeapFlag_ShaderVisible;
		m_samplerHeap = m_device.CreateDescriptorHeap(samplerHeapDesc);

		GfxShaderResourceViewDesc srvDesc;
		srvDesc.m_format = kGfxFormat_R8G8B8A8_Unorm;
		srvDesc.m_miplevels = 1;
		srvDesc.m_srvDimension = kGfxDimension_Texture2D;
		m_device.CreateShaderResourceView(m_cbvSrvUavHeap, 0, m_texture, srvDesc);

		GfxConstantBufferViewDesc cbvDesc;
		cbvDesc.m_buffer = &m_constantBuffer;
		m_device.CreateConstantBufferView(m_cbvSrvUavHeap, 1, cbvDesc);
		
		GfxSamplerDesc samplerDesc;
		m_device.CreateSampler(m_samplerHeap, 0, samplerDesc);

		// commandList経由でリソースのデータをアップロードする.
		BeginRender();
		{
			m_graphicsCommandList.UploadBuffer(m_device, m_vertexBuffer, kVertexData, sizeof(kVertexData));

			std::vector<uint8_t> texData = GenerateTextureData(textureDesc.m_width, textureDesc.m_height);
			m_graphicsCommandList.UploadTexture(m_device, m_texture, &texData[0], texData.size());
		}
		EndRender();

		return 0;
	}

	int Pipeline::OnTerminate()
	{
		m_swapChain.Flip(); // wait previous frame.

		m_device.ReleaseDescriptorHeap(m_samplerHeap);

		m_device.ReleaseDescriptorHeap(m_cbvSrvUavHeap);

		m_device.ReleaseTexture(m_texture);

		m_device.ReleaseBuffer(m_constantBuffer);

		m_device.ReleaseBuffer(m_vertexBuffer);

		m_device.ReleaseGraphicsState(m_graphicsState);

		m_device.ReleaseRootSignature(m_rootSignature);

		PipelineBase::OnTerminate();

		return 0;
	}
	
	int Pipeline::OnUpdate(const AppUpdateInfo&)
	{
		static float s_delta = 0.1f;
		if(8.0f <= m_constantAddr->m_uvScale)
		{
			s_delta = -0.1f;
		}
		else
		if(m_constantAddr->m_uvScale <= 1.0f)
		{
			s_delta = 0.1f;
		}
		m_constantAddr->m_uvScale += s_delta;
		return 0;
	}
	
	int Pipeline::OnRender(const AppRenderInfo&)
	{
		BeginRender();

		GfxTexture tex = m_swapChain.GetSwapChainTexture();

		m_graphicsCommandList.SetGraphicsState(m_graphicsState);
						
		m_graphicsCommandList.SetGraphicsRootSignature(m_rootSignature);

		GfxDescriptorHeap* heaps[] = {&m_cbvSrvUavHeap, &m_samplerHeap};
		m_graphicsCommandList.SetDescriptorHeaps((uint32_t)ArraySize(heaps), heaps);
		m_graphicsCommandList.SetGraphicsDescriptorTable(0, m_cbvSrvUavHeap.GetGpuDescriptor(kGfxDescriptorHeapType_CbvSrvUav, 0));
		m_graphicsCommandList.SetGraphicsDescriptorTable(1, m_samplerHeap.GetGpuDescriptor(kGfxDescriptorHeapType_Sampler, 0));

		m_graphicsCommandList.SetRenderTargets(1, &tex);
		m_graphicsCommandList.SetViewports(1, &m_viewport);
		m_graphicsCommandList.SetScissors(1, &m_scissor);

		m_graphicsCommandList.ClearRenderTarget(tex, 0.0f, 0.2f, 0.4f, 1.0f);

		m_graphicsCommandList.SetPrimitiveTopology(kGfxPrimitiveTopology_TriangleList);
		m_graphicsCommandList.SetVertexBuffers(
			0,
			1,
			&GfxVertexBufferView(&m_vertexBuffer, sizeof(PosUvVertex)));
			
		m_graphicsCommandList.DrawInstanced(6, 1, 0, 0);

		EndRender();

		return 0;
	}
	
} // namespace APP002
} // namespace SI
