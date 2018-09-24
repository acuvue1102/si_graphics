
#include "si_base/renderer/model.h"

#include "si_base/renderer/mesh.h"
#include "si_base/renderer/sub_mesh.h"
#include "si_base/renderer/material.h"
#include "si_base/renderer/geometry.h"

namespace SI
{
	ModelSerializeData::ModelSerializeData(ModelSerializeData&& src)
		: m_rootNode(src.m_rootNode)
		, m_nodes(src.m_nodes)     
		, m_nodeCores(src.m_nodeCores) 
		, m_meshes(src.m_meshes)    
		, m_subMeshes(src.m_subMeshes)
		, m_materials(src.m_materials)
		, m_geometries(src.m_geometries)
		, m_strings(src.m_strings)
		, m_stringPool(src.m_stringPool)
	{
		// 確保した領域の管理を委譲して、コピー元をクリアする.
		src.m_rootNode = NodeSerializeData();
		src.m_nodes.Reset();
		src.m_nodeCores.Reset();
		src.m_meshes.Reset();
		src.m_subMeshes.Reset();
		src.m_materials.Reset();
		src.m_geometries.Reset();
		src.m_strings.Reset();
		src.m_stringPool.Reset();
	}

	void ModelSerializeData::Initialize(const Model& model)
	{
		m_rootNode   = model.GetRootNode().ConvertSerializeData();

		ConstArray<Node> srcNodes = model.GetNodes();
		uint32_t nodeCount = srcNodes.GetItemCount();
		if(0 < nodeCount)
		{
			SI_ASSERT(!m_nodes.IsValid());
			m_nodes.Setup(SI_NEW_ARRAY(NodeSerializeData, nodeCount), nodeCount);
			for(uint32_t i=0; i<nodeCount; ++i)
			{
				m_nodes[i] = srcNodes.GetItem(i).ConvertSerializeData();
			}
		}

		ConstArray<NodeCore> srcNodeCores = model.GetNodeCores();
		uint32_t nodeCoreCount = srcNodeCores.GetItemCount();
		if(0 < nodeCoreCount)
		{
			SI_ASSERT(!m_nodeCores.IsValid());
			m_nodeCores.Setup(SI_NEW_ARRAY(NodeCoreSerializeData, nodeCoreCount), nodeCoreCount);
			for(uint32_t i=0; i<nodeCoreCount; ++i)
			{
				m_nodeCores[i] = srcNodeCores.GetItem(i).ConvertSerializeData();
			}
		}
		
		ConstArray<Mesh> srcMeshes = model.GetMeshes();
		uint32_t meshCount = srcMeshes.GetItemCount();
		if(0 < meshCount)
		{
			SI_ASSERT(!m_meshes.IsValid());
			m_meshes.Setup(SI_NEW_ARRAY(MeshSerializeData, meshCount), meshCount);
			for(uint32_t i=0; i<meshCount; ++i)
			{
				m_meshes[i] = srcMeshes.GetItem(i).ConvertSerializeData();
			}
		}
		
		ConstArray<SubMesh> srcSubMeshes = model.GetSubMeshes();
		uint32_t subMeshCount = srcSubMeshes.GetItemCount();
		if(0 < subMeshCount)
		{
			SI_ASSERT(!m_subMeshes.IsValid());
			m_subMeshes.Setup(SI_NEW_ARRAY(SubMeshSerializeData, subMeshCount), subMeshCount);
			for(uint32_t i=0; i<subMeshCount; ++i)
			{
				m_subMeshes[i] = srcSubMeshes.GetItem(i).ConvertSerializeData();
			}
		}
		
		ConstArray<const Material*> srcMaterials = model.GetMaterials();
		uint32_t materialCount = srcMaterials.GetItemCount();
		if(0 < materialCount)
		{
			SI_ASSERT(!m_materials.IsValid());
			m_materials.Setup(SI_NEW_ARRAY(MaterialSerializeData, materialCount), materialCount);
			for(uint32_t i=0; i<materialCount; ++i)
			{
				m_materials[i] = srcMaterials.GetItem(i)->ConvertSerializeData();
			}
		}
		
		ConstArray<const Geometry*> srcGeometies = model.GetGeometries();
		uint32_t geometryCount = srcGeometies.GetItemCount();
		if(0 < geometryCount)
		{
			SI_ASSERT(!m_geometries.IsValid());
			m_geometries.Setup(SI_NEW_ARRAY(GeometrySerializeData, geometryCount), geometryCount);
			for(uint32_t i=0; i<geometryCount; ++i)
			{
				m_geometries[i] = srcGeometies.GetItem(i)->ConvertSerializeData();
			}
		}
		
		ConstArray<SI::LongObjectIndex> srcStrings = model.GetStrings();
		uint32_t stringCount = srcStrings.GetItemCount();
		if(0 < stringCount)
		{
			SI_ASSERT(!m_strings.IsValid());
			m_strings.Setup(SI_NEW_ARRAY(SI::LongObjectIndex, stringCount), stringCount);
			for(uint32_t i=0; i<stringCount; ++i)
			{
				m_strings[i] = srcStrings[i];
			}
		}
		
		ConstArray<char> srcStringPool = model.GetStringPool();
		uint32_t stringPoolCount = srcStringPool.GetItemCount();
		if(0 < stringPoolCount)
		{
			SI_ASSERT(!m_stringPool.IsValid());
			m_stringPool.Setup(SI_NEW_ARRAY(char, stringPoolCount), stringPoolCount);
			for(uint32_t i=0; i<stringPoolCount; ++i)
			{
				m_stringPool[i] = srcStringPool[i];
			}
		}
	}

