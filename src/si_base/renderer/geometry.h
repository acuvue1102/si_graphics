#pragma once

#include "si_base/container/array.h"
#include "si_base/renderer/renderer_common.h"
#include "si_base/gpu/gfx_buffer_ex.h"

namespace SI
{
	class Geometry
	{
	public:
		Geometry(){}
		~Geometry(){}

	private:
		GfxBufferEx m_vertexBuffer;
		GfxBufferEx m_indexBuffer;
	};

} // namespace SI
