
#include "pipeline_006.h"

#include <string>
#include <vector>
#include <array>
#include <si_base/core/core.h>
#include <si_app/file/path_storage.h>
#include <si_base/math/math.h>
#include <si_base/input/keyboard.h>
#include "compiledShaders\raytracing.hlsl.h"

namespace SI
{
namespace APP006
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
				float u = ((float)x + 0.5f) / (float)width;
				float v = ((float)y + 0.5f) / (float)height;

				Vfloat3 color(u, v, 0);

				pData[n] = color.X();	// R
				pData[n + 1] = color.Y();	// G
				pData[n + 2] = color.Z();	// B
				pData[n + 3] = 1.0f;	    // A
			}

			return std::move(data);
		}
		
		const char* kHitGroupShaderName = "MyHitGroup";
		const char* kRayGenShaderName = "MyRaygenShader";
		const char* kClosestHitShaderName = "MyClosestHitShader";
		const char* kMissShaderName = "MyMissShader";
	}
	struct Viewport
	{
		float left;
		float top;
		float right;
		float bottom;
	};

	struct Pipeline::RaytracingShaderConstant
	{
		Viewport viewport;
		Viewport stencil;

		std::array<float,4> incolor;
	};
	
	struct Pipeline::TextureShaderConstant
	{
		float m_vertexScale[2];
		float m_uvScale[2];
	};

	//////////////////////////////////////////////////////////
	
	Pipeline::Pipeline(int observerSortKey)
		: PipelineBase(observerSortKey)
		, m_textureConstant(nullptr)
		, m_dxrConstant(nullptr)
		, m_currentComputeId(0)
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
				"vertex",
				kVertexData,
				ArraySize(kVertexData),
				sizeof(kVertexData[0]),
				0);
		}

		// textureシェーダのセットアップ.
		{
			if(m_textureVS.LoadAndCompile("asset\\shader\\texture.hlsl") != 0) return -1;
			if(m_texturePS.LoadAndCompile("asset\\shader\\texture.hlsl") != 0) return -1;
		}
		
		// result textureのセットアップ.
		{
			m_resultTexture.InitializeAs2DUav(
				"copyedTexture",
				1980,
				1080,
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
		
		// samplerのセットアップ
		{
			GfxSamplerDesc samplerDesc;
			m_sampler.Initialize(samplerDesc);
		}

		// root signatureのセットアップ
		{
			GfxRootSignatureDescEx rootSignatureDesc;
			rootSignatureDesc.CreateTables(2);

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
			stateDesc.m_rootSignature      = &m_rootSignatures.Get();
			stateDesc.m_rtvFormats[0]      = GfxFormat::R8G8B8A8_Unorm;
			stateDesc.m_dsvFormat          = GfxFormat::Unknown;
			stateDesc.m_depthEnable        = false;
			m_graphicsStates = m_device.CreateGraphicsState(stateDesc);
		}

		// GlobalRootSignature
		{
			GfxRootSignatureDescEx rootSignatureDesc;
			rootSignatureDesc.CreateTables(1);

			GfxDescriptorHeapTableEx& table0 = rootSignatureDesc.GetTable(0);
			table0.ReserveRanges(1);
			table0.GetRange(0).Set(GfxDescriptorRangeType::Uav, 1, 0, GfxDescriptorRangeFlag::Volatile);

			rootSignatureDesc.CreateRootDescriptors(1);
			GfxRootDescriptor& descriptor0 = rootSignatureDesc.GetRootDescriptor(0);
			descriptor0.InitAsSRV(0);

			rootSignatureDesc.SetFlags(GfxRootSignatureFlag::None);
			rootSignatureDesc.SetName("dxrGlobalRootSig");
			m_dxrGlobalRootSignatures.Initialize(rootSignatureDesc);
		}

		// LocalRootSignature
		{
			GfxRootSignatureDescEx rootSignatureDesc;
			rootSignatureDesc.CreateRootDescriptors(1);

			GfxRootDescriptor& rootDescriptor =  rootSignatureDesc.GetRootDescriptor(0);
			rootDescriptor.m_shaderRegisterIndex = 0;
			rootDescriptor.m_type = GfxRootDescriptorType::CBV;

			rootSignatureDesc.SetFlags(GfxRootSignatureFlag::LocalRootSignature);

			rootSignatureDesc.SetName("dxrLocalRootSig");
			m_dxrLocalRootSignatures.Initialize(rootSignatureDesc);
		}

		// コンスタントバッファのセットアップ
		{
			float aspectRatio = 1920.0f / 1080.0f;
			float border = 0.1f;
			m_dxrConstantBuffers.InitializeAsConstant("textureConstant", sizeof(RaytracingShaderConstant));
			m_dxrConstant = static_cast<RaytracingShaderConstant*>(m_dxrConstantBuffers.GetMapPtr());
			m_dxrConstant->viewport = { -1.0f, -1.0f, 1.0f, 1.0f };
			m_dxrConstant->stencil = 
			{
				-1 + border / aspectRatio, -1 + border,
				 1 - border / aspectRatio,  1 - border
			};

			m_dxrConstant->incolor = {1,0,0,1};
		}

		// Raytracing Stateをセットアップする.
		{
			GfxRaytracingStateDesc raytracingDesc = m_device.CreateRaytracingStateDesc();
			raytracingDesc.SetType(GfxStateObjectType::RaytracingPipeline);

			raytracingDesc.ReserveSubObject(7);
			{
				GfxDxilLibraryDesc dxidDesc = raytracingDesc.CreateDxilLibraryDesc();
				GfxShaderCodeByte dxilLib(g_sb_raytracing, sizeof(g_sb_raytracing));
				
				SI_ASSERT(dxilLib.m_shaderBytecode);

				dxidDesc.SetDxilLibrary(dxilLib);
				dxidDesc.AddExportDesc(kRayGenShaderName);
				dxidDesc.AddExportDesc(kClosestHitShaderName);
				dxidDesc.AddExportDesc(kMissShaderName);

				raytracingDesc.AddSubObject(dxidDesc);
			}

			{
				GfxHitGroupDesc hitGroupDesc = raytracingDesc.CreateHitGroupDesc();
				hitGroupDesc.SetClosestHitShaderImport(kClosestHitShaderName);
				hitGroupDesc.SetHitGroupExport(kHitGroupShaderName);
				hitGroupDesc.SetType(GfxHitGroupType::Triangle);

				raytracingDesc.AddSubObject(hitGroupDesc);
			}

			{
				GfxRaytracingShaderConfigDesc shaderCofigDesc = raytracingDesc.CreateRaytracingShaderConfigDesc();
				shaderCofigDesc.SetMaxAttributeSizeInBytes(2 * sizeof(float));
				shaderCofigDesc.SetMaxPayloadSizeInBytes(4 * sizeof(float));

				raytracingDesc.AddSubObject(shaderCofigDesc);
			}

			{
				GfxStateSubObject subObject = raytracingDesc.CreateAndAddLocalRootSignature(&m_dxrLocalRootSignatures.Get());

				// Shader association
				GfxSubObjectToExportAssosiation rootSignatureAssociation = raytracingDesc.CreateSubObjectToExportAssosiation();
				rootSignatureAssociation.SetSubObjectToAssociation(subObject);
				rootSignatureAssociation.AddExport(kRayGenShaderName);

				raytracingDesc.AddSubObject(rootSignatureAssociation);
			}

			raytracingDesc.CreateAndAddGlobalRootSignature(&m_dxrGlobalRootSignatures.Get());

			{
				// PipelineConfigの設定
				GfxRaytracingPipelineConfigDesc pipelineConfig = raytracingDesc.CreateRaytracingPipelineConfigDesc();		
				pipelineConfig.SetMaxTraceRecursionDepth(1);

				raytracingDesc.AddSubObject(pipelineConfig);
			}


			// RaytracingState作成.
			m_dxrState = m_device.CreateRaytracingState(raytracingDesc);
		}

		m_dxrScene = m_device.CreateRaytracingScene();

		// Initialize Geometry
		{
			uint16_t indices[] =
			{
				0,1,2
			};
			m_dxrIndex.InitializeAsUpload("dxrIndex", indices, sizeof(indices));

			float offset = 0.7f;
			float vertices[] =
			{
				0,      -offset, 1.0f,
				-offset, offset, 1.0f,
				offset,  offset, 1.0f
			};
			m_dxrVertex.InitializeAsUpload("dxrVertex", vertices, sizeof(vertices));
		}


		// Initialize ShaderTable
		{
			GfxRaytracingShaderTablesDesc tablesDesc;

			struct RootArgument
			{
				SI::GpuAddress m_cb;
			} rootArguments;
			rootArguments.m_cb = m_dxrConstantBuffers.GetGpuAddress();

			// rayGen
			{
				GfxShaderTableDesc& rayGenTableDesc = tablesDesc.GetRayGenTableDesc();
				rayGenTableDesc.ReserveRecord(1);
				rayGenTableDesc.AddShaderRecord(m_dxrState, kRayGenShaderName, rootArguments);
			}

			// miss
			{
				GfxShaderTableDesc& missTableDesc = tablesDesc.GetMissTableDesc();
				missTableDesc.ReserveRecord(1);
				missTableDesc.AddShaderRecord(m_dxrState, kMissShaderName);
			}

			// hit group
			{
				GfxShaderTableDesc& hitGroupTableDesc = tablesDesc.GetHitGroupTableDesc();
				hitGroupTableDesc.ReserveRecord(1);
				hitGroupTableDesc.AddShaderRecord(m_dxrState, kHitGroupShaderName);
			}

			m_dxrShaderTables = m_device.CreateRaytracingShaderTables(tablesDesc);
		}

		{
			m_dxrDescriptorHeap.InitializeAsCbvSrvUav(1);

			GfxUnorderedAccessViewDesc uavDesc;
			uavDesc.m_uavDimension = GfxDimension::Texture2D;
			uavDesc.m_format = m_resultTexture.GetFormat();
			m_dxrDescriptorHeap.SetUnorderedAccessView(0, m_resultTexture.Get(), uavDesc);
		}

		auto textureData = GenerateTextureData(m_resultTexture.GetWidth(), m_resultTexture.GetHeight());

		// commandList経由でリソースのデータをアップロードする.
		BeginRender();
		{
			GfxGraphicsContext& context = m_contextManager.GetGraphicsContext(0);

			GfxRaytracingGeometryDesc geometryDesc;
			geometryDesc.m_type = GfxRaytracingGeometryType::Triangles;
			geometryDesc.m_flags = GfxRaytracingGeometryFlag::Opaque;
			GfxRaytracingGeometryTriangleDesc& triangleDesc = geometryDesc.m_triangle;
			triangleDesc.m_indexFormat = GfxFormat::R16_Uint;
			triangleDesc.m_indexBuffer = m_dxrIndex.Get().GetGpuAddress();
			triangleDesc.m_indexCount  = (uint32_t)(m_dxrIndex.Get().GetSize() / sizeof(uint16_t));
			triangleDesc.m_vertexFormat = GfxFormat::R32G32B32_Float;
			triangleDesc.m_vertexBuffer = m_dxrVertex.Get().GetGpuAddress();
			triangleDesc.m_vertexCount  = (uint32_t)(m_dxrVertex.Get().GetSize() / (sizeof(float)*3));

			context.BeginBuildAccelerationStructures(m_dxrScene);
			context.AddGeometry(geometryDesc);
			context.EndBuildAccelerationStructures();

			//context.UploadBuffer(m_device, m_quadVertexBuffer, kVertexData, sizeof(kVertexData), GfxResourceState::VertexAndConstantBuffer);

			context.UploadTexture(
				m_device,
				m_resultTexture,
				&textureData[0],
				sizeof(textureData[0]) * textureData.size(),
				GfxResourceState::PixelShaderResource);
		}
		EndRender();

		return 0;
	}

	int Pipeline::OnTerminate()
	{
		m_swapChain.Wait();

		m_device.ReleaseRaytracingScene(m_dxrScene);

		m_device.ReleaseRaytracingShaderTables(m_dxrShaderTables);

		//m_dxrIndex.Terminate();
		//m_dxrVertex.Terminate();

		m_device.ReleaseRaytracingState(m_dxrState);
		
		for(int i=0; i<kMaxRaytracingCompute; ++i)
		{
			m_device.ReleaseComputeState(m_computeStates[i]);
		}
		m_device.ReleaseGraphicsState(m_graphicsStates);

		m_computeRootSignatures.Terminate();
		m_rootSignatures.Terminate();

		m_sampler.Terminate();
		
		m_textureConstant = nullptr;
		m_constantBuffers.TerminateAsConstant();

		m_resultTexture.TerminateUav();

		m_texturePS.Release();
		m_textureVS.Release();
		
		for(int i=0; i<kMaxRaytracingCompute; ++i)
		{
			m_raytracingCS[i].Release();
		}
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

		// Raytracing
		{
			context.SetComputeRootSignature(m_dxrGlobalRootSignatures);

			GfxDescriptorHeap* heaps[] = { &m_dxrDescriptorHeap.Get() };
			context.SetDescriptorHeapsDirectly(1, heaps);
			context.SetComputeDescriptorTable(0, m_dxrDescriptorHeap.Get().GetGpuDescriptor(0));
			context.SetComputeRootSRV(1, m_dxrScene.GetTopASBuffer());

			GfxDispatchRaysDesc rayDesc;
			rayDesc.m_width = m_resultTexture.GetWidth();
			rayDesc.m_height = m_resultTexture.GetHeight();
			rayDesc.m_depth = 1;
			rayDesc.m_tables = &m_dxrShaderTables;

			context.SetPipelineState(m_dxrState);

			context.DispatchRays(rayDesc);
		}

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
	
	void Pipeline::OnKeyboard(const App& app, Key k, bool isDown)
	{
		if(k == Key::Right && isDown)
		{
			++m_currentComputeId;
			
			if(kMaxRaytracingCompute <= m_currentComputeId)
			{
				m_currentComputeId = 0;
			}
		}
		else if(k == Key::Left && isDown)
		{
			--m_currentComputeId;
			if(m_currentComputeId < 0)
			{
				m_currentComputeId = kMaxRaytracingCompute-1;
			}
		}
	}
	
} // namespace APP006
} // namespace SI
