#pragma once

#include "si_base/gpu/gfx_descriptor_heap.h"

namespace SI
{
	class GfxSamplerEx
	{
	public:
		GfxSamplerEx();
		~GfxSamplerEx();

		void Initialize(const GfxSamplerDesc& samplerDesc);
		void Terminate();

		GfxDescriptor GetDescriptor() const{ return m_descriptor; }

	private:
		GfxDescriptor m_descriptor;
	};

} // namespace SI
