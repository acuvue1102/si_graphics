
#include "si_base/renderer/model.h"

#include "si_base/renderer/mesh.h"
#include "si_base/renderer/sub_mesh.h"
#include "si_base/renderer/material.h"
#include "si_base/renderer/geometry.h"
#include "si_base/renderer/material_factory.h"

namespace SI
{
#if 0
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
		m_rootNode   = model.GetRootNode().ExportSerializeData();

		ConstArray<Node> srcNodes = model.GetNodes();
		uint32_t nodeCount = srcNodes.GetItemCount();
		if(0 < nodeCount)
		{
			SI_ASSERT(!m_nodes.IsValid());
			m_nodes.Setup(SI_NEW_ARRAY(NodeSerializeData, nodeCount), nodeCount);
			for(uint32_t i=0; i<nodeCount; ++i)
			{
				m_nodes[i] = srcNodes.GetItem(i).ExportSerializeData();
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
				m_nodeCores[i] = srcNodeCores.GetItem(i).ExportSerializeData();
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
				m_meshes[i] = srcMeshes.GetItem(i).ExportSerializeData();
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
				m_subMeshes[i] = srcSubMeshes.GetItem(i).ExportSerializeData();
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
				m_materials[i] = srcMaterials.GetItem(i)->ExportSerializeData();
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
				m_geometries[i] = srcGeometies.GetItem(i)->ExportSerializeData();
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
#endif

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
			MaterialFactory::Release(m_materials[i]);
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
	
	void Model::ExportSerializeData(ModelSerializeData& outData) const
	{
		GetRootNode().ExportSerializeData(outData.m_rootNode);

		ConstArray<Node> srcNodes = GetNodes();
		uint32_t nodeCount = srcNodes.GetItemCount();
		if(0 < nodeCount)
		{
			SI_ASSERT(!outData.m_nodes.IsValid());
			outData.m_nodes.Setup(SI_NEW_ARRAY(NodeSerializeData, nodeCount), nodeCount);
			for(uint32_t i=0; i<nodeCount; ++i)
			{
				srcNodes.GetItem(i).ExportSerializeData(outData.m_nodes[i]);
			}
		}

		ConstArray<NodeCore> srcNodeCores = GetNodeCores();
		uint32_t nodeCoreCount = srcNodeCores.GetItemCount();
		if(0 < nodeCoreCount)
		{
			SI_ASSERT(!outData.m_nodeCores.IsValid());
			outData.m_nodeCores.Setup(SI_NEW_ARRAY(NodeCoreSerializeData, nodeCoreCount), nodeCoreCount);
			for(uint32_t i=0; i<nodeCoreCount; ++i)
			{
				srcNodeCores.GetItem(i).ExportSerializeData(outData.m_nodeCores[i]);
			}
		}
		
		ConstArray<Mesh> srcMeshes = GetMeshes();
		uint32_t meshCount = srcMeshes.GetItemCount();
		if(0 < meshCount)
		{
			SI_ASSERT(!outData.m_meshes.IsValid());
			outData.m_meshes.Setup(SI_NEW_ARRAY(MeshSerializeData, meshCount), meshCount);
			for(uint32_t i=0; i<meshCount; ++i)
			{
				srcMeshes.GetItem(i).ExportSerializeData(outData.m_meshes[i]);
			}
		}
		
		ConstArray<SubMesh> srcSubMeshes = GetSubMeshes();
		uint32_t subMeshCount = srcSubMeshes.GetItemCount();
		if(0 < subMeshCount)
		{
			SI_ASSERT(!outData.m_subMeshes.IsValid());
			outData.m_subMeshes.Setup(SI_NEW_ARRAY(SubMeshSerializeData, subMeshCount), subMeshCount);
			for(uint32_t i=0; i<subMeshCount; ++i)
			{
				srcSubMeshes.GetItem(i).ExportSerializeData(outData.m_subMeshes[i]);
			}
		}
		
		ConstArray<const Material*> srcMaterials = GetMaterials();
		uint32_t materialCount = srcMaterials.GetItemCount();
		if(0 < materialCount)
		{
			SI_ASSERT(!outData.m_materials.IsValid());
			outData.m_materials.Setup(SI_NEW_ARRAY(MaterialSerializeData, materialCount), materialCount);
			for(uint32_t i=0; i<materialCount; ++i)
			{
				srcMaterials.GetItem(i)->ExportSerializeData(outData.m_materials[i]);
			}
		}
		
		ConstArray<const Geometry*> srcGeometies = GetGeometries();
		uint32_t geometryCount = srcGeometies.GetItemCount();
		if(0 < geometryCount)
		{
			SI_ASSERT(!outData.m_geometries.IsValid());
			outData.m_geometries.Setup(SI_NEW_ARRAY(GeometrySerializeData, geometryCount), geometryCount);
			for(uint32_t i=0; i<geometryCount; ++i)
			{
				srcGeometies.GetItem(i)->ExportSerializeData(outData.m_geometries[i]);
			}
		}
		
		ConstArray<SI::LongObjectIndex> srcStrings = GetStrings();
		uint32_t stringCount = srcStrings.GetItemCount();
		if(0 < stringCount)
		{
			SI_ASSERT(!outData.m_strings.IsValid());
			outData.m_strings.Setup(SI_NEW_ARRAY(SI::LongObjectIndex, stringCount), stringCount);
			for(uint32_t i=0; i<stringCount; ++i)
			{
				outData.m_strings[i] = srcStrings[i];
			}
		}
		
		ConstArray<char> srcStringPool = GetStringPool();
		uint32_t stringPoolCount = srcStringPool.GetItemCount();
		if(0 < stringPoolCount)
		{
			SI_ASSERT(!outData.m_stringPool.IsValid());
			outData.m_stringPool.Setup(SI_NEW_ARRAY(char, stringPoolCount), stringPoolCount);
			for(uint32_t i=0; i<stringPoolCount; ++i)
			{
				outData.m_stringPool[i] = srcStringPool[i];
			}
		}
	}

	void Model::ImportSerializeData(const ModelSerializeData& serializeData)
	{
		m_rootNode.ImportSerializeData(serializeData.m_rootNode);
		
		uint32_t nodeCount = serializeData.m_nodes.GetItemCount();
		if(0 < nodeCount)
		{
			AllocateNodes(nodeCount);
			for(uint32_t i=0; i<nodeCount; ++i)
			{
				m_nodes[i].ImportSerializeData(serializeData.m_nodes.GetItem(i));
			}
		}

		uint32_t nodeCoreCount = serializeData.m_nodeCores.GetItemCount();
		if(0 < nodeCoreCount)
		{
			AllocateNodeCores(nodeCoreCount);
			for(uint32_t i=0; i<nodeCoreCount; ++i)
			{
				m_nodeCores[i].ImportSerializeData(serializeData.m_nodeCores.GetItem(i));
			}
		}
		
		uint32_t meshCount = serializeData.m_meshes.GetItemCount();
		if(0 < meshCount)
		{
			AllocateMeshes(meshCount);
			for(uint32_t i=0; i<meshCount; ++i)
			{
				m_meshes[i].ImportSerializeData(serializeData.m_meshes.GetItem(i));
			}
		}

		uint32_t subMeshCount = serializeData.m_subMeshes.GetItemCount();
		if(0 < subMeshCount)
		{
			AllocateSubMeshes(subMeshCount);
			for(uint32_t i=0; i<subMeshCount; ++i)
			{
				m_subMeshes[i].ImportSerializeData(serializeData.m_subMeshes.GetItem(i));
			}
		}
		
		uint32_t materialCount = serializeData.m_materials.GetItemCount();
		if(0 < materialCount)
		{
			AllocateMaterials(materialCount);
			for(uint32_t i=0; i<materialCount; ++i)
			{
				m_materials[i] = MaterialFactory::Create(MaterialType::Simple);
				m_materials[i]->ImportSerializeData(serializeData.m_materials.GetItem(i));
				m_materials[i]->SetupDrawStageMask();
			}
		}
		
		uint32_t geometryCount = serializeData.m_geometries.GetItemCount();
		if(0 < geometryCount)
		{
			AllocateGeometries(geometryCount);
			for(uint32_t i=0; i<geometryCount; ++i)
			{
				m_geometries[i] = SI_NEW(SI::Geometry);
				m_geometries[i]->ImportSerializeData(serializeData.m_geometries.GetItem(i));
			}
		}
		
		uint32_t stringCount = serializeData.m_strings.GetItemCount();
		if(0 < stringCount)
		{
			AllocateStrings(stringCount);
			memcpy(&m_strings[0], &serializeData.m_strings[0], sizeof(SI::LongObjectIndex) * stringCount);
		}
		
		uint32_t stringPoolCount = serializeData.m_stringPool.GetItemCount();
		if(0 < stringPoolCount)
		{
			AllocateStringPool(stringPoolCount);
			memcpy(&m_stringPool[0], &serializeData.m_stringPool[0], sizeof(char) * stringPoolCount);
		}
	}
	
} // namespace SI
