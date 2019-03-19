
#include "si_base/renderer/material.h"

#include "si_base/gpu/gfx_shader.h"
#include "si_base/gpu/gfx_graphics_state.h"
#include "si_base/gpu/gfx_input_layout.h"
#include "si_base/gpu/gfx_root_signature_ex.h"
#include "si_base/renderer/renderer.h"

namespace SI
{
	struct SimpleMaterialCB
	{
		float m_uvScale[2];
	};

	void RenderMaterial_Simple::Initialize()
	{
		std::string shaderPath = "asset\\shader\\lambert.hlsl";
		if(m_vertexShader.LoadAndCompile(shaderPath.c_str()) != 0)
		{
			SI_ASSERT(0);
			return;
		}

		if(m_pixelShader.LoadAndCompile(shaderPath.c_str()) != 0)
		{
			SI_ASSERT(0);
			return;
		}
		
		GfxRootSignatureDescEx rootSignatureDesc;
		rootSignatureDesc.ReserveTables(2);
		
		static const uint32_t kSrvCount = 1;
		static const uint32_t kCbvCount = 3;
		static const uint32_t kSamplerCount = 1;
		GfxDescriptorHeapTableEx& table0 = rootSignatureDesc.GetTable(0);
		table0.ReserveRanges(2);
		table0.GetRange(0).Set(GfxDescriptorRangeType::Srv, kSrvCount, 0, GfxDescriptorRangeFlag::Volatile);
		table0.GetRange(1).Set(GfxDescriptorRangeType::Cbv, kCbvCount, 0, GfxDescriptorRangeFlag::Volatile);

		GfxDescriptorHeapTableEx& table1 = rootSignatureDesc.GetTable(1);
		table1.ReserveRanges(1);
		table1.GetRange(0).Set(GfxDescriptorRangeType::Sampler, kSamplerCount, 0, GfxDescriptorRangeFlag::DescriptorsVolatile);

		rootSignatureDesc.SetName("rootSig0");
		m_rootSignature.Initialize(rootSignatureDesc);		

		Renderer& render = *Renderer::GetInstance();
		for(uint32_t i=0; i<kFrameCount; ++i)
		{
			GfxBufferEx_Constant& constantBuffer = m_constantBuffers[i];
			GfxDescriptorHeap& srvHeap = m_srvHeaps[i];
			GfxDescriptorHeap& cbvHeap = m_cbvHeaps[i];
			GfxDescriptorHeap& samplerHeap = m_samplerHeaps[i];

			// コンスタントバッファのセットアップ
			{
				constantBuffer.InitializeAsConstant("SimpleMaterialCB", sizeof(SimpleMaterialCB));
				SimpleMaterialCB* cb = static_cast<SimpleMaterialCB*>(constantBuffer.GetMapPtr());
				memset(cb, 0, sizeof(SimpleMaterialCB));

				cb->m_uvScale[0]  = 1.0f;
				cb->m_uvScale[1]  = 1.0f;
			}

			// SRVのセットアップ
			{
				GfxDescriptorHeapDesc srvHeapDesc;
				srvHeapDesc.m_descriptorCount = 1;
				srvHeapDesc.m_type = GfxDescriptorHeapType::CbvSrvUav;
				srvHeapDesc.m_flag = GfxDescriptorHeapFlag::ShaderVisible;
				GfxDevice& device = *GfxDevice::GetInstance();
				srvHeap = device.CreateDescriptorHeap(srvHeapDesc);
			
				//GfxShaderResourceViewDesc sceneSrvDesc;
				//sceneSrvDesc.m_arraySize
				//GfxBuffer sceneCb = render.GetSceneDB(i).GetBuffer();
				//sceneCbvDesc.m_buffer = &sceneCb;
				//device.CreateConstantBufferView(srvHeap, 0, sceneCbvDesc);
			}

			// CBVのセットアップ
			{
				GfxDescriptorHeapDesc cbvHeapDesc;
				cbvHeapDesc.m_descriptorCount = 3;
				cbvHeapDesc.m_type = GfxDescriptorHeapType::CbvSrvUav;
				cbvHeapDesc.m_flag = GfxDescriptorHeapFlag::ShaderVisible;
				GfxDevice& device = *GfxDevice::GetInstance();
				cbvHeap = device.CreateDescriptorHeap(cbvHeapDesc);
			
				GfxConstantBufferViewDesc sceneCbvDesc;
				GfxBuffer sceneCb = render.GetSceneDB(i).GetBuffer();
				sceneCbvDesc.m_buffer = &sceneCb;
				device.CreateConstantBufferView(cbvHeap, 0, sceneCbvDesc);
			
				//GfxConstantBufferViewDesc instanceCbvDesc;
				//GfxBuffer instanceCb = render.GetDummyCB().GetBuffer(); // ひとまずダミーのCBを指しとく.
				//instanceCbvDesc.m_buffer = &instanceCb;
				//device.CreateConstantBufferView(cbvHeap, 1, instanceCbvDesc);
			
				GfxConstantBufferViewDesc materialCbvDesc;
				GfxBuffer materialCb = constantBuffer.GetBuffer();
				materialCbvDesc.m_buffer = &materialCb;
				device.CreateConstantBufferView(cbvHeap, 2, materialCbvDesc);
			}
		
			GfxDescriptorHeapDesc samplerHeapDesc;
			samplerHeapDesc.m_descriptorCount = 1;
			samplerHeapDesc.m_type = GfxDescriptorHeapType::CbvSrvUav;
			samplerHeapDesc.m_flag = GfxDescriptorHeapFlag::ShaderVisible;
			GfxDevice& device = *GfxDevice::GetInstance();
			samplerHeap = device.CreateDescriptorHeap(samplerHeapDesc);
		}
	}

	//---------------------------------------------------

	Material::Material()
		: m_name(kInvalidObjectIndex)
	{
	}
	
	Material::~Material()
	{
	}

	void Material::ImportSerializeData(const MaterialSerializeData& serializeData)
	{
		m_name = serializeData.m_name;
		
		m_mask.EnableMaskBit(RendererDrawStageType_Opaque);
	}

	Material* Material::Create()
	{
		return new Material();
	}

	void Material::Release(Material*& material)
	{
		delete material;
		material = nullptr;
	}
	
} // namespace SI
