
#include "si_base/renderer/material/material_simple.h"

#include "si_base/misc/bitwise.h"
#include "si_base/file/file_utility.h"
#include "si_base/gpu/gfx_shader.h"
#include "si_base/gpu/gfx_graphics_state.h"
#include "si_base/gpu/gfx_input_layout.h"
#include "si_base/gpu/gfx_root_signature_ex.h"
#include "si_base/gpu/gfx_dds.h"
#include "si_base/renderer/renderer.h"
#include "si_base/renderer/scenes.h"
#include "si_base/renderer/renderer.h"

namespace SI
{
	void RenderMaterialSimple_Opaque::Initialize(const Material& material)
	{				
		DefaultSetup(
			"material_simple",
			"asset\\shader\\simple.hlsl",
			2); // materialConstantSlot
	}
	
	void RenderMaterialSimple_Opaque::RenderSetup(uint32_t frameIndex, const IScenes& scenes, const Material& material)
	{
		GfxTexture baseColorTex = GetTexture(material.GetBaseColorTextureId(), scenes, Renderer::GetInstance()->GetWhiteTexture().Get());
		SI_ASSERT(baseColorTex.IsValid());

		if(m_constantBuffers[frameIndex].Get().IsValid())
		{
			void* constant = m_constantBuffers[frameIndex].GetMapPtr();
			if(constant)
			{
				Vfloat4 baseColor = material.GetBaseColorFactor();

				struct MaterialCB
				{
					Vfloat4 cbBaseColor;
					float cbUvScale[2];
				};
				SI_ASSERT(m_constantBuffers->GetSize() == sizeof(MaterialCB));

				MaterialCB* cb = (MaterialCB*)constant;
				cb->cbBaseColor = baseColor;
				cb->cbUvScale[0] = cb->cbUvScale[1] = 1.0f;
			}
		}

		GfxShaderResourceViewDesc srvDesc;
		srvDesc.m_format = baseColorTex.GetFormat();
		srvDesc.m_arraySize = baseColorTex.GetArraySize();
		srvDesc.m_miplevels = baseColorTex.GetMipLevels();

		GfxDescriptorHeapEx& srvHeap = GetSrvHeap(frameIndex);
		if (srvHeap.IsValid())
		{
			srvHeap.SetShaderResourceView(0, baseColorTex, srvDesc);
		}

		GfxSamplerDesc samplerDesc;
		GfxDescriptorHeapEx& samplerHeap = GetSamplerHeap(frameIndex);
		if(samplerHeap.IsValid())
		{
			samplerHeap.SetSampler(0, samplerDesc);
		}
	}
	
} // namespace SI
