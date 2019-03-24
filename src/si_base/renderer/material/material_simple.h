#pragma once

#include "si_base/renderer/material.h"

namespace SI
{
	class RenderMaterialSimple_Opaque : public RenderMaterial
	{
	public:
		RenderMaterialSimple_Opaque() {}
		virtual ~RenderMaterialSimple_Opaque(){}

		void Initialize();
		
		virtual RendererDrawStageType GetStageType() const override
		{
			return RendererDrawStageType_Opaque;
		}

		struct SimpleMaterialCB
		{
			float m_uvScale[2];
		};
	private:
		GfxBufferEx_Constant m_constant;
	};

	class MaterialSimple : public Material
	{
	public:
		MaterialSimple();
		virtual ~MaterialSimple();
		
		virtual void ExportSerializeData(MaterialSerializeData& outData) const override;
		virtual void ImportSerializeData(const MaterialSerializeData& serializeData) override;
			
		virtual void UpdateRenderMaterial(RenderMaterial& renderMaterial, uint32_t frameIndex) const override;

	private:		
		float               m_uvScale[2];
		GfxTextureEx_Static m_texture;
	};

} // namespace SI
