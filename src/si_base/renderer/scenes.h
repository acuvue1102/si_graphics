#pragma once

#include <memory>
#include <vector>

#include "si_base/renderer/accessor.h"
#include "si_base/renderer/buffer_view.h"
#include "si_base/renderer/material.h"
#include "si_base/renderer/mesh.h"
#include "si_base/renderer/node.h"
#include "si_base/renderer/scene.h"

namespace SI
{
	class ScenesInstance;
	class Scenes;
	using ScenesPtr = std::shared_ptr<Scenes>;

	class TextureInfo
	{
	public:
		TextureInfo()
			: m_imageId(-1)
			, m_samplerId(-1)
		{}

		void SetImageId(int id){ m_imageId = id; }
		int GetImageId() const{ return m_imageId; }

		void SetSamplerId(int id){ m_samplerId = id; }
		int GetSamplerId() const{ return m_samplerId; }

	private:
		int m_imageId;
		int m_samplerId;
	};

	class IScenes
	{
	public:
		virtual uint32_t GetSceneCount      () const = 0;
		virtual uint32_t GetNodeCount       () const = 0;
		virtual uint32_t GetMeshCount       () const = 0;
		virtual uint32_t GetMaterialCount   () const = 0;
		virtual uint32_t GetAccessorCount   () const = 0;
		virtual uint32_t GetTextureInfoCount() const = 0;
		virtual uint32_t GetImageCount      () const = 0;

		virtual Scene&                GetScene      (uint32_t id) = 0;
		virtual Node&                 GetNode       (uint32_t id) = 0;
		virtual Mesh&                 GetMesh       (uint32_t id) = 0;
		virtual Material&             GetMaterial   (uint32_t id) = 0;
		virtual Accessor&             GetAccessor   (uint32_t id) = 0;
		virtual TextureInfo&          GetTextureInfo(uint32_t id) = 0;
		virtual GfxTexture&           GetImage      (uint32_t id) = 0;

		virtual const Scene&          GetScene      (uint32_t id) const = 0;
		virtual const Node&           GetNode       (uint32_t id) const = 0;
		virtual const Mesh&           GetMesh       (uint32_t id) const = 0;
		virtual const Material&       GetMaterial   (uint32_t id) const = 0;
		virtual const Accessor&       GetAccessor   (uint32_t id) const = 0;
		virtual const TextureInfo&    GetTextureInfo(uint32_t id) const = 0;
		virtual const GfxTexture&     GetImage      (uint32_t id) const = 0;
	};

	class Scenes : public IScenes
	{
		friend class ScenesInstance;
	public:
		Scenes(){}
		~Scenes(){}

		void AllocateScenes       (size_t sceneCount)      { m_scenes.resize(sceneCount); }
		void AllocateNodes        (size_t nodeCount)       { m_nodes.resize(nodeCount); }
		void AllocateMeshes       (size_t meshCount)       { m_meshes.resize(meshCount); }
		void AllocateMaterials    (size_t materialCount)   { m_materials.resize(materialCount); }
		void AllocateAccessors    (size_t accessorCount)   { m_accessors.resize(accessorCount); }
		void AllocateTextureInfos (size_t textureInfoCount){ m_textureInfos.resize(textureInfoCount); }
		void AllocateImages       (size_t imageCount)      { m_images.resize(imageCount); }

		uint32_t  GetSceneCount      () const override{ return (uint32_t)m_scenes.size(); }
		uint32_t  GetNodeCount       () const override{ return (uint32_t)m_nodes.size(); }
		uint32_t  GetMeshCount       () const override{ return (uint32_t)m_meshes.size(); }
		uint32_t  GetMaterialCount   () const override{ return (uint32_t)m_materials.size(); }
		uint32_t  GetAccessorCount   () const override{ return (uint32_t)m_accessors.size(); }
		uint32_t  GetTextureInfoCount() const override{ return (uint32_t)m_textureInfos.size(); }
		uint32_t  GetImageCount      () const override{ return (uint32_t)m_images.size(); }

		Scene&                GetScene      (uint32_t id) override{ return m_scenes[id]; }
		Node&                 GetNode       (uint32_t id) override{ return m_nodes[id]; }
		Mesh&                 GetMesh       (uint32_t id) override{ return m_meshes[id]; }
		Material&             GetMaterial   (uint32_t id) override{ return m_materials[id]; }
		Accessor&             GetAccessor   (uint32_t id) override{ return m_accessors[id]; }
		TextureInfo&          GetTextureInfo(uint32_t id) override{ return m_textureInfos[id]; }
		GfxTexture&           GetImage      (uint32_t id) override{ return m_images[id]; }

		const Scene&                GetScene      (uint32_t id) const override{ return m_scenes[id]; }
		const Node&                 GetNode       (uint32_t id) const override{ return m_nodes[id]; }
		const Mesh&                 GetMesh       (uint32_t id) const override{ return m_meshes[id]; }
		const Material&             GetMaterial   (uint32_t id) const override{ return m_materials[id]; }
		const Accessor&             GetAccessor   (uint32_t id) const override{ return m_accessors[id]; }
		const TextureInfo&          GetTextureInfo(uint32_t id) const override{ return m_textureInfos[id]; }
		const GfxTexture&           GetImage      (uint32_t id) const override{ return m_images[id]; }

	public:
		static ScenesPtr Create(){ return std::make_shared<Scenes>(); }

	private:
		std::vector<Scene>                m_scenes;
		std::vector<Node>                 m_nodes;
		std::vector<Mesh>                 m_meshes;
		std::vector<Material>             m_materials;
		std::vector<Accessor>             m_accessors; // 頂点.
		std::vector<TextureInfo>          m_textureInfos;
		std::vector<GfxTexture>           m_images;
	};
	
} // namespace SI
