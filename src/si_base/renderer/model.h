﻿#pragma once

#include <memory>
#include <vector>
#include "si_base/container/array.h"
#include "si_base/renderer/node.h"
#include "si_base/misc/reference_counter.h"
#include "si_base/core/new_delete.h"

namespace SI
{
	class Node;
	class NodeCore;
	class Mesh;
	class SubMesh;
	class Material;
	class Geometry;

	class Model
	{
	public:
		Model();
		~Model();
	public:
		void AllocateNodes       (ObjectIndex size);
		void AllocateNodeCores   (ObjectIndex size);
		void AllocateMeshes      (ObjectIndex size);
		void AllocateSubMeshes   (ObjectIndex size);
		void AllocateMaterials   (ObjectIndex size);
		void AllocateGeometries  (ObjectIndex size);
		void AllocateStrings     (ObjectIndex size);
		void AllocateStringPool  (LongObjectIndex size);
		
		void DeallocateNodes       ();
		void DeallocateNodeCores   ();
		void DeallocateMeshes      ();
		void DeallocateSubMeshes   ();
		void DeallocateMaterials   ();
		void DeallocateGeometries  ();
		void DeallocateStrings     ();
		void DeallocateStringPool  ();
		
		      Node&           GetRootNode()        { return m_rootNode; }
		const Node&           GetRootNode()  const { return m_rootNode; }

		Array<Node>&          GetNodes()           { return m_nodes;    }
		ConstArray<Node>      GetNodes()      const{ return m_nodes;    }
		
		Array<NodeCore>&      GetNodeCores()       { return m_nodeCores; }
		ConstArray<NodeCore>  GetNodeCores()  const{ return m_nodeCores; }
		 
		Array<Mesh>&          GetMeshes()          { return m_meshes;    }
		ConstArray<Mesh>      GetMeshes()     const{ return m_meshes;    }

		Array<SubMesh>&       GetSubMeshes()       { return m_subMeshes; }
		ConstArray<SubMesh>   GetSubMeshes()  const{ return m_subMeshes; }

		Array<Material*>&           GetMaterials() { return m_materials; }
		ConstArray<const Material*> GetMaterials()  const
		{
			return ConstArray<const Material*>(m_materials.GetItemsAddr(), m_materials.GetItemCount());
		}

		Array<Geometry*>&           GetGeometries() { return m_geometries; }
		ConstArray<const Geometry*> GetGeometries() const
		{
			return ConstArray<const Geometry*>(m_geometries.GetItemsAddr(), m_geometries.GetItemCount());
		}

		Array<LongObjectIndex>&          GetStrings()         { return m_strings; }
		ConstArray<LongObjectIndex>      GetStrings()    const{ return m_strings; }

		Array<char>&          GetStringPool()         { return m_stringPool; }
		ConstArray<char>      GetStringPool()    const{ return m_stringPool; }
		
	private:
		friend class FbxParser;

	private:
		Node                   m_rootNode;   // root. m_children以外はダミー.
		Array<Node>            m_nodes;      // nodeの階層配列. Node内のindexでアクセスする.
		Array<NodeCore>        m_nodeCores;  // nodeの中身情報. m_nodesと同様にNode内のindexでアクセスする.
		Array<Mesh>            m_meshes;     // mesh配列.       Mesh用のNode::m_nodeComponentでアクセスする.
		Array<SubMesh>         m_subMeshes;  // submesh配列.    Mesh::m_submeshIndecesでアクセスする.
		Array<Material*>       m_materials;  // material配列.   Submesh::m_materialsでアクセスする.
		Array<Geometry*>       m_geometries; // geometry配列.   Submesh::m_geometriesでアクセスする.
		Array<LongObjectIndex> m_strings;    // string配列.     Node::m_nameやMaterial::m_nameでアクセスする.
		Array<char>            m_stringPool; // string配列の実体.
	};

	using ModelPtr = std::shared_ptr<Model>;

} // namespace SI