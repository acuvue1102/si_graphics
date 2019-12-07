
#include "si_base/renderer/scenes_instance.h"

#include "si_base/renderer/render_item.h"
#include "si_base/gpu/gfx_utility.h"
#include "si_base/gpu/gfx_input_layout.h"

namespace SI
{
	void ScenesInstance::Setup()
	{
		m_drawStageList.Clear();

		// 使われているステージ数を数える.
		uint32_t stageItemCounts[(int)RendererDrawStageType::Max] = {};
		RendererDrawStageMask allStageMask;
		uint32_t materialCount = GetMaterialCount();
		for(uint32_t m=0; m<materialCount; ++m)
		{
			const Material& material = GetMaterial(m);
			RendererDrawStageMask stageMask = material.GetDrawStageMask();
			allStageMask |= stageMask;

			while(!stageMask.IsEmpty())
			{
				RendererDrawStageType stageType = stageMask.GetFirstStage();
				SI_ASSERT((size_t)stageType < ArraySize(stageItemCounts));

				++(stageItemCounts[(int)stageType]);

				stageMask.DisableMaskBit(stageType);
			}
		}
		uint32_t stageCount = allStageMask.GetStageCount();

		if(stageCount<=0) return;

		// 数えたステージのメモリリザーブ.
		m_drawStageList.Reserve(stageCount);
		while(!allStageMask.IsEmpty())
		{
			RendererDrawStageType stageType = allStageMask.GetFirstStage();
			SI_ASSERT((size_t)stageType < ArraySize(stageItemCounts));
			m_drawStageList.Add(stageType).m_renderItems.reserve(stageItemCounts[(int)stageType]);

			allStageMask.DisableMaskBit(stageType);
		}

		RenderItem renderItem;
		renderItem.m_scenes = this;

		uint32_t nodeCount = GetNodeCount();
		for(uint32_t n=0; n<nodeCount; ++n)
		{
			Node& node = GetNode(n);
			int meshId = node.GetMeshId();
			if(meshId < 0) continue;
			SI_ASSERT((uint32_t)meshId < GetMeshCount());

			renderItem.m_worldMatrix = node.GetMatrix();

			Mesh& mesh = GetMesh(meshId);
			uint32_t subMeshCount = mesh.GetSubMeshCount();
			for(uint32_t s=0; s<subMeshCount; ++s)
			{
				SubMesh* subMesh = mesh.GetSubMesh(s);
				if(!subMesh) continue;

				renderItem.m_subMesh = subMesh;

				int materialId = subMesh->GetMaterialId();
				if(materialId<0) continue;

				int indexId = subMesh->GetIndicesAccessorId();
				if(indexId<0) continue;

				Accessor& indexAccessor = GetAccessor(indexId);

				renderItem.m_indexAccessor = &indexAccessor;
				renderItem.m_vertexAttributes = subMesh->GetVertexAttributes();

				Material& material = GetMaterial((uint32_t)materialId);
				renderItem.m_material = &material;

				RendererDrawStageMask stageMask = material.GetDrawStageMask();
				while(!stageMask.IsEmpty())
				{
					RendererDrawStageType stageType = stageMask.GetFirstStage();
					
					RenderMaterial* renderMaterial = material.GetRenderMaterial(stageType);
					if(renderMaterial)
					{
						renderItem.m_renderMaterial = renderMaterial;

						// レンダーアイテム追加.
						m_drawStageList.GetDrawStage(stageType)->m_renderItems.push_back(renderItem);
					}

					stageMask.DisableMaskBit(stageType);
				}
			}
		}
	}

} // namespace SI
