#pragma once

#include <cstdint>

namespace SI
{
	template<typename BitType, typename MaskType = uint32_t>
	class EnumFlags
	{
	public:
		EnumFlags()
			: m_mask(0)
		{
		}

		EnumFlags(BitType b)
			: m_mask(static_cast<MaskType>(b))
		{
		}

		explicit EnumFlags(MaskType m)
			: m_mask(m)
		{
		}
		
		inline MaskType GetMask() const
		{
			return m_mask;
		}
		
	public:
		// operators.
		inline EnumFlags<BitType, MaskType> operator|(const EnumFlags<BitType, MaskType>& rhs) const
		{
			return EnumFlags<BitType, MaskType>(m_mask | rhs.m_mask);
		}

		inline EnumFlags<BitType, MaskType> operator&(const EnumFlags<BitType, MaskType>& rhs) const
		{
			return EnumFlags<BitType, MaskType>(m_mask & rhs.m_mask);
		}

		inline EnumFlags<BitType, MaskType> operator^(const EnumFlags<BitType, MaskType>& rhs) const
		{
			return EnumFlags<BitType, MaskType>(m_mask ^ rhs.m_mask);
		}
				
		inline EnumFlags<BitType, MaskType>& operator|=(const EnumFlags<BitType, MaskType>& rhs)
		{
			m_mask |= rhs.m_mask;
			return *this;
		}

		inline EnumFlags<BitType, MaskType>& operator&=(const EnumFlags<BitType, MaskType>& rhs)
		{
			m_mask &= rhs.m_mask;
			return *this;
		}

		inline EnumFlags<BitType, MaskType>& operator^=(const EnumFlags<BitType, MaskType>& rhs)
		{
			m_mask ^= rhs.m_mask;
			return *this;
		}

		inline bool operator==(const EnumFlags<BitType, MaskType>& rhs) const
		{
			return m_mask == rhs.m_mask;
		}

		inline bool operator!=(const EnumFlags<BitType, MaskType>& rhs) const
		{
			return m_mask != rhs.m_mask;
		}
		
		explicit operator bool() const
		{
			return !!m_mask;
		}

	private:
		union
		{
			MaskType m_mask;

			BitType  m_type; // debugerで見た時にタイプが見えるように. 使わない.
		};
	};
}

#define SI_DECLARE_ENUM_FLAGS(FlagsType, FlagType) \
using FlagsType = ::SI::EnumFlags<FlagType>; \
inline FlagsType operator|( FlagType a, FlagType b ) \
{ \
	return FlagsType( a ) | b; \
}
