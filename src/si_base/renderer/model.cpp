
#include "si_base/renderer/model.h"

#include "si_base/renderer/mesh.h"
#include "si_base/renderer/sub_mesh.h"
#include "si_base/renderer/material.h"
#include "si_base/renderer/geometry.h"

namespace SI
{
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
