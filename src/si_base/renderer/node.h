#pragma once

#include "si_base/container/array.h"
#include "si_base/math/vquat.h"
#include "si_base/renderer/renderer_common.h"

namespace SI
{
	enum class NodeType : uint16_t
	{
		Transform = 0,  // componentなし.
		Mesh,           // Model::m_meshesのインデックスとなる.
		Light,          // Model::m_lightsのインデックスとなる.
		Max,
	};
	
	// Nodeの共通情報
	class NodeCore
	{
	public:
		NodeCore(){}
		~NodeCore(){}

		Vfloat4x4 GetLocalMatrix() const{ return m_localMatrix; }

	private:
		friend class FbxParser;

	private:
		Vfloat4x4   m_localMatrix; // このノードのローカルマトリックス.
		Vfloat4x4   m_worldMatrix; // 親のノードの影響を受けたワールドマトリックス.

		bool        m_isMatrixChanged; // worldMatrixの更新が必要か.
	};

	// Nodeクラス.
	// 階層情報くらいしか保持せず、中身はNodeCoreやNodeTypeごとの各オブジェクトに委ねる
	class Node
	{
	public:
		Node()
			//: m_name(nullptr)
			: m_name(kInvalidLongObjectIndex)
			, m_children()
			, m_current(kInvalidObjectIndex)
			, m_parent(kInvalidObjectIndex)
			, m_nodeComponentType(NodeType::Transform)
			, m_nodeComponent(kInvalidObjectIndex)
		{
		}

		~Node(){}
		
		LongObjectIndex GetNameIndex() const{ return m_name; }
		
		ObjectIndexRange GetChildren() const{ return m_children; }
		uint16_t      GetChildrenCount()                    const{ return m_children.m_count;        }
		ObjectIndex   GetChildNodeIndex(uint16_t index)     const
		{
			SI_ASSERT(m_children.m_first != kInvalidObjectIndex);
			SI_ASSERT(index < m_children.m_count);
			return m_children.m_first + index;
		}

		ObjectIndex   GetCurrentNodeIndex()                 const{ return m_current;                 }
		ObjectIndex   GetParentNodeIndex()                  const{ return m_parent;                  }
		NodeType      GetNodeComponentType()                const{ return m_nodeComponentType;       }
		ObjectIndex   GetNodeComponentIndex()               const{ return m_nodeComponent;           }

	private:
		friend class FbxParser;

	private:
		//const char*           m_name;
		LongObjectIndex       m_name;
		ObjectIndexRange      m_children;          // 子のノード. Model::m_nodesやm_nodeCoresのインデックス.
		ObjectIndex           m_current;           // 今のノード. Model::m_nodesやm_nodeCoresのインデックス.
		ObjectIndex           m_parent;            // 親のノード. Model::m_nodesやm_nodeCoresのインデックス.
		NodeType              m_nodeComponentType; // ノードのタイプ.
		ObjectIndex           m_nodeComponent;     // ノードのタイプごとのインデックス.
	};

} // namespace SI
