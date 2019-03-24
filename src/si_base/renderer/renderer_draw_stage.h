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
		{
		}

		RendererDrawStage(
			RendererDrawStageType type)
			: m_type(type)
		{
		}
		
		void Initialize(uint32_t renderItemCount)
		{
			m_renderItems.Setup(renderItemCount);
		}

		void Terminate()
		{
			m_renderItems.Reset();
		}

		void SetDrawStageType(RendererDrawStageType type)
		{
			m_type = type;
		}

		RendererDrawStageType GetDrawStageType() const
		{
			return m_type;
		}

		uint32_t GetRenderItemCount() const
		{
			return m_renderItems.GetItemCount();
		}

		RenderItem& GetRenderItem(uint32_t index)
		{
			return m_renderItems[index];
		}

		const RenderItem& GetRenderItem(uint32_t index) const
		{
			return m_renderItems[index];
		}

	private:
		RendererDrawStageType  m_type;
		SafeArray<RenderItem>  m_renderItems;
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
