#pragma once

#include <memory>
#include "si_base/container/array.h"
#include "si_base/renderer/node.h"
#include "si_base/misc/reference_counter.h"

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
				
		Node&                  GetRootNode()        { return m_rootNode; }
		const Node&            GetRootNode()   const{ return m_rootNode; }
		
		Array<Node*>           GetNodes()           { return m_nodes;    }
		ConstArray<Node*>      GetNodes()      const{ return m_nodes;    }
		
		Array<NodeCore*>       GetNodeCores()       { return m_nodeCores; }
		ConstArray<NodeCore*>  GetNodeCores()  const{ return m_nodeCores; }
		 
		Array<Mesh*>           GetMeshes()          { return m_meshes;    }
		ConstArray<Mesh*>      GetMeshes()     const{ return m_meshes;    }    

		Array<SubMesh*>        GetSubMeshes()       { return m_subMeshes; }
		ConstArray<SubMesh*>   GetSubMeshes()  const{ return m_subMeshes; }

		Array<Material**>      GetMaterials()       { return m_materials; }
		ConstArray<Material**> GetMaterials()  const{ return m_materials; }

		Array<Geometry**>      GetGeometries()      { return m_geometries; }
		ConstArray<Geometry**> GetGeometries() const{ return m_geometries; }

	private:
		Node                 m_rootNode;   // root. m_children以外はダミー.
		Array<Node*>         m_nodes;      // nodeの階層配列. Node内のindexでアクセスする.
		Array<NodeCore*>     m_nodeCores;  // nodeの中身情報. m_nodesと同様にNode内のindexでアクセスする.
		Array<Mesh*>         m_meshes;     // mesh配列.       Mesh用のNode::m_nodeComponentでアクセスする.
		Array<SubMesh*>      m_subMeshes;  // submesh配列.    Mesh::m_submeshIndecesでアクセスする.
		Array<Material**>    m_materials;  // material配列.   Submesh::m_materialsでアクセスする.
		Array<Geometry**>    m_geometries; // geometry配列.   Submesh::m_geometriesでアクセスする.
	};

	using ModelPtr = std::shared_ptr<Model>;

} // namespace SI
