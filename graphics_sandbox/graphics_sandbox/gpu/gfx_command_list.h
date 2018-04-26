#pragma once

#include "gpu/gfx_config.h"

namespace SI
{
	class BaseCommandList;

	class GfxCommandList
	{
	public:
		GfxCommandList(){}
		virtual ~GfxCommandList(){}

		virtual BaseCommandList* GetBaseCommandList() = 0;
	};

} // namespace SI
