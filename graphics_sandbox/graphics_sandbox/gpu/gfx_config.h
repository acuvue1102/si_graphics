#pragma once

#include <stdint.h>

#define SI_USE_DX12 1


#if SI_USE_DX12
#else
#error "Not Supported"
#endif

namespace SI
{
	struct GfxDeviceConfig
	{
		void* m_hWnd           = nullptr;
		uint32_t m_bufferCount = 3u;
		uint32_t m_width       = 1920u;
		uint32_t m_height      = 1080u;
	};
}
