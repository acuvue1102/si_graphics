
#include "si_base/renderer/material.h"

#include "si_base/misc/bitwise.h"
#include "si_base/file/file_utility.h"
#include "si_base/gpu/gfx_shader.h"
#include "si_base/gpu/gfx_graphics_state.h"
#include "si_base/gpu/gfx_input_layout.h"
#include "si_base/gpu/gfx_root_signature_ex.h"
#include "si_base/gpu/gfx_dds.h"
#include "si_base/renderer/renderer.h"
#include "si_base/renderer/material/material_simple.h"

namespace SI
{
	bool RenderMaterial::DefaultSetup(
		const char* name,
		const char* shaderPath,
		int8_t materialConstantSlot)
	{
		if(m_vertexShader.LoadAndCompile(shaderPath) != 0)
		{
			SI_ASSERT(0);
			return false;
		}

		if(m_pixelShader.LoadAndCompile(shaderPath) != 0)
		{
			SI_ASSERT(0);
			return false;
		}
		
		const GfxShaderBinding& vsBinding = m_vertexShader.GetBindingResource();
		const GfxShaderBinding& psBinding = m_pixelShader.GetBindingResource();

		uint64_t constantMask = vsBinding.m_constantSlotMask | psBinding.m_constantSlotMask;
		uint32_t constantCount = Bitwise::BitCount64(constantMask);

		uint64_t srvMask = vsBinding.m_srvSlotMask | psBinding.m_srvSlotMask;
		uint32_t srvCount = Bitwise::BitCount64(srvMask);

		uint64_t samplerMask = vsBinding.m_samplerSlotMask | psBinding.m_samplerSlotMask;
		uint32_t samplerCount = Bitwise::BitCount64(samplerMask);

		// vs/psで共通のバッファのサイズを取得する.
		uint16_t materialConstantSize = 0;
		for(const GfxShaderBinding& binding : {vsBinding, psBinding})
		{
			// なるべくすぐ見つかると思われる後ろから調べる.
			for(int i=(int)binding.m_constantCount-1; 0<=i; --i)
			{
				const GfxShaderConstantInfo& sbInfo = binding.m_constantInfoArray[i];
				if(materialConstantSlot == sbInfo.m_slot)
				{
					materialConstantSize = Max(materialConstantSize, sbInfo.m_size);
					break;
				}
			}
		}

		GfxRootSignatureDescEx rootSignatureDesc;
		SetupDefaultRootSignatureDescEx(
			rootSignatureDesc,
			name,
			srvCount,
			samplerCount,
			constantCount);
		m_rootSignature.Initialize(rootSignatureDesc);

		//Renderer& render = *Renderer::GetInstance();
		for(uint32_t i=0; i<kFrameCount; ++i)
		{
			GfxBufferEx_Constant& constantBuffer = m_constantBuffers[i];
			GfxDescriptorHeapEx& srvHeap = m_srvHeaps[i];
			GfxDescriptorHeapEx& samplerHeap = m_samplerHeaps[i];

			// マテリアル用のコンスタントバッファのセットアップ
			if(0<materialConstantSize)
			{
				constantBuffer.InitializeAsConstant(name, materialConstantSize);
				void* cb = constantBuffer.GetMapPtr();
				memset(cb, 0, materialConstantSize);
			}

			// SRVのセットアップ
			if(0<srvCount)
			{
				srvHeap.InitializeAsCbvSrvUav(srvCount);
			}
		
			// Samplerのセットアップ
			if(0<samplerCount)
			{
				samplerHeap.InitializeAsSampler(samplerCount);
			}
		}

		return true;
	}

	void RenderMaterial::SetupDefaultRootSignatureDescEx(
		GfxRootSignatureDescEx& outRootSignatureDesc,
		const char* name,
		uint32_t srvCount,
		uint32_t samplerCount,
		uint32_t cbvCount)
	{
		m_CBVRootIndexOffset = 0;
		if(0<srvCount) ++m_CBVRootIndexOffset;
		if(0<samplerCount) ++m_CBVRootIndexOffset;

		if(0<m_CBVRootIndexOffset)
		{
			outRootSignatureDesc.CreateTables(m_CBVRootIndexOffset);

			uint32_t rootIndex = 0;
			if(0<srvCount)
			{
				GfxDescriptorHeapTableEx& table0 = outRootSignatureDesc.GetTable(rootIndex++);
				table0.ReserveRanges(1);
				table0.GetRange(0).Set(GfxDescriptorRangeType::Srv, srvCount, 0, GfxDescriptorRangeFlag::Volatile);
			}

			if(0<samplerCount)
			{
				GfxDescriptorHeapTableEx& table1 = outRootSignatureDesc.GetTable(rootIndex++);
				table1.ReserveRanges(1);
				table1.GetRange(0).Set(GfxDescriptorRangeType::Sampler, samplerCount, 0, GfxDescriptorRangeFlag::DescriptorsVolatile);
			}

			SI_ASSERT(rootIndex==m_CBVRootIndexOffset);
		}

		outRootSignatureDesc.CreateRootDescriptors(cbvCount);
		for(uint32_t i=0; i<cbvCount; ++i)
		{
			GfxRootDescriptor& rootDescriptor = outRootSignatureDesc.GetRootDescriptor(i);
			rootDescriptor.m_type = GfxRootDescriptorType::CBV;
			rootDescriptor.m_shaderRegisterIndex = i;
			rootDescriptor.m_registerSpace = 0;
			rootDescriptor.m_flags = GfxRootDescriptorFlag::DataVolatile;
			rootDescriptor.m_visibility = GfxShaderVisibility::All;
		}
		
		outRootSignatureDesc.SetName(name);
	}

	GfxTexture RenderMaterial::GetTexture(int textureId, const IScenes& scenes, const GfxTexture& defaultTex)
	{
		if(0 <= textureId && (uint32_t)textureId < scenes.GetTextureInfoCount())
		{
			const TextureInfo& ti = scenes.GetTextureInfo((uint32_t)textureId);
			int imageId = ti.GetImageId();
			if(0 <= imageId && 0 < scenes.GetImageCount())
			{
				return scenes.GetImage(imageId);
			}
		}

		return defaultTex;
	}

	//---------------------------------------------------
	
	Material::~Material()
	{
		size_t renderMaterialCount = m_renderMaterials.size();
		for(size_t i=0; i<renderMaterialCount; ++i)
		{
			RenderMaterial* r = m_renderMaterials[i];
			SI_DELETE(r);
		}
		m_renderMaterials.clear();
	}

	void Material::Setup()
	{
		m_renderMaterials.reserve(1);

		RenderMaterialSimple_Opaque* rm = SI_NEW(RenderMaterialSimple_Opaque);
		rm->Initialize(*this);
		
		m_renderMaterials.push_back(rm);

		SetupDrawStageMask();
	}

	void Material::SetupDrawStageMask()
	{
		m_mask.Reset();
		size_t count = m_renderMaterials.size();
		for(size_t i=0; i<count; ++i)
		{
			RenderMaterial* m = m_renderMaterials[i];
			RendererDrawStageType stageType = m->GetStageType();

			SI_ASSERT(!m_mask.CheckEnable(stageType), "同じステージに2個以上RenderMaterialがある.");
			m_mask.EnableMaskBit(stageType);
		}
	}

	void Material::UpdateRenderMaterial(uint32_t frameIndex, const IScenes& scenes, RenderMaterial* renderMaterial) const
	{
		renderMaterial->RenderSetup(frameIndex, scenes, *this);
	}
	
} // namespace SI