	void ModelSerializeData::Terminate()
	{
		if(m_nodes.IsValid())
		{
			NodeSerializeData* addr = m_nodes.GetItemsAddr();
			SI_DELETE_ARRAY(addr);
			m_nodes.Reset();
		}
		if(m_nodeCores.IsValid())
		{
			NodeCoreSerializeData* addr = m_nodeCores.GetItemsAddr();
			SI_DELETE_ARRAY(addr);
			m_nodeCores.Reset();
		}
		if(m_meshes.IsValid())
		{
			MeshSerializeData* addr = m_meshes.GetItemsAddr();
			SI_DELETE_ARRAY(addr);
			m_meshes.Reset();
		}
		if(m_subMeshes.IsValid())
		{
			SubMeshSerializeData* addr = m_subMeshes.GetItemsAddr();
			SI_DELETE_ARRAY(addr);
			m_subMeshes.Reset();
		}
		if(m_materials.IsValid())
		{
			MaterialSerializeData* addr = m_materials.GetItemsAddr();
			SI_DELETE_ARRAY(addr);
			m_materials.Reset();
		}
		if(m_geometries.IsValid())
		{
			GeometrySerializeData* addr = m_geometries.GetItemsAddr();
			SI_DELETE_ARRAY(addr);
			m_geometries.Reset();
		}
		if(m_strings.IsValid())
		{
			uint32_t* addr = m_strings.GetItemsAddr();
			SI_DELETE_ARRAY(addr);
			m_strings.Reset();
		}
		if(m_stringPool.IsValid())
		{
			char* addr = m_stringPool.GetItemsAddr();
			SI_DELETE_ARRAY(addr);
			m_stringPool.Reset();
		}
	}

	Model::Model()
	{
	}
	
	Model::~Model()
	{
		DeallocateNodes();
		DeallocateNodeCores();
		DeallocateMeshes();
		DeallocateSubMeshes();
		DeallocateMaterials();
		DeallocateGeometries();
		DeallocateStrings();
		
		SI_ASSERT(!m_nodes.IsValid());
		SI_ASSERT(!m_nodeCores.IsValid());
		SI_ASSERT(!m_meshes.IsValid());
		SI_ASSERT(!m_subMeshes.IsValid());
		SI_ASSERT(!m_materials.IsValid());
		SI_ASSERT(!m_geometries.IsValid());
		SI_ASSERT(!m_strings.IsValid());
	}
	
	void Model::AllocateNodes(ObjectIndex size)
	{
		if(size==0) return;

		SI_ASSERT(!m_nodes.IsValid());
		m_nodes.Setup(SI_NEW_ARRAY(Node, size), (uint32_t)size);
	}
	
	void Model::AllocateNodeCores(ObjectIndex size)
	{
		if(size==0) return;

		SI_ASSERT(!m_nodeCores.IsValid());
		m_nodeCores.Setup(SI_NEW_ARRAY(NodeCore, size), (uint32_t)size);
	}

