
#include "si_base/renderer/model_instance.h"
#include "si_base/renderer/render_item.h"
#include "si_base/gpu/gfx_utility.h"
#include "si_base/gpu/gfx_input_layout.h"

namespace SI
{
	ModelInstance::ModelInstance()
		: m_model( std::make_shared<Model>() )
		, m_drawStageList()
	{
	}

	ModelInstance::ModelInstance(ModelPtr& model)
		: m_model(model)
	{
	}

	ModelInstance::~ModelInstance()
	{
	}
	
	void ModelInstance::Setup()
	{
		SI_ASSERT(m_drawStageList == nullptr);
		
		m_drawStageList = std::make_shared<RendererDrawStageList>();

		ConstArray<Material*> materials  = GetMaterials();

		// 使われているステージ数を数える.
		RendererDrawStageMask stageMask;
		uint32_t materialCount = materials.GetItemCount();
		for(uint32_t m=0; m<materialCount; ++m)
		{
			const Material* material = materials[m];
			stageMask |= material->GetDrawStageMask();
		}
		uint32_t stageCount = stageMask.GetStageCount();

		if(stageCount<=0) return;
		
		ConstArray<Geometry*> geometries = GetGeometries();
		uint32_t geometryCount = geometries.GetItemCount();
		
		// 各ステージのレンダーアイテムを作る.
		ConstArray<SubMesh> subMeshes = GetSubMeshes();
		uint32_t subMeshCount = subMeshes.GetItemCount();
		m_drawStageList->Reserve(stageCount);
		for(uint32_t s=0; s<stageCount; ++s)
		{
			RendererDrawStageType stageType = stageMask.GetFirstStage();
			SI_ASSERT((int)stageType<(int)RendererDrawStageType_Max);
			stageMask.DisableMaskBit(stageType);

			RendererDrawStage& stage = m_drawStageList->Add(stageType);
			
			// RenderItemの数を調べる
			uint32_t renderItemCount = 0;
			for(uint32_t sm=0; sm<subMeshCount; ++sm)
			{
				const SubMesh& subMesh = subMeshes[sm];
				ObjectIndex materialIndex = subMesh.GetMaterialIndex();
				if(materialCount <= materialIndex) continue;

				Material* material = materials[materialIndex];
				if(!material->GetDrawStageMask().CheckEnable(stageType)) continue;
				
				ObjectIndex geometryIndex = subMesh.GetGeometryIndex();
				if(geometryCount <= geometryIndex) continue;

				++renderItemCount;
			}
			
			stage.Initialize(renderItemCount);

			// RenderItemをセットアップ
			uint32_t ri = 0;
			for(uint32_t sm=0; sm<subMeshCount; ++sm)
			{
				const SubMesh& subMesh = subMeshes[sm];

				ObjectIndex materialIndex = subMesh.GetMaterialIndex();
				if(materialCount <= materialIndex) continue;
				Material* material = materials[materialIndex];
				RenderMaterial* renderMaterial = material->GetRenderMaterialSet(stageType);

				ObjectIndex geometryIndex = subMesh.GetGeometryIndex();
				if(geometryCount <= geometryIndex) continue;
				Geometry* geometry = geometries[geometryIndex];

				RenderItem& renderItem = stage.GetRenderItem(ri++);
				renderItem.Setup(material, renderMaterial, geometry);
			}
			SI_ASSERT(renderItemCount==ri);
		}
		SI_ASSERT(stageMask.GetStageCount()==0u);
	}

} // namespace SI
