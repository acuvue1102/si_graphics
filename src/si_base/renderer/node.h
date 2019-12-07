#pragma once

#include <memory>
#include <vector>
#include "si_base/core/assert.h"
#include "si_base/misc/string.h"
#include "si_base/math/math.h"

namespace SI
{
	class Node
	{
	public:
		Node()
			: m_id(-1)
			, m_parentId(-1)
			, m_meshId(-1)
		{}

		~Node(){}

		void ReserveNodes(size_t nodeCount){ m_childrenNodeIds.reserve(nodeCount); }
		void AddNodeId(int nodeIndex){ m_childrenNodeIds.push_back(nodeIndex); }

		int GetChildrenNodeCount() const{ return (int)m_childrenNodeIds.size(); }
		int GetChildrenNodeId(int index) const{ return m_childrenNodeIds[index]; }

		void SetParentId(int parentId){ m_parentId = parentId; }
		int GetParentId() const{ return m_parentId; }

		void SetId(int id){ m_id = id; }
		int GetId() const{ return m_id; }

		void SetMeshId(int meshId){ m_meshId = meshId; }
		int GetMeshId() const{ return m_meshId; }

		void SetName(const char* name){ m_name = name; }
		const char* GetName() const{ return m_name.c_str(); }

		void SetMatrix(Vfloat4x4_arg matrix){ m_matrix = matrix; }
		const Vfloat4x4& GetMatrix() const{ return m_matrix; }

	private:
		String m_name;
		int m_id;
		int m_parentId;
		std::vector<int> m_childrenNodeIds;
		int m_meshId;
		Vfloat4x4  m_matrix;
	};
} // namespace SI