	void Model::AllocateMeshes(ObjectIndex size)
	{
		if(size==0) return;

		SI_ASSERT(!m_meshes.IsValid());
		m_meshes.Setup(SI_NEW_ARRAY(Mesh, size), (uint32_t)size);
	}

	void Model::AllocateSubMeshes(ObjectIndex size)
	{
		if(size==0) return;

		SI_ASSERT(!m_subMeshes.IsValid());
		m_subMeshes.Setup(SI_NEW_ARRAY(SubMesh, size), (uint32_t)size);
	}

	void Model::AllocateMaterials(ObjectIndex size)
	{
		if(size==0) return;

		SI_ASSERT(!m_materials.IsValid());
		m_materials.Setup(SI_NEW_ARRAY(Material*, size), (uint32_t)size);

		for(uint32_t i=0; i<size; ++i)
		{
			m_materials[i] = nullptr;
		}
	}
	
	void Model::AllocateGeometries(ObjectIndex size)
	{
		if(size==0) return;

		SI_ASSERT(!m_geometries.IsValid());
		m_geometries.Setup(SI_NEW_ARRAY(Geometry*, size), (uint32_t)size);

		for(uint32_t i=0; i<size; ++i)
		{
			m_geometries[i] = nullptr;
		}
	}

	void Model::AllocateStrings(ObjectIndex size)
	{
		if(size==0) return;
		SI_ASSERT(!m_strings.IsValid());
		m_strings.Setup(SI_NEW_ARRAY(LongObjectIndex, size), (uint32_t)size);
			
		for(uint32_t i=0; i<size; ++i)
		{
			m_strings[i] = kInvalidLongObjectIndex;
		}
	}

	void Model::AllocateStringPool(LongObjectIndex size)
	{
		if(size==0) return;
		SI_ASSERT(!m_stringPool.IsValid());
		m_stringPool.Setup(SI_NEW_ARRAY(char, size), (uint32_t)size);
			
		m_stringPool[0] = 0;
	}
	
	void Model::DeallocateNodes()
	{
		if(!m_nodes.IsValid()) return;
				
		Node* nodes =m_nodes.GetItemsAddr();
		SI_DELETE_ARRAY(nodes);
		m_nodes.Reset();
	}

	void Model::DeallocateNodeCores()
	{
		if(!m_nodeCores.IsValid()) return;
		
		NodeCore* nodeCores =m_nodeCores.GetItemsAddr();
		SI_DELETE_ARRAY(nodeCores);
		m_nodeCores.Reset();
	}

	void Model::DeallocateMeshes()
	{
		if(!m_meshes.IsValid()) return;
		
		Mesh* meshes =m_meshes.GetItemsAddr();
		SI_DELETE_ARRAY(meshes);
		m_meshes.Reset();
	}

	void Model::DeallocateSubMeshes()
	{
		if(!m_subMeshes.IsValid()) return;
		
		SubMesh* subMeshes =m_subMeshes.GetItemsAddr();
		SI_DELETE_ARRAY(subMeshes);
		m_subMeshes.Reset();
	}

	void Model::DeallocateMaterials()
	{
		if(!m_materials.IsValid()) return;

		uint32_t materialCount = m_materials.GetItemCount();
		for(uint32_t i=0; i<materialCount; ++i)
		{
			Material::Release(m_materials[i]);
		}

		Material** materials = m_materials.GetItemsAddr();
		SI_DELETE_ARRAY(materials);
		m_materials.Reset();
	}

	void Model::DeallocateGeometries()
	{
		if(!m_geometries.IsValid()) return;

		uint32_t geometoryCount = m_geometries.GetItemCount();
		for(uint32_t i=0; i<geometoryCount; ++i)
		{
			Geometry::Release(m_geometries[i]);
		}

		Geometry** geometries = m_geometries.GetItemsAddr();
		SI_DELETE_ARRAY(geometries);
		m_geometries.Reset();
	}

	void Model::DeallocateStrings()
	{
		if(!m_strings.IsValid()) return;
		
		LongObjectIndex* strings = m_strings.GetItemsAddr();
		SI_DELETE_ARRAY(strings);
		m_strings.Reset();
	}

	void Model::DeallocateStringPool()
	{
		if(!m_stringPool.IsValid()) return;
		
		char* stringPool = m_stringPool.GetItemsAddr();
		SI_DELETE_ARRAY(stringPool);
		m_stringPool.Reset();
	}
	
} // namespace SI
