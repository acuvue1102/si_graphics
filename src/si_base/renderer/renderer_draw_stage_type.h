#pragma once

#include "si_base/container/array.h"
#include "si_base/renderer/renderer_common.h"
#include "si_base/renderer/render_item.h"

namespace SI
{
	enum RendererDrawStageType
	{
		RendererDrawStageType_Shadow = 0,
		RendererDrawStageType_Opaque,
		RendererDrawStageType_Max,
	};
	
	class RendererDrawStageMask
	{
	public:
		RendererDrawStageMask()
			: m_mask(0)
		{
		}

		void EnableMaskBit(RendererDrawStageType type)
		{
			m_mask |= (uint64_t)1 << (uint64_t)type;
		}

		void DisableMaskBit(RendererDrawStageType type)
		{
			m_mask &= ~((uint64_t)1 << (uint64_t)type);
		}

		bool CheckEnable(RendererDrawStageType type) const
		{
			return (bool)(m_mask & ((uint64_t)1 << (uint64_t)type));
		}

	private:
		uint64_t m_mask;
	};

} // namespace SI
