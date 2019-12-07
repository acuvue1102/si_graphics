#pragma once

#include <memory>
#include <vector>
#include <algorithm>

#include "si_base/renderer/scenes.h"
#include "si_base/renderer/renderer_graphics_state.h"
#include "si_base/renderer/renderer_draw_stage.h"

namespace SI
{
	class ScenesInstance;
	using ScenesInstancePtr = std::shared_ptr<ScenesInstance>;

	class ScenesInstance : public IScenes
	{
	public:
		ScenesInstance()
		{}

		ScenesInstance(const ScenesPtr& original)
			: m_original(original)
		{}

		ScenesInstance(const ScenesInstancePtr& originalIns)
			: m_originalIns(originalIns)
		{
		}

		ScenesInstance(const ScenesInstance& ins) = delete;

		~ScenesInstance()
		{
			ReleaseAll();
		}

		void ReleaseScenes       (){ for( auto* s : m_scenes )       { if(s) SI_DELETE(s); } m_scenes.clear();       }
		void ReleaseNodes        (){ for( auto* n : m_nodes )        { if(n) SI_DELETE(n); } m_nodes.clear();        }
		void ReleaseMeshes       (){ for( auto* m : m_meshes )       { if(m) SI_DELETE(m); } m_meshes.clear();       }
		void ReleaseMaterials    (){ for( auto* m : m_materials )    { if(m) SI_DELETE(m); } m_materials.clear();    }
		void ReleaseAccessors    (){ for( auto* a : m_accessors )    { if(a) SI_DELETE(a); } m_accessors.clear();    }
		void ReleaseTextureInfos (){ for( auto* t : m_textureInfos ) { if(t) SI_DELETE(t); } m_textureInfos.clear(); }
		void ReleaseImages       (){ for( auto* i : m_images )       { if(i) SI_DELETE(i); } m_images.clear();       }

		void ReleaseAll()
		{
			ReleaseScenes        ();
			ReleaseNodes         ();
			ReleaseMeshes        ();
			ReleaseMaterials     ();
			ReleaseAccessors     ();
			ReleaseTextureInfos  ();
			ReleaseImages        ();
		}

		void AllocateScenes      (uint32_t sceneCount)      { m_scenes.resize(sceneCount);            std::fill(m_scenes.begin(),       m_scenes.end(),       nullptr); }
		void AllocateNodes       (uint32_t nodeCount)       { m_nodes.resize(nodeCount);              std::fill(m_nodes.begin(),        m_nodes.end(),        nullptr); }
		void AllocateMeshes      (uint32_t meshCount)       { m_meshes.resize(meshCount);             std::fill(m_meshes.begin(),       m_meshes.end(),       nullptr); }
		void AllocateMaterials   (uint32_t materialCount)   { m_materials.resize(materialCount);      std::fill(m_materials.begin(),    m_materials.end(),    nullptr); }
		void AllocateAccessors   (uint32_t accessorCount)   { m_accessors.resize(accessorCount);      std::fill(m_accessors.begin(),    m_accessors.end(),    nullptr); }
		void AllocateTextureInfos(uint32_t textureCount)    { m_textureInfos.resize(textureCount);    std::fill(m_textureInfos.begin(), m_textureInfos.end(), nullptr); }
		void AllocateImages      (uint32_t imageCount)      { m_images.resize(imageCount);            std::fill(m_images.begin(),       m_images.end(),       nullptr); }

		void AllocateScenes      () { AllocateScenes     (m_originalIns? m_originalIns->GetSceneCount        () : (uint32_t)m_original->m_scenes.size());       }
		void AllocateNodes       () { AllocateNodes      (m_originalIns? m_originalIns->GetNodeCount         () : (uint32_t)m_original->m_nodes.size());        }
		void AllocateMeshes      () { AllocateMeshes     (m_originalIns? m_originalIns->GetMeshCount         () : (uint32_t)m_original->m_meshes.size());       }
		void AllocateMaterials   () { AllocateMaterials  (m_originalIns? m_originalIns->GetMaterialCount     () : (uint32_t)m_original->m_materials.size());    }
		void AllocateAccessors   () { AllocateAccessors  (m_originalIns? m_originalIns->GetAccessorCount     () : (uint32_t)m_original->m_accessors.size());    }
		void AllocateTextureInfos() { AllocateAccessors  (m_originalIns? m_originalIns->GetTextureInfoCount  () : (uint32_t)m_original->m_textureInfos.size()); }
		void AllocateImages      () { AllocateAccessors  (m_originalIns? m_originalIns->GetImageCount        () : (uint32_t)m_original->m_images.size());       }

