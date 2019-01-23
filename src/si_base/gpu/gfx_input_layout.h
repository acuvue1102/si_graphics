#pragma once

#include <cstdint>
#include <string.h>
#include "si_base/gpu/gfx_enum.h"

namespace SI
{
	struct GfxInputElement
	{
		const char* m_semanticsName     = nullptr;
		uint32_t    m_semanticsId       = 0;
		GfxFormat   m_format            = GfxFormat::Unknown;
		uint32_t    m_inputSlot         = 0;
		uint32_t    m_alignedByteOffset = 0; // offsetのバイト数.

		bool operator==(const GfxInputElement& e) const
		{
			if(m_semanticsName)
			{
				if(!e.m_semanticsName) return false;
				if(strcmp(m_semanticsName, e.m_semanticsName)!=0) return false;
			}
			else
			{
				if(e.m_semanticsName) return false;
			}
			
			if(m_semanticsId != e.m_semanticsId) return false;
			if(m_format != e.m_format) return false;
			if(m_inputSlot != e.m_inputSlot) return false;
			if(m_alignedByteOffset != e.m_alignedByteOffset) return false;

			return true;
		}
		
		bool operator!=(const GfxInputElement& e) const
		{
			return !(*this==e);
		}
	};

} // namespace SI
