#pragma once

#include "si_base/container/array.h"
#include "si_base/renderer/renderer_common.h"

namespace SI
{
	class Material;

	struct SubMeshSerializeData
	{
		ObjectIndex   m_materialIndex;
		ObjectIndex   m_geometryIndex;
		
		SubMeshSerializeData()
			: m_materialIndex(kInvalidObjectIndex)
			, m_geometryIndex(kInvalidObjectIndex)
		{
		}

		SI_REFLECTION(
			SI::SubMeshSerializeData,
			SI_REFLECTION_MEMBER_AS_TYPE(m_materialIndex, uint16_t),
			SI_REFLECTION_MEMBER_AS_TYPE(m_geometryIndex, uint16_t))
	};

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
		
		SubMeshSerializeData ConvertSerializeData() const
		{
			SubMeshSerializeData serializeData;
			serializeData.m_materialIndex = m_materialIndex;
			serializeData.m_geometryIndex = m_geometryIndex;

			return serializeData;
		}

	private:
		friend class FbxParser;

	private:
		ObjectIndex   m_materialIndex; // Model(Instance)::m_materialsのIndex
		ObjectIndex   m_geometryIndex; // Model(Instance)::m_geometriesのIndex
	};
	
} // namespace SI
