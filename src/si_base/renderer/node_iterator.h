#pragma once

#include <array>
#include "si_base/renderer/model.h"
#include "si_base/renderer/model_instance.h"
#include "si_base/renderer/node.h"

namespace SI
{
	// NodeIteratorのためのNodeIndex走査クラス.
	// このクラスは直接使わない.
	class NodeIndexHandler
	{
	public:
		NodeIndexHandler(const Model& model);
		NodeIndexHandler(const ModelInstance& modelInstance);
		~NodeIndexHandler();

		void Reset();
				
		bool IsEnd() const;
		void Next();
		
		ObjectIndex  GetCurrentNodeIndex()                const;
		ObjectIndex  GetNestNodeIndex(uint16_t nestIndex) const;
		ObjectIndex  GetNestIndex()                       const;

	private:
		static const size_t kMaxNest = 126;
		using NodeIndexArray  = std::array<ObjectIndex, kMaxNest>; // m_nodes/m_nodesCoreのインデックス.
		using ChildIndexArray = std::array<uint16_t,    kMaxNest>; // childrenのインデックス.

	private:
		const Node*              m_rootNode;
		ConstArray<Node*>        m_nodes;

		ObjectIndex              m_currentNodeIndex; // m_nodes/m_nodeCoresのインデックス.
		uint16_t                 m_rootChild;
		int16_t                  m_nestIndex;
		NodeIndexArray           m_nestNodeIndexArray;
		ChildIndexArray          m_nestChildArray;
	};
	
	/////////////////////////////////////////////////////////////////////////////////////////

	class ConstNodeIterator
	{
	public:
		ConstNodeIterator(const Model& model);
		ConstNodeIterator(const ModelInstance& modelInstance);
		~ConstNodeIterator();

		void Reset();

		bool IsEnd() const;
		void Next();
		
		const Node*     GetNode() const;
		const Node*     GetNode(uint16_t nestIndex) const;
		const NodeCore* GetNodeCore() const;
		const NodeCore* GetNodeCore(uint16_t nestIndex) const;
		int16_t         GetNestIndex() const;

	private:
		ConstNodeIterator(); // 禁止

	private:
		const Node*              m_rootNode;
		ConstArray<Node*>        m_nodes;
		ConstArray<NodeCore*>    m_nodeCores;

		NodeIndexHandler         m_nodeIndexHandler;
	};

	/////////////////////////////////////////////////////////////////////////////////////////
		
	class NodeIterator
	{
	public:
		NodeIterator(Model& model);
		NodeIterator(ModelInstance& modelInstance);
		~NodeIterator();

		void Reset();

		bool IsEnd() const;
		void Next();
		
		Node*           GetNode() const;
		Node*           GetNode(uint16_t nestIndex) const;
		NodeCore*       GetNodeCore() const;
		NodeCore*       GetNodeCore(uint16_t nestIndex) const;
		int16_t         GetNestIndex() const;

	private:
		NodeIterator(); // 禁止

	private:
		Node*                    m_rootNode;
		Array<Node*>             m_nodes;
		Array<NodeCore*>         m_nodeCores;

		NodeIndexHandler         m_nodeIndexHandler;
	};

	/////////////////////////////////////////////////////////////////////////////////////////


} // namespace SI

#include "si_base/renderer/node_iterator.inl"
