
#include "si_base/renderer/renderer_graphics_state.h"
#include "si_base/misc/hash.h"

namespace SI
{
	void RendererGraphicsStateDesc::UpdateHash()
	{
		const RendererGraphicsStateDescCore& core = *this;

		Hash64Generator generator;
		generator.Add(core);
		m_hash = generator.Generate();
	}
	
	Hash64 RendererGraphicsStateDesc::GetHash()
	{
		if(m_hash==0)
		{
			UpdateHash();
		}
		SI_ASSERT(m_hash!=0);

		return m_hash;
	}

} // namespace SI
