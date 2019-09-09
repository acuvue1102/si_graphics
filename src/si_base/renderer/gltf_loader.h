#pragma once

#include <memory>
#include <vector>
#include "si_base/core/assert.h"
#include "si_base/misc/string.h"
#include "si_base/math/math.h"

#include "si_base/gpu/gfx.h"

namespace SI
{
	class Accessor
	{
	public:
		Accessor()
			: m_bufferViewId(-1)
			, m_count(0)
			, m_format(GfxFormat::Unknown)
		{
		}
		
		void SetBufferViewId(int bufferViewId){ m_bufferViewId = bufferViewId; }
		int GetBufferViewId() const{ return m_bufferViewId; }

		void SetFormat(GfxFormat format){ m_format = format; }
		GfxFormat GetFormat() const{ return m_format; }

		void SetCount(size_t count){ m_count = count; }
		size_t GetCount() const{ return m_count; }

	private:
		int                  m_bufferViewId;
		size_t               m_count;
		GfxFormat            m_format;
	};

	class Material
	{
	public:
		Material()
		{
		}

		void SetName(const char* name){ m_name = name; }
		const char* GetName() const{ return m_name.c_str(); }

	private:
		String m_name;
	};

	struct VertexAttribute
	{
		GfxSemantics m_semantics;
		int          m_accessorId;
	};

	class SubMesh
	{
	public:
		SubMesh()
			: m_topology(SI::GfxPrimitiveTopology::TriangleList)
			, m_materialId(-1)
			, m_indicesAccessorId(-1)
		{}

		SI::GfxPrimitiveTopology GetTopology() const{ return m_topology; }
		void SetTopology(SI::GfxPrimitiveTopology topology){ m_topology = topology; }

		int GetMaterialId() const{ return m_materialId; }
		void SetMaterialId(int materialId){ m_materialId = materialId; }

		int GetIndicesAccessorId() const{ return m_indicesAccessorId; }
		void SetIndicesAccessorId(int accessorId){ m_indicesAccessorId = accessorId; }

		void ReserveVertexAttribute(size_t attributeCount){ m_vertexAttributes.reserve(attributeCount); }
		void AddVertexAttribute(const VertexAttribute& attribute){ m_vertexAttributes.push_back(attribute); }
		size_t GetVertexAttributeCount() const{ return m_vertexAttributes.size(); }
		VertexAttribute& GetVertexAttribute(size_t id){ return m_vertexAttributes[id]; }

	private:
		SI::GfxPrimitiveTopology m_topology;
		int m_materialId;
		int m_indicesAccessorId;
		std::vector<VertexAttribute> m_vertexAttributes;
	};

	class Mesh
	{
	public:
		Mesh(){}
		~Mesh()
		{
			for(SubMesh* subMesh : m_subMeshes)
			{
				delete subMesh;
			}
			m_subMeshes.clear();
		}

		void SetName(const char* name){ m_name = name; }
		const char* GetName() const{ return m_name.c_str(); }

		void ReserveSubmeshes(size_t subMeshCount){ m_subMeshes.reserve(subMeshCount); }
		SubMesh* CreateNewSubMesh(){ m_subMeshes.push_back(new SubMesh); return m_subMeshes.back(); }

	private:
		String m_name;
		std::vector<SubMesh*> m_subMeshes;
	};

	class Node
	{
	public:
		Node()
			: m_mesh(nullptr)
		{}

		~Node(){}

		void ReserveNodes(size_t nodeCount){ m_childrenNodes.reserve(nodeCount); }
		void AddNodeId(int nodeIndex){ m_childrenNodes.push_back(nodeIndex); }

		int GetChildrenNodeCount() const{ return (int)m_childrenNodes.size(); }
		int GetChildrenNodeId(int index) const{ return m_childrenNodes[index]; }

		void SetMesh(Mesh* mesh){ m_mesh = mesh; }

		void SetName(const char* name){ m_name = name; }
		const char* GetName() const{ return m_name.c_str(); }

		void SetMatrix(Vfloat4x4_arg matrix){ m_matrix = matrix; }
		const Vfloat4x4& GetMatrix() const{ return m_matrix; }

	private:
		String m_name;
		std::vector<int> m_childrenNodes;
		Mesh* m_mesh;
		Vfloat4x4  m_matrix;
	};

	class Scene
	{
	public:
		Scene(){}
		~Scene()
		{
		}

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

	class RootScene;
	using RootScenePtr = std::shared_ptr<RootScene>;

	class RootScene
	{
	public:
		RootScene()
		{
		}

		void AllocateScenes     (size_t sceneCount)      { m_scenes.resize(sceneCount); }
		void AllocateNodes      (size_t nodeCount)       { m_nodes.resize(nodeCount); }
		void AllocateMeshes     (size_t meshCount)       { m_meshes.resize(meshCount); }
		void AllocateMaterials  (size_t materialCount)   { m_materials.resize(materialCount); }
		void AllocateAccessors  (size_t accessorCount)   { m_accessors.resize(accessorCount); }
		void AllocateBufferViews(size_t bufferViewCount) { m_bufferViews.resize(bufferViewCount); }
		void AllocateBuffers    (size_t bufferCount)     { m_buffers.resize(bufferCount); }

		size_t   GetSceneCount     () const{ return m_scenes.size(); }
		size_t   GetNodeCount      () const{ return m_nodes.size(); }
		size_t   GetMeshCount      () const{ return m_meshes.size(); }
		size_t   GetMaterialCount  () const{ return m_materials.size(); }
		size_t   GetAccessorCount  () const{ return m_accessors.size(); }
		size_t   GetBufferViewCount() const{ return m_bufferViews.size(); }
		size_t   GetBufferCount    () const{ return m_buffers.size(); }

		Scene&                GetScene     (size_t id){ return m_scenes[id]; }
		Node&                 GetNode      (size_t id){ return m_nodes[id]; }
		Mesh&                 GetMesh      (size_t id){ return m_meshes[id]; }
		Material&             GetMaterial  (size_t id){ return m_materials[id]; }
		Accessor&             GetAccessor  (size_t id){ return m_accessors[id]; }
		GfxVertexBufferView&  GetBufferView(size_t id){ return m_bufferViews[id]; }
		GfxBuffer&            GetBuffer    (size_t id){ return m_buffers[id]; }

	private:
		std::vector<Scene>                m_scenes;
		std::vector<Node>                 m_nodes;
		std::vector<Mesh>                 m_meshes;
		std::vector<Material>             m_materials;
		std::vector<Accessor>             m_accessors;
		std::vector<GfxVertexBufferView>  m_bufferViews;
		std::vector<GfxBuffer>            m_buffers;

		RootScenePtr                      m_original; // when scene is cloned.
	};

	class GltfLoaderImpl;
	class GltfLoader
	{
	public:
		GltfLoader();
		~GltfLoader();

		RootScenePtr Load(const char* filePath);

	private:
		GltfLoaderImpl* m_impl;
	};

} // namespace SI
