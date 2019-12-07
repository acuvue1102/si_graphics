#pragma once

#include <memory>
#include <vector>
#include "si_base/core/assert.h"
#include "si_base/misc/string.h"

#include "si_base/gpu/gfx.h"

namespace SI
{
	struct VertexAttribute
	{
		GfxSemantics m_semantics;
		int          m_accessorId;
	};

	class SubMesh
	{
	public:
		SubMesh()
			: m_topology(SI::GfxPrimitiveTopology::TriangleList)
			, m_materialId(-1)
			, m_indicesAccessorId(-1)
		{}

		SI::GfxPrimitiveTopology GetTopology() const{ return m_topology; }
		void SetTopology(SI::GfxPrimitiveTopology topology){ m_topology = topology; }

		int GetMaterialId() const{ return m_materialId; }
		void SetMaterialId(int materialId){ m_materialId = materialId; }

		int GetIndicesAccessorId() const{ return m_indicesAccessorId; }
		void SetIndicesAccessorId(int accessorId){ m_indicesAccessorId = accessorId; }

		void ReserveVertexAttribute(uint32_t attributeCount){ m_vertexAttributes.reserve(attributeCount); }
		void AddVertexAttribute(const VertexAttribute& attribute){ m_vertexAttributes.push_back(attribute); }
		uint32_t GetVertexAttributeCount() const{ return (uint32_t)m_vertexAttributes.size(); }
		VertexAttribute& GetVertexAttribute(uint32_t id){ return m_vertexAttributes[id]; }

		std::vector<VertexAttribute>* GetVertexAttributes(){ return &m_vertexAttributes; }

	private:
		SI::GfxPrimitiveTopology m_topology;
		int m_materialId;
		int m_indicesAccessorId;
		std::vector<VertexAttribute> m_vertexAttributes;
	};

} // namespace SI
