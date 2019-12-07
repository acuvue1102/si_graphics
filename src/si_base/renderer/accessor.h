#pragma once

#include <memory>
#include <vector>
#include "si_base/core/assert.h"

#include "si_base/gpu/gfx.h"

namespace SI
{
	class Accessor
	{
	public:
		Accessor()
			: m_buffer()
			, m_count(0)
			, m_format(GfxFormat::Unknown)
		{
		}

		Accessor(
			GfxBuffer buffer,
			uint32_t  count,
			GfxFormat format)
			: m_buffer(buffer)
			, m_count(count)
			, m_format(format)
		{
		}

		~Accessor()
		{
			Release();
		}

		void Release()
		{
			if(m_buffer.IsValid())
			{
				GfxDevice& device = *GfxDevice::GetInstance();
				device.ReleaseBuffer(m_buffer);
			}
		}

		void SetBuffer(GfxBuffer buffer){ m_buffer = buffer; }
		GfxBuffer& GetBuffer() { return m_buffer; }

		void SetFormat(GfxFormat format){ m_format = format; }
		GfxFormat GetFormat() const{ return m_format; }

		void SetCount(uint32_t count){ m_count = count; }
		uint32_t GetCount() const{ return m_count; }

		uint32_t GetSizeInByte() const{ return (uint32_t)((m_count * GetFormatBits(m_format)) / 8); }

	private:
		GfxBuffer            m_buffer;
		uint32_t             m_count;
		GfxFormat            m_format;
	};

} // namespace SI
