
#include "si_base/renderer/material/material_simple.h"

#include "si_base/misc/bitwise.h"
#include "si_base/file/file_utility.h"
#include "si_base/gpu/gfx_shader.h"
#include "si_base/gpu/gfx_graphics_state.h"
#include "si_base/gpu/gfx_input_layout.h"
#include "si_base/gpu/gfx_root_signature_ex.h"
#include "si_base/gpu/gfx_dds.h"
#include "si_base/renderer/renderer.h"

namespace SI
{
	void RenderMaterialSimple_Opaque::Initialize()
	{				
		DefaultSetup(
			"material_simple",
			"asset\\shader\\simple.hlsl",
			2); // materialConstantSlot
	}

	//---------------------------------------------------

	MaterialSimple::MaterialSimple()
	{
		m_uvScale[0] = m_uvScale[1] = 1.0f;
	}
	
	MaterialSimple::~MaterialSimple()
	{
		uint32_t renderMaterialCount = m_renderMaterials.GetItemCount();
		for(uint32_t i=0; i<renderMaterialCount; ++i)
		{
			RenderMaterial* r = m_renderMaterials[i];
			SI_DELETE(r);
		}
		m_renderMaterials.Reset();
	}

	void MaterialSimple::ExportSerializeData(MaterialSerializeData& outData) const
	{
		outData.m_name = m_name;
	}

	void MaterialSimple::ImportSerializeData(const MaterialSerializeData& serializeData)
	{
		m_name = serializeData.m_name;

		// TODO: ここでアトリビュート等を読み込む.
		m_uvScale[0] = m_uvScale[1] = 1.0f;

		uint32_t vecCount = serializeData.m_vfloat4s.GetItemCount();
		for (uint32_t i = 0; i < vecCount; ++i)
		{
			const Vfloat4Attribute& attr = serializeData.m_vfloat4s.GetItem(i);

			if (strcmp(attr.m_name, "DiffuseColor") == 0)
			{
				m_diffuse = attr.m_value;

				if (attr.m_filePath && attr.m_filePath[0])
				{
					std::vector<uint8_t> texData;
					GfxDdsMetaData texMetaData;
					int ret = FileUtility::Load(texData, attr.m_filePath);
					SI_ASSERT(ret == 0);

					m_diffuseTexture.InitializeDDS("test_texture2", &texData[0], texData.size(), texMetaData);

					SI_DEVICE().UploadTextureLater(m_diffuseTexture.Get(), texMetaData.m_image, texMetaData.m_imageSise);
				}
			}
		}

		if(!m_diffuseTexture.IsValid())
		{
			m_diffuseTexture.InitializeAs2DStatic("diffuse", 1, 1, GfxFormat::R32G32B32A32_Float, 1);
			SI_DEVICE().UploadTextureLater(m_diffuseTexture.Get(), &m_diffuse, sizeof(float)*4);
		}
		
		// 各RenderMaterialをセットアップする
		m_renderMaterials.Setup(1);
		RenderMaterialSimple_Opaque* simple = SI_NEW(RenderMaterialSimple_Opaque);
		simple->Initialize();
		
		for (uint32_t f = 0; f < kFrameCount; ++f)
		{
			GfxShaderResourceViewDesc srvDesc;
			srvDesc.m_format = m_diffuseTexture.GetFormat();
			srvDesc.m_arraySize = m_diffuseTexture.GetArraySize();
			srvDesc.m_miplevels = m_diffuseTexture.GetMipLevels();

			GfxDescriptorHeapEx& srvHeap = simple->GetSrvHeap(f);
			if (srvHeap.IsValid())
			{
				srvHeap.SetShaderResourceView(0, m_diffuseTexture.Get(), srvDesc);
			}

			GfxSamplerDesc samplerDesc;
			GfxDescriptorHeapEx& samplerHeap = simple->GetSamplerHeap(f);
			if(samplerHeap.IsValid())
			{
				samplerHeap.SetSampler(0, samplerDesc);
			}
		}

		m_renderMaterials.SetItem(0, simple);
	}
	
	void MaterialSimple::UpdateRenderMaterial(RenderMaterial& renderMaterial, uint32_t frameIndex) const
	{
		if(renderMaterial.GetStageType() == RendererDrawStageType_Opaque)
		{
			RenderMaterialSimple_Opaque& renderMaterialOpaque = (RenderMaterialSimple_Opaque&)renderMaterial;
			GfxBufferEx_Constant& constantBuffer = renderMaterialOpaque.GetConstantBuffer(frameIndex);
			auto* cb = (RenderMaterialSimple_Opaque::SimpleMaterialCB*)constantBuffer.GetMapPtr();
			cb->m_uvScale[0] = m_uvScale[0];
			cb->m_uvScale[1] = m_uvScale[1];
		}
	}
	
} // namespace SI
