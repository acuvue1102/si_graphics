#pragma once

#include "si_base/container/array.h"
#include "si_base/renderer/renderer_graphics_state.h"
#include "si_base/gpu/gfx_graphics_state_ex.h"
#include "si_base/renderer/submesh.h"

namespace SI
{
	class IScenes;
	class Material;
	class RenderMaterial;
	class SubMesh;
	class Accessor;
	class GfxGraphicsContext;
	
	struct RenderState
	{
	};

	struct RenderItem
	{
		bool NeedToCreatePSO(const RendererGraphicsStateDesc& renderDesc) const;
		void SetupPSO(const RendererGraphicsStateDesc& renderDesc);
		bool IsValid() const;

		Vfloat4x4                      m_worldMatrix;

		IScenes*                       m_scenes = nullptr;
		Material*                      m_material = nullptr;
		RenderMaterial*                m_renderMaterial = nullptr;
		SubMesh*                       m_subMesh = nullptr;
		Accessor*                      m_indexAccessor = nullptr;
		std::vector<VertexAttribute>*  m_vertexAttributes = nullptr;

		GfxGraphicsStateEx             m_graphicsState;
		RendererGraphicsStateDesc      m_graphicsStateDesc;
	};

} // namespace SI
