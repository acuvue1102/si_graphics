#pragma once

#include "si_base/renderer/material.h"

namespace SI
{
	class IScenes;

	class RenderMaterialSimple_Opaque : public RenderMaterial
	{
	public:
		RenderMaterialSimple_Opaque() {}
		virtual ~RenderMaterialSimple_Opaque(){}

		void Initialize(const Material& material);

		void RenderSetup(uint32_t frameIndex, const IScenes& scene, const Material& material) override;
		
		virtual RendererDrawStageType GetStageType() const override
		{
			return RendererDrawStageType::Opaque;
		}

		struct SimpleMaterialCB
		{
			float m_uvScale[2];
		};
	private:
		GfxBufferEx_Constant m_constant;
	};

} // namespace SI
