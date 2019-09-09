#pragma once

#include "si_base/renderer/node_iterator.h"

namespace SI
{
	NodeIndexHandler::NodeIndexHandler(const Model& model)
		: m_rootNode(&model.GetRootNode())
		, m_nodes(model.GetNodes())
		, m_currentNodeIndex((0<m_rootNode->GetChildrenCount())? m_rootNode->GetChildNodeIndex(0) : kInvalidObjectIndex)
		, m_rootChild(0)
		, m_nestIndex(0)
		, m_nestNodeIndexArray()
		, m_nestChildArray()
	{
		m_nestNodeIndexArray[0] = m_currentNodeIndex;
		m_nestChildArray[0]     = 0;
	}
	
	NodeIndexHandler::NodeIndexHandler(const ModelInstance& modelInstance)
		: m_rootNode(&modelInstance.GetRootNode())
		, m_nodes(modelInstance.GetNodes())
		, m_currentNodeIndex((0<m_rootNode->GetChildrenCount())? m_rootNode->GetChildNodeIndex(0) : kInvalidObjectIndex)
		, m_rootChild(0)
		, m_nestIndex(0)
		, m_nestNodeIndexArray()
		, m_nestChildArray()
	{
		m_nestNodeIndexArray[0] = m_currentNodeIndex;
		m_nestChildArray[0]     = 0;
	}

	NodeIndexHandler::~NodeIndexHandler()
	{
	}
	
	void NodeIndexHandler::Reset()
	{
		m_currentNodeIndex      = (0<m_rootNode->GetChildrenCount())? m_rootNode->GetChildNodeIndex(0) : kInvalidObjectIndex;
		m_rootChild             = 0;
		m_nestIndex             = 0;
		m_nestNodeIndexArray[0] = m_currentNodeIndex;
		m_nestChildArray[0]     = 0;
	}
	
	bool NodeIndexHandler::IsEnd() const
	{
		return m_currentNodeIndex == kInvalidObjectIndex;
	}

	void NodeIndexHandler::Next()
	{
		if(IsEnd()) return;

		const Node* node = m_nodes[m_currentNodeIndex];
		// 子がいる場合は先に子を走査する.
		if(0 < node->GetChildrenCount())
		{
			++m_nestIndex;
			SI_ASSERT((size_t)m_nestIndex<m_nestChildArray.size());
			m_nestChildArray[m_nestIndex] = 0;

			m_currentNodeIndex = node->GetChildNodeIndex(0);
			m_nestNodeIndexArray[m_nestIndex] = m_currentNodeIndex;
			return;
		}

		// 1つ以上親のノードにフォーカスを移動させる.
		while(true)
		{
			// 最上位ノードなので、rootNodeの子内で走査する.
			if(m_nestIndex==0)
			{
				++m_rootChild;
				if(m_rootNode->GetChildrenCount() <= m_rootChild)
				{
					// 終わり.
					m_currentNodeIndex = kInvalidObjectIndex;
					return;
				}

				// rootNodeの兄弟に移動.
				m_currentNodeIndex = m_rootNode->GetChildNodeIndex(m_rootChild);
				m_nestNodeIndexArray[0] = m_currentNodeIndex;
				return;
			}
			
			// 一旦、親に移動.
			--m_nestIndex;
			++m_nestChildArray[m_nestIndex];
			uint16_t childIndex = m_nestChildArray[m_nestIndex];
			node = m_nodes[m_nestNodeIndexArray[m_nestIndex]];

			if(childIndex < node->GetChildrenCount())
			{
				// 兄弟に移動.
				++m_nestIndex;
				m_currentNodeIndex = node->GetChildNodeIndex(childIndex);
				m_nestNodeIndexArray[m_nestIndex] = m_currentNodeIndex;
				return;
			}
		}
	}
	
	ObjectIndex  NodeIndexHandler::GetCurrentNodeIndex() const
	{
		return m_currentNodeIndex;
	}

	ObjectIndex  NodeIndexHandler::GetNestNodeIndex(uint16_t nestIndex) const
	{
		SI_ASSERT((int16_t)nestIndex <= m_nestIndex);
		return m_nestNodeIndexArray[m_nestIndex];
	}

	ObjectIndex  NodeIndexHandler::GetNestIndex() const
	{
		return m_nestIndex;
	}

	//////////////////////////////////////////////////////////////////////////////////

	
	ConstNodeIterator::ConstNodeIterator(const Model& model)
		: m_rootNode(&model.GetRootNode())
		, m_nodes(model.GetNodes())
		, m_nodeCores(model.GetNodeCores())
		, m_nodeIndexHandler(model)
	{
	}

	ConstNodeIterator::ConstNodeIterator(const ModelInstance& modelInstance)
		: m_rootNode(&modelInstance.GetRootNode())
		, m_nodes(modelInstance.GetNodes())
		, m_nodeCores(modelInstance.GetNodeCores())
		, m_nodeIndexHandler(modelInstance)
	{
	}

	ConstNodeIterator::~ConstNodeIterator()
	{
	}
	
	void ConstNodeIterator::Reset()
	{
		m_nodeIndexHandler.Reset();
	}
	
	bool ConstNodeIterator::IsEnd() const
	{
		return m_nodeIndexHandler.IsEnd();
	}

