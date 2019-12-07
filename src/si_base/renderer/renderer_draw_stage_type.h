#pragma once

#include "si_base/misc/bitwise.h"


namespace SI
{
	enum class RendererDrawStageType
	{
		Shadow = 0,
		Opaque,
		Max,
	};
	
	class RendererDrawStageMask
	{
	public:
		RendererDrawStageMask()
			: m_mask(0)
		{
		}

		void Reset()
		{
			m_mask = 0;
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

		RendererDrawStageMask operator|=(RendererDrawStageMask mask)
		{
			m_mask |= mask.m_mask;
			return *this;
		}

		RendererDrawStageMask operator&=(RendererDrawStageMask mask)
		{
			m_mask |= mask.m_mask;
			return *this;
		}

		RendererDrawStageMask operator|(const RendererDrawStageMask& m)
		{
			RendererDrawStageMask ret;
			ret.m_mask = (m_mask | m.m_mask);
			return ret;
		}

		RendererDrawStageMask operator&(const RendererDrawStageMask& m)
		{
			RendererDrawStageMask ret;
			ret.m_mask = (m_mask & m.m_mask);
			return ret;
		}

		uint32_t GetStageCount() const
		{
			return Bitwise::BitCount64(m_mask);
		}

		bool IsEmpty() const
		{
			return (m_mask == (uint64_t)0);
		}

		RendererDrawStageType GetFirstStage() const
		{
			int bit = Bitwise::LSB64(m_mask);
			if(bit<0) return RendererDrawStageType::Max;
			SI_ASSERT(bit<(int)RendererDrawStageType::Max);

			return (RendererDrawStageType)bit;
		}

	private:
		uint64_t m_mask;
	};

} // namespace SI
