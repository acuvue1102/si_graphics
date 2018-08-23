#pragma once

#include <cstdint>

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
	};

} // namespace SI
