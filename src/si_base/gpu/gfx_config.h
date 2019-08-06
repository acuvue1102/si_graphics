#pragma once

#include <cstdint>

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
		
		size_t   m_objectPoolSize = 1024 * 1024;
		size_t   m_tempPoolSize   = 1024 * 1024;

		bool enableDxr = true;
	};
	
	static const uint32_t kMaxNumDescriptors      = 256;
	static const uint32_t kMaxNumDescriptorTables = 32;

	using GpuAddress = uint64_t;
}