		void CloneScenes      () { AllocateScenes       (); uint32_t s = GetSceneCount       (); for(uint32_t i=0; i<s; ++i){ CloneScene       (i); } }
		void CloneNodes       () { AllocateNodes        (); uint32_t s = GetNodeCount        (); for(uint32_t i=0; i<s; ++i){ CloneNode        (i); } }
		void CloneMeshes      () { AllocateMeshes       (); uint32_t s = GetMeshCount        (); for(uint32_t i=0; i<s; ++i){ CloneMesh        (i); } }
		void CloneMaterials   () { AllocateMaterials    (); uint32_t s = GetMaterialCount    (); for(uint32_t i=0; i<s; ++i){ CloneMaterial    (i); } }
		void CloneAccessors   () { AllocateAccessors    (); uint32_t s = GetAccessorCount    (); for(uint32_t i=0; i<s; ++i){ CloneAccessor    (i); } }
		void CloneTextureInfos() { AllocateTextureInfos (); uint32_t s = GetTextureInfoCount (); for(uint32_t i=0; i<s; ++i){ CloneTextureInfo (i); } }
		void CloneImages      () { AllocateImages       (); uint32_t s = GetImageCount       (); for(uint32_t i=0; i<s; ++i){ CloneImage       (i); } }

		Scene&                AllocateScene       (uint32_t id) { if(!m_scenes[id])      { m_scenes[id]       = SI_NEW(Scene);       }  return  *m_scenes[id];       }
		Node&                 AllocateNode        (uint32_t id) { if(!m_nodes[id])       { m_nodes[id]        = SI_NEW(Node);        }  return  *m_nodes[id];        }
		Mesh&                 AllocateMesh        (uint32_t id) { if(!m_meshes[id])      { m_meshes[id]       = SI_NEW(Mesh);        }  return  *m_meshes[id];       }
		Material&             AllocateMaterial    (uint32_t id) { if(!m_materials[id])   { m_materials[id]    = SI_NEW(Material);    }  return  *m_materials[id];    }
		Accessor&             AllocateAccessor    (uint32_t id) { if(!m_accessors[id])   { m_accessors[id]    = SI_NEW(Accessor);    }  return  *m_accessors[id];    }
		TextureInfo&          AllocateTextureInfo (uint32_t id) { if(!m_textureInfos[id]){ m_textureInfos[id] = SI_NEW(TextureInfo); }  return  *m_textureInfos[id]; }
		GfxTexture&           AllocateImage       (uint32_t id) { if(!m_images[id])      { m_images[id]       = SI_NEW(GfxTexture);  }  return  *m_images[id];       }

		Scene&                CloneScene       (uint32_t id) { Scene&        ret = AllocateScene       (id); ret = m_originalIns? m_originalIns->GetScene        (id) : m_original->m_scenes[id];       return ret; }
		Node&                 CloneNode        (uint32_t id) { Node&         ret = AllocateNode        (id); ret = m_originalIns? m_originalIns->GetNode         (id) : m_original->m_nodes[id];        return ret; }
		Mesh&                 CloneMesh        (uint32_t id) { Mesh&         ret = AllocateMesh        (id); ret = m_originalIns? m_originalIns->GetMesh         (id) : m_original->m_meshes[id];       return ret; }
		Material&             CloneMaterial    (uint32_t id) { Material&     ret = AllocateMaterial    (id); ret = m_originalIns? m_originalIns->GetMaterial     (id) : m_original->m_materials[id];    return ret; }
		Accessor&             CloneAccessor    (uint32_t id) { Accessor&     ret = AllocateAccessor    (id); ret = m_originalIns? m_originalIns->GetAccessor     (id) : m_original->m_accessors[id];    return ret; }
		TextureInfo&          CloneTextureInfo (uint32_t id) { TextureInfo&  ret = AllocateTextureInfo (id); ret = m_originalIns? m_originalIns->GetTextureInfo  (id) : m_original->m_textureInfos[id]; return ret; }
		GfxTexture&           CloneImage       (uint32_t id) { GfxTexture&   ret = AllocateImage       (id); ret = m_originalIns? m_originalIns->GetImage        (id) : m_original->m_images[id];       return ret; }
																						 
		uint32_t   GetSceneCount        () const override{ return !m_scenes.empty()?        (uint32_t)m_scenes.size()         : m_originalIns? m_originalIns->GetSceneCount        () : (uint32_t)m_original->m_scenes.size()       ; }
		uint32_t   GetNodeCount         () const override{ return !m_nodes.empty()?         (uint32_t)m_nodes.size()          : m_originalIns? m_originalIns->GetNodeCount         () : (uint32_t)m_original->m_nodes.size()        ; }
		uint32_t   GetMeshCount         () const override{ return !m_meshes.empty()?        (uint32_t)m_meshes.size()         : m_originalIns? m_originalIns->GetMeshCount         () : (uint32_t)m_original->m_meshes.size()       ; }
		uint32_t   GetMaterialCount     () const override{ return !m_materials.empty()?     (uint32_t)m_materials.size()      : m_originalIns? m_originalIns->GetMaterialCount     () : (uint32_t)m_original->m_materials.size()    ; }
		uint32_t   GetAccessorCount     () const override{ return !m_accessors.empty()?     (uint32_t)m_accessors.size()      : m_originalIns? m_originalIns->GetAccessorCount     () : (uint32_t)m_original->m_accessors.size()    ; }
		uint32_t   GetTextureInfoCount  () const override{ return !m_textureInfos.empty()?  (uint32_t)m_textureInfos.size()   : m_originalIns? m_originalIns->GetTextureInfoCount  () : (uint32_t)m_original->m_textureInfos.size() ; }
		uint32_t   GetImageCount        () const override{ return !m_images.empty()?        (uint32_t)m_images.size()         : m_originalIns? m_originalIns->GetImageCount        () : (uint32_t)m_original->m_images.size()       ; }

