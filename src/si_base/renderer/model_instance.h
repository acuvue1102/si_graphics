#pragma once

#include "si_base/container/array.h"
#include "si_base/renderer/model.h"
#include "si_base/renderer/renderer_graphics_state.h"
#include "si_base/renderer/renderer_draw_stage.h"

namespace SI
{
	class Model;
	class Node;
	class NodeCore;
	class Mesh;
	class SubMesh;
	class Material;
	class Geometry;
	
	class ModelInstance
	{
	public:
		ModelInstance();
		explicit ModelInstance(ModelPtr& model);
		~ModelInstance();
		
		Model*                 GetModel()           { return m_model.get(); }
		const Model*           GetModel()      const{ return m_model.get(); }
		
		const Node&            GetRootNode()   const{ return m_model->GetRootNode(); }
		
		Array<Node>            GetNodes()           { return m_model->GetNodes();    }
		ConstArray<Node>       GetNodes()      const{ return m_model->GetNodes();    }
		
		Array<NodeCore>        GetNodeCores()       { return m_nodeCores.IsValid()? m_nodeCores : m_model->GetNodeCores(); }
		ConstArray<NodeCore>   GetNodeCores()  const{ return m_nodeCores.IsValid()? m_nodeCores : m_model->GetNodeCores(); }
		 
		Array<Mesh>            GetMeshes()          { return m_model->GetMeshes();    }
		ConstArray<Mesh>       GetMeshes()     const{ return m_model->GetMeshes();    }

		Array<SubMesh>         GetSubMeshes()       { return m_model->GetSubMeshes();    }
		ConstArray<SubMesh>    GetSubMeshes()  const{ return m_model->GetSubMeshes();    }

		Array<Material*>       GetMaterials()       { return m_materials.IsValid()? m_materials : m_model->GetMaterials();    }
		ConstArray<Material*>  GetMaterials()  const{ return m_materials.IsValid()? m_materials : m_model->GetMaterials();    }

		Array<Geometry*>       GetGeometries()      { return m_geometries.IsValid()? m_geometries : m_model->GetGeometries();    }
		ConstArray<Geometry*>  GetGeometries() const{ return m_geometries.IsValid()? m_geometries : m_model->GetGeometries();    }

		RendererDrawStageList& GetDrawStageList(){ return *m_drawStageList; }
		
		void Setup();

		using RendererDrawStageListPtr = std::shared_ptr<RendererDrawStageList>;
	private:
		ModelPtr                  m_model;
		Array<NodeCore>           m_nodeCores;
		Array<Material*>          m_materials;
		Array<Geometry*>          m_geometries;

		RendererDrawStageListPtr  m_drawStageList;
	};
	
	using ModelInstancePtr = std::shared_ptr<ModelInstance>;

} // namespace SI