	void ConstNodeIterator::Next()
	{
		m_nodeIndexHandler.Next();
	}

	const Node* ConstNodeIterator::GetNode() const
	{
		if(IsEnd()) return nullptr;
		
		ObjectIndex currentNodeIndex = m_nodeIndexHandler.GetCurrentNodeIndex();
		SI_ASSERT(currentNodeIndex != kInvalidObjectIndex && currentNodeIndex<m_nodes.GetItemCount());
		return m_nodes[currentNodeIndex];
	}

	const Node* ConstNodeIterator::GetNode(uint16_t nestIndex) const
	{
		if(IsEnd()) return nullptr;
		if(m_nodeIndexHandler.GetNestIndex() < (int16_t)nestIndex) return nullptr;
		
		ObjectIndex nodeIndex = m_nodeIndexHandler.GetNestNodeIndex(nestIndex);
		SI_ASSERT(nodeIndex != kInvalidObjectIndex && nodeIndex<m_nodes.GetItemCount());
		return m_nodes[nodeIndex];
	}
	
	const NodeCore* ConstNodeIterator::GetNodeCore() const
	{
		if(IsEnd()) return nullptr;
		
		ObjectIndex currentNodeIndex = m_nodeIndexHandler.GetCurrentNodeIndex();
		SI_ASSERT(currentNodeIndex != kInvalidObjectIndex && currentNodeIndex<m_nodeCores.GetItemCount());
		return m_nodeCores[currentNodeIndex];
	}

	const NodeCore* ConstNodeIterator::GetNodeCore(uint16_t nestIndex) const
	{
		if(IsEnd()) return nullptr;
		if(m_nodeIndexHandler.GetNestIndex() < (int16_t)nestIndex) return nullptr;
		
		ObjectIndex nodeIndex = m_nodeIndexHandler.GetNestNodeIndex(nestIndex);
		SI_ASSERT(nodeIndex != kInvalidObjectIndex && nodeIndex<m_nodeCores.GetItemCount());
		return m_nodeCores[nodeIndex];
	}

	int16_t ConstNodeIterator::GetNestIndex() const
	{
		return m_nodeIndexHandler.GetNestIndex();
	}

	
	//////////////////////////////////////////////////////////////////////////////////


	NodeIterator::NodeIterator(Model& model)
		: m_rootNode(&model.GetRootNode())
		, m_nodes(model.GetNodes())
		, m_nodeCores(model.GetNodeCores())
		, m_nodeIndexHandler(model)
	{
	}

	NodeIterator::NodeIterator(ModelInstance& modelInstance)
		: m_rootNode(&modelInstance.GetRootNode())
		, m_nodes(modelInstance.GetNodes())
		, m_nodeCores(modelInstance.GetNodeCores())
		, m_nodeIndexHandler(modelInstance)
	{
	}

	NodeIterator::~NodeIterator()
	{
	}
	
	void NodeIterator::Reset()
	{
		m_nodeIndexHandler.Reset();
	}
	
	bool NodeIterator::IsEnd() const
	{
		return m_nodeIndexHandler.IsEnd();
	}

	void NodeIterator::Next()
	{
		m_nodeIndexHandler.Next();
	}

	Node* NodeIterator::GetNode() const
	{
		if(IsEnd()) return nullptr;
		
		ObjectIndex currentNodeIndex = m_nodeIndexHandler.GetCurrentNodeIndex();
		SI_ASSERT(currentNodeIndex != kInvalidObjectIndex && currentNodeIndex<m_nodes.GetItemCount());
		return m_nodes[currentNodeIndex];
	}

	Node* NodeIterator::GetNode(uint16_t nestIndex) const
	{
		if(IsEnd()) return nullptr;
		if(m_nodeIndexHandler.GetNestIndex() < (int16_t)nestIndex) return nullptr;
		
		ObjectIndex nodeIndex = m_nodeIndexHandler.GetNestNodeIndex(nestIndex);
		SI_ASSERT(nodeIndex != kInvalidObjectIndex && nodeIndex<m_nodes.GetItemCount());
		return m_nodes[nodeIndex];
	}
	
	NodeCore* NodeIterator::GetNodeCore() const
	{
		if(IsEnd()) return nullptr;
		
		ObjectIndex currentNodeIndex = m_nodeIndexHandler.GetCurrentNodeIndex();
		SI_ASSERT(currentNodeIndex != kInvalidObjectIndex && currentNodeIndex<m_nodeCores.GetItemCount());
		return m_nodeCores[currentNodeIndex];
	}

	NodeCore* NodeIterator::GetNodeCore(uint16_t nestIndex) const
	{
		if(IsEnd()) return nullptr;
		if(m_nodeIndexHandler.GetNestIndex() < (int16_t)nestIndex) return nullptr;
		
		ObjectIndex nodeIndex = m_nodeIndexHandler.GetNestNodeIndex(nestIndex);
		SI_ASSERT(nodeIndex != kInvalidObjectIndex && nodeIndex<m_nodeCores.GetItemCount());
		return m_nodeCores[nodeIndex];
	}

	int16_t NodeIterator::GetNestIndex() const
	{
		return m_nodeIndexHandler.GetNestIndex();
	}

} // namespace SI
