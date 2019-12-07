#pragma once

#include <cstdint>
#include <string.h>
#include "si_base/gpu/gfx_enum.h"
#include "si_base/gpu/gfx_utility.h"

namespace SI
{
	struct GfxInputElement
	{
		const char* m_semanticsName;
		uint32_t    m_semanticsId;
		GfxFormat   m_format;
		uint32_t    m_inputSlot;
		uint32_t    m_alignedByteOffset; // offsetのバイト数.

		GfxInputElement()
			: m_semanticsName(nullptr)
			, m_semanticsId(0)
			, m_format(GfxFormat::Unknown)
			, m_inputSlot(0)
			, m_alignedByteOffset(0)
		{
		}

		GfxInputElement(
			const char* semanticsName,
			uint32_t    semanticsId,
			GfxFormat   format,
			uint32_t    inputSlot,
			uint32_t    alignedByteOffset)
			: m_semanticsName(semanticsName)
			, m_semanticsId(semanticsId)
			, m_format(format)
			, m_inputSlot(inputSlot)
			, m_alignedByteOffset(alignedByteOffset)
		{
		}

		GfxInputElement(
			GfxSemantics semantics,
			GfxFormat   format,
			uint32_t    inputSlot,
			uint32_t    alignedByteOffset)
			: m_semanticsName(GetSemanticsName(semantics.m_semanticsType))
			, m_semanticsId(semantics.m_semanticsIndex)
			, m_format(format)
			, m_inputSlot(inputSlot)
			, m_alignedByteOffset(alignedByteOffset)
		{
		}

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
