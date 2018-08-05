#pragma once

#include <cstdint>

namespace SI
{
	using ObjectIndex = uint16_t;
	static const ObjectIndex kInvalidObjectIndex = (ObjectIndex)(-1);

	struct ObjectIndexRange
	{
		ObjectIndex m_first = kInvalidObjectIndex;
		uint16_t    m_count = 0;
	};

} // namespace SI
