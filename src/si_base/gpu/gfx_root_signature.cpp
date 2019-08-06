
#include "si_base/core/core.h"
#include "si_base/gpu/dx12/dx12_shader.h"
#include "si_base/gpu/gfx_root_signature.h"
#include "si_base/gpu/gfx_device.h"
#include "si_base/memory/pool_allocator.h"
#include "si_base/gpu/dx12/dx12_root_signature.h"

namespace SI
{
	void* GfxRootSignature::GetNative()
	{
		return m_base->GetNative();
	}

} // namespace SI
