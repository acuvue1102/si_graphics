#pragma once

#include <vector>
#include "si_base/container/array.h"
#include "si_base/renderer/render_item.h"
#include "si_base/renderer/renderer_draw_stage_type.h"

namespace SI
{
	struct RendererDrawStage
	{
		RendererDrawStage()
			: m_type(RendererDrawStageType::Max)
		{
		}

		RendererDrawStage(
			RendererDrawStageType type)
			: m_type(type)
		{
		}

		RendererDrawStageType   m_type = RendererDrawStageType::Max;
		std::vector<RenderItem> m_renderItems;
	};

	class RendererDrawStageList
	{
	public:
		RendererDrawStageList()
		{
		}

		void Clear()
		{
			m_drawStageList.clear();
			m_mask = RendererDrawStageMask();
		}

		void Reserve(uint32_t count)
		{
			m_drawStageList.reserve(count);
		}
		
		RendererDrawStage& Add(RendererDrawStageType type)
		{
			SI_ASSERT(!HasDrawStage(type));

			m_mask.EnableMaskBit(type);
			return m_drawStageList.emplace_back(type);
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
				if(m_drawStageList[i].m_type == type)
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
