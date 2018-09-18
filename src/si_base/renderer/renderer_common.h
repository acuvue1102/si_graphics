#pragma once

#include <cstdint>
#include "si_base/serialization/reflection.h"

namespace SI
{
	using ObjectIndex     = uint16_t;
	using LongObjectIndex = uint32_t;
	static const ObjectIndex kInvalidObjectIndex = (ObjectIndex)(-1);
	static const LongObjectIndex kInvalidLongObjectIndex = (LongObjectIndex)(-1);

	struct ObjectIndexRange
	{
		ObjectIndex m_first = kInvalidObjectIndex;
		uint16_t    m_count = 0;

		SI_REFLECTION(
			SI::ObjectIndexRange,
			SI_REFLECTION_MEMBER(m_first),
			SI_REFLECTION_MEMBER(m_count))
	};

} // namespace SI
