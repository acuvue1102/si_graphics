
#include "si_base/renderer/renderer_graphics_state.h"
#include "si_base/misc/hash.h"

namespace SI
{
	void RendererGraphicsStateDesc::GenerateHash()
	{
		const RendererGraphicsStateDescCore& core = *this;

		Hash64Generator generator;
		generator.Add(core);
		m_hash = generator.Generate();
	}

} // namespace SI