		Scene&           GetScene        (uint32_t id) override{ return ( (!m_scenes.empty()       && m_scenes[id])?       *m_scenes[id] :         m_originalIns? m_originalIns->GetScene        (id) : m_original->m_scenes[id]        );  }
		Node&            GetNode         (uint32_t id) override{ return ( (!m_nodes.empty()        && m_nodes[id])?        *m_nodes[id] :          m_originalIns? m_originalIns->GetNode         (id) : m_original->m_nodes[id]         );  }
		Mesh&            GetMesh         (uint32_t id) override{ return ( (!m_meshes.empty()       && m_meshes[id])?       *m_meshes[id] :         m_originalIns? m_originalIns->GetMesh         (id) : m_original->m_meshes[id]        );  }
		Material&        GetMaterial     (uint32_t id) override{ return ( (!m_materials.empty()    && m_materials[id])?    *m_materials[id] :      m_originalIns? m_originalIns->GetMaterial     (id) : m_original->m_materials[id]     );  }
		Accessor&        GetAccessor     (uint32_t id) override{ return ( (!m_accessors.empty()    && m_accessors[id])?    *m_accessors[id] :      m_originalIns? m_originalIns->GetAccessor     (id) : m_original->m_accessors[id]     );  }
		TextureInfo&     GetTextureInfo  (uint32_t id) override{ return ( (!m_textureInfos.empty() && m_textureInfos[id])? *m_textureInfos[id] :   m_originalIns? m_originalIns->GetTextureInfo  (id) : m_original->m_textureInfos[id]  );  }
		GfxTexture&      GetImage        (uint32_t id) override{ return ( (!m_images.empty()       && m_images[id])?       *m_images[id] :         m_originalIns? m_originalIns->GetImage        (id) : m_original->m_images[id]        );  }

		const Scene&           GetScene        (uint32_t id) const override{ return ( (!m_scenes.empty()       && m_scenes[id])?       *m_scenes[id] :         m_originalIns? m_originalIns->GetScene        (id) : m_original->m_scenes[id]        );  }
		const Node&            GetNode         (uint32_t id) const override{ return ( (!m_nodes.empty()        && m_nodes[id])?        *m_nodes[id] :          m_originalIns? m_originalIns->GetNode         (id) : m_original->m_nodes[id]         );  }
		const Mesh&            GetMesh         (uint32_t id) const override{ return ( (!m_meshes.empty()       && m_meshes[id])?       *m_meshes[id] :         m_originalIns? m_originalIns->GetMesh         (id) : m_original->m_meshes[id]        );  }
		const Material&        GetMaterial     (uint32_t id) const override{ return ( (!m_materials.empty()    && m_materials[id])?    *m_materials[id] :      m_originalIns? m_originalIns->GetMaterial     (id) : m_original->m_materials[id]     );  }
		const Accessor&        GetAccessor     (uint32_t id) const override{ return ( (!m_accessors.empty()    && m_accessors[id])?    *m_accessors[id] :      m_originalIns? m_originalIns->GetAccessor     (id) : m_original->m_accessors[id]     );  }
		const TextureInfo&     GetTextureInfo  (uint32_t id) const override{ return ( (!m_textureInfos.empty() && m_textureInfos[id])? *m_textureInfos[id] :   m_originalIns? m_originalIns->GetTextureInfo  (id) : m_original->m_textureInfos[id]  );  }
		const GfxTexture&      GetImage        (uint32_t id) const override{ return ( (!m_images.empty()       && m_images[id])?       *m_images[id] :         m_originalIns? m_originalIns->GetImage        (id) : m_original->m_images[id]        );  }

		RendererDrawStageList& GetDrawStageList(){ return m_drawStageList; }

		void Setup();

	public:
		static ScenesInstancePtr Create(const ScenesPtr& original)
		{
			ScenesInstancePtr ret = std::make_shared<ScenesInstance>(original);
			ret->Setup();
			return std::move(ret);
		}

		static ScenesInstancePtr Create(const ScenesInstancePtr& originalIns){ return std::make_shared<ScenesInstance>(originalIns); }

	private:
		// 各要素を上書きできるように.
		std::vector<Scene*>                m_scenes;
		std::vector<Node*>                 m_nodes;
		std::vector<Mesh*>                 m_meshes;
		std::vector<Material*>             m_materials;
		std::vector<Accessor*>             m_accessors;
		std::vector<TextureInfo*>          m_textureInfos;
		std::vector<GfxTexture*>           m_images;

		ScenesInstancePtr                  m_originalIns;
		ScenesPtr                          m_original;
		RendererDrawStageList              m_drawStageList;
	};
	
} // namespace SI
