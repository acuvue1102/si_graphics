// テクスチャの種類別のクラス群
#pragma once

#include <cstdint>
#include "si_base/core/assert.h"
#include "si_base/gpu/gfx_enum.h"
#include "si_base/memory/handle_allocator.h"

namespace SI
{
	class GfxGpuResource
	{
	protected:
		GfxGpuResource(){}
		virtual ~GfxGpuResource(){}
		
	public:
		virtual GfxResourceStates GetInitialResourceStates() const
		{
			SI_ASSERT(0);
			return GfxResourceState::Max;
		}
		
		virtual uint32_t GetResourceStateHandle() const
		{
			SI_ASSERT(0);
			return kInvalidHandle;
		}

		virtual void SetResourceStateHandle(uint32_t /*handle*/)
		{
			SI_ASSERT(0);
		}

		virtual void* GetNativeResource()
		{
			return nullptr;
		}
	};

} // namespace SI
