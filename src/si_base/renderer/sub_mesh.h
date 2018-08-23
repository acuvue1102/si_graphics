#pragma once

#include "si_base/container/array.h"
#include "si_base/renderer/renderer_common.h"

namespace SI
{
	class Material;

	class SubMesh
	{
	public:
		SubMesh()
			: m_materialIndex(kInvalidObjectIndex)
			, m_geometryIndex(kInvalidObjectIndex)
		{
		}

		~SubMesh()
		{
		}

	private:
		friend class FbxParser;

	private:
		ObjectIndex   m_materialIndex; // Model(Instance)::m_materialsのIndex
		ObjectIndex   m_geometryIndex; // Model(Instance)::m_geometriesのIndex
	};

} // namespace SI
