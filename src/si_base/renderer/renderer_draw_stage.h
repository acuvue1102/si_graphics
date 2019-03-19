#pragma once

#include <vector>
#include "si_base/container/array.h"
#include "si_base/renderer/renderer_common.h"
#include "si_base/renderer/render_item.h"
#include "si_base/renderer/renderer_draw_stage_type.h"

namespace SI
{
	class RendererDrawStage
	{
	public:
		RendererDrawStage()
			: m_type(RendererDrawStageType_Max)
			, m_submeshIndex(kInvalidObjectIndex)
			, m_renderItem()
		{
		}

		RendererDrawStage(
			RendererDrawStageType type,
			ObjectIndex submeshIndex)
			: m_type(type)
			, m_submeshIndex(submeshIndex)
			, m_renderItem()
		{
		}
		
		void SetDrawStageType(RendererDrawStageType type)
		{
			m_type = type;
		}

		RendererDrawStageType GetDrawStageType() const
		{
			return m_type;
		}

		RenderItem& GetRenderItem()
		{
			return m_renderItem;
		}

		const RenderItem& GetRenderItem() const
		{
			return m_renderItem;
		}

		void SetSubmeshIndex(ObjectIndex index)
		{
			m_submeshIndex = index;
		}

		ObjectIndex GetSubmeshIndex() const
		{
			return m_submeshIndex;
		}

	private:
		RendererDrawStageType m_type;
		ObjectIndex           m_submeshIndex;
		RenderItem            m_renderItem;
	};

	class RendererDrawStageList
	{
	public:
		RendererDrawStageList()
		{
		}

		void Reserve(uint32_t count)
		{
			m_drawStageList.reserve(count);
		}
		
		RendererDrawStage& Add(RendererDrawStageType type, ObjectIndex submeshIndex)
		{
			SI_ASSERT(!HasDrawStage(type));

			m_mask.EnableMaskBit(type);
			return m_drawStageList.emplace_back(type, submeshIndex);
		}

		bool HasDrawStage(RendererDrawStageType type) const
		{
			return m_mask.CheckEnable(type);
		}

		bool IsEmpty() const
		{
			return m_drawStageList.empty();
		}

		RendererDrawStage* GetDrawStage(RendererDrawStageType type) 
		{
			if(!HasDrawStage(type)) return nullptr; // 使われてないステージは基本的にここで全て弾かれる.

			uint32_t count = (uint32_t)m_drawStageList.size();
			for(uint32_t i=0; i<count; ++i)
			{
				if(m_drawStageList[i].GetDrawStageType() == type)
				{
					return &m_drawStageList[i];
				}
			}

			SI_ASSERT(0); // 来ないはず.
			return nullptr;
		}

	private:
		std::vector<RendererDrawStage> m_drawStageList;
		RendererDrawStageMask          m_mask;
	};

} // namespace SI
