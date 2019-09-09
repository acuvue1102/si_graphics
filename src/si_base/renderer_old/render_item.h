#pragma once

#include "si_base/container/array.h"
#include "si_base/renderer/renderer_common.h"
#include "si_base/renderer/renderer_graphics_state.h"
#include "si_base/gpu/gfx_graphics_state_ex.h"

namespace SI
{
	class Material;
	class RenderMaterial;
	class Geometry;
	class GfxGraphicsContext;
	
	struct RenderState
	{
	};

	class RenderItem
	{
	public:
		RenderItem()
			: m_material(nullptr)
			, m_renderMaterial(nullptr)
			, m_geometry(nullptr)
			, m_nodeIndex(kInvalidObjectIndex)
		{}
		~RenderItem(){}

		void Setup(
			Material* material,
			RenderMaterial* renderMaterial,
			Geometry* geometry,
			ObjectIndex nodeIndex)
		{
			m_material = material;
			m_renderMaterial = renderMaterial;
			m_geometry = geometry;
			m_nodeIndex = nodeIndex;
			SI_ASSERT(m_nodeIndex != kInvalidObjectIndex);
		}

		bool NeedToCreatePSO(const RendererGraphicsStateDesc& renderDesc) const;
		void SetupPSO(const RendererGraphicsStateDesc& renderDesc);

		bool IsValid() const
		{
			if(!m_material) return false;
			if(!m_renderMaterial) return false;
			if(!m_geometry) return false;
			if(!m_graphicsState.Get().IsValid()) return false;
			return true;
		}
		
		Material&                  GetMaterial()      { return *m_material; }
		RenderMaterial&            GetRenderMaterial(){ return *m_renderMaterial; }
		Geometry&                  GetGeometry()      { return *m_geometry; }
		GfxGraphicsStateEx&        GetGraphicsState() { return m_graphicsState; }
		ObjectIndex                GetNodeIndex() const{ return m_nodeIndex; }

	private:
		Material*                  m_material;
		RenderMaterial*            m_renderMaterial;
		Geometry*                  m_geometry;
		ObjectIndex                m_nodeIndex;
		GfxGraphicsStateEx         m_graphicsState;
		RendererGraphicsStateDesc  m_graphicsStateDesc;
	};

} // namespace SI
