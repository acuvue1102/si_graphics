#pragma once

#include <memory>
#include <vector>

#include "si_base/renderer/accessor.h"
#include "si_base/renderer/material.h"
#include "si_base/renderer/mesh.h"
#include "si_base/renderer/node.h"
#include "si_base/renderer/scene.h"

namespace SI
{
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
	
} // namespace SI
