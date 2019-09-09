#pragma once

#include <cstdint>
#include "si_base/serialization/reflection.h"

namespace SI
{
	using ObjectIndex     = uint16_t;
	using LongObjectIndex = uint32_t;
	static const ObjectIndex kInvalidObjectIndex = (ObjectIndex)(-1);
	static const LongObjectIndex kInvalidLongObjectIndex = (LongObjectIndex)(-1);
	static const uint32_t kFrameCount = 3;

	struct ObjectIndexRange
	{
		ObjectIndexRange()
			: m_first(kInvalidObjectIndex)
			, m_count(0)
		{
		}

		ObjectIndex m_first;
		uint16_t    m_count;

		SI_REFLECTION(
			SI::ObjectIndexRange,
			SI_REFLECTION_MEMBER_AS_TYPE(m_first, uint16_t),
			SI_REFLECTION_MEMBER(m_count))
	};

} // namespace SI
