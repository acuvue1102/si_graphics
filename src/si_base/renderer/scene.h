#pragma once

#include <memory>
#include <vector>
#include "si_base/misc/string.h"

namespace SI
{
	class Scene
	{
	public:
		Scene(){}
		~Scene(){}

		void SetName(const char* name){ m_name = name; }
		const char* GetName() const{ return m_name.c_str(); }

		void ReserveNodes(size_t nodeCount){ m_nodes.reserve(nodeCount); }
		void AddNodeId(int nodeIndex){ m_nodes.push_back(nodeIndex); }

		int GetNodeCount() const{ return (int)m_nodes.size(); }
		int GetNodeId(int index) const{ return m_nodes[index]; }

	private:
		String m_name;
		std::vector<int> m_nodes;
	};

} // namespace SI
